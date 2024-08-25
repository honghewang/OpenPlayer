//
//  OPPackageQueue.cpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/15.
//

#include "OPPacketQueue.hpp"
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
}

static OPPacketItem emptyPacketItem(nullptr, -1);


int OPPacketQueue::count() {
    std::lock_guard<std::mutex> locker(packetMutex);
    return (int)packetList.size();
}


void OPPacketQueue::clear() {
    std::lock_guard<std::mutex> locker(packetMutex);
    for (auto item : packetList) {
        if (item.pkt) {
            av_packet_free(&item.pkt);
        }
    }
    packetList.clear();
}


void OPPacketQueue::pushPacket(OPPacketItem packetItem) {
    std::lock_guard<std::mutex> locker(packetMutex);
    packetList.push_back(packetItem);
}

OPPacketItem OPPacketQueue::popPacket() {
    std::lock_guard<std::mutex> locker(packetMutex);
    if (packetList.size() > 0) {
        OPPacketItem packetItem = packetList.front();
        packetList.pop_front();
        return packetItem;
    }
    return emptyPacketItem;
}

OPPacketItem OPPacketQueue::topPacket() {
    std::lock_guard<std::mutex> locker(packetMutex);
    if (packetList.size() > 0) {
        OPPacketItem packetItem = packetList.front();
        return packetItem;
    }
    return emptyPacketItem;
}
