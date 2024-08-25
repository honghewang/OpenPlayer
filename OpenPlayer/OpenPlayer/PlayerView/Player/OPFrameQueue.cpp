//
//  OPFrameQueue.cpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/15.
//

#include "OPFrameQueue.hpp"
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
}

static int OPFrameQueueCount = 10;
static OPFrameItem emptyFrameItem(nullptr, -1);


int OPFrameQueue::count() {
    std::lock_guard<std::mutex> locker(frameMutex);
    return (int)frameList.size();
}

void OPFrameQueue::clear() {
    std::lock_guard<std::mutex> locker(frameMutex);
    for (auto item : frameList) {
        if (item.frame) {
            av_frame_free(&item.frame);
        }
    }
    frameList.clear();
    fullCon.notify_one();
}

bool OPFrameQueue::pushFrame(OPFrameItem packetItem) {
    std::unique_lock<std::mutex> locker(frameMutex);
    if (frameList.size() == OPFrameQueueCount) {
        if (frameList.back().serial != packetItem.serial) {
            frameList.clear();
        } else {
            fullCon.wait_for(locker, std::chrono::milliseconds(10));
        }
    }
    if (frameList.size() == OPFrameQueueCount) {
        return false;
    } else {
        frameList.push_back(packetItem);
        // 这里释放空锁
        emptyCon.notify_one();
        return true;
    }
}

OPFrameItem OPFrameQueue::popFrame() {
    std::unique_lock<std::mutex> locker(frameMutex);
    // 这里可能播放完了
    if (frameList.size() > 0) {
        OPFrameItem frameItem = frameList.front();
        frameList.pop_front();
        // 这里释放满锁
        fullCon.notify_one();
        return frameItem;
    }
    return emptyFrameItem;
}

OPFrameItem OPFrameQueue::topFrame() {
    std::unique_lock<std::mutex> locker(frameMutex);
    if (frameList.size() > 0) {
        OPFrameItem frameItem = frameList.front();
        return frameItem;
    }
    return emptyFrameItem;
}

