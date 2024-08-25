//
//  OPVideoController.cpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/26.
//

#include "OPVideoController.hpp"
#include "OPFrameQueue.hpp"
#include "OPPlayer.hpp"
#include <thread>
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}

using namespace std;


void videoRefresh(OPPlayer *player, std::weak_ptr<OPPlayerContext> wc) {
    while (true) {
        if (wc.expired()) {
            return;
        }
        
        std::shared_ptr<OPFrameQueue> videoFrameQueue;
        std::shared_ptr<OPVideoController> videoController;
        OPPtrManager::getState(player, [&] (bool isReleased){
            if (!isReleased) {
                // 延长生命周期
                videoFrameQueue = player->streamOperation->videoframeQueue;
                videoController = player->videoController;
            }
        });
        if (!videoFrameQueue.get() || !videoController.get() || videoController->weakContext.expired()) {
            return;
        }
        
        if (videoController->weakContext.lock()->state == OPPlayerStatePlayPause) {
            this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        std::shared_ptr<OPPlayerContext> context = wc.lock();
        std::function<void(int width, int height, char *yData, char *uData, char *vData)> refreshFunc = context->refreshFunc;
        OPFrameItem item = videoFrameQueue->popFrame();
        int serail = context->serial;
        while (item.serial != -1 && serail != item.serial) {
            av_frame_free(&item.frame);
            item = videoFrameQueue->popFrame();
        }
        // 暂时没有数据
        if (item.serial == -1) {
            if (context->state == OPPlayerStatePlayAbort) {
                return;
            } else {
                continue;
            }
        }
        
        // TODO 如果是rgba也不需要转
        if (item.frame->format != AV_PIX_FMT_YUV420P) {
            // 此处使用sws转换
        }
        
        if (refreshFunc == nullptr) {
            av_frame_free(&item.frame);
            return;
        } else {
            double now = context->getPlayTime();
            double frameStart = item.frame->pts * context->vTimeRation;
            double frameEnd = frameStart + item.frame->pkt_duration * context->vTimeRation;
            if (frameEnd < now) {
                av_frame_free(&item.frame);
                continue;
            }
            
            context.reset();
            int sleepTime = (frameStart - now) * 1000;
            if (sleepTime > 0) {
                int maxTime = 60;
                if (sleepTime > maxTime) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(maxTime));
                }
                
            }
            // TODO 此处应该复制
            // 此处有休眠
            if (wc.expired()) {
                av_frame_free(&item.frame);
                return;
            } else {
                if (item.serial != wc.lock()->serial) {
                    av_frame_free(&item.frame);
                    continue;
                }
                int width = item.frame->width;
                int height = item.frame->height;
                refreshFunc(width, height, (char *)item.frame->data[0], (char *)item.frame->data[1], (char *)item.frame->data[2]);
                av_frame_free(&item.frame);
            }
        }
    }
}

OPVideoController::~OPVideoController() {
    if (vctx) {
        sws_freeContext(vctx);
    }
}

void OPVideoController::play(AVCodecContext *avctx, OPPlayer *player) {
    videoRefreshThread = std::thread(videoRefresh, player, weakContext);
    videoRefreshThread.detach();
}

void OPVideoController::pause(bool pause) {
    
}

void OPVideoController::stop() {
    
}


