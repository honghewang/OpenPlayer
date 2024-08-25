//
//  OPPackageQueue.hpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/15.
//

#ifndef OPPacketQueue_hpp
#define OPPacketQueue_hpp

#include <stdio.h>
#include <list>
#include <thread>
#include <condition_variable>

class AVPacket;

struct OPPacketItem {
    AVPacket *pkt;
    int serial;
    
    OPPacketItem(AVPacket *p, int s):pkt(p), serial(s) {}
    ~OPPacketItem() { }
};

class OPPacketQueue {
public:
    OPPacketQueue() {}
    ~OPPacketQueue() { clear(); }
    
    int count();
    void clear();
    void pushPacket(OPPacketItem packetItem);
    OPPacketItem popPacket();
    OPPacketItem topPacket();
    
private:
    std::list<OPPacketItem> packetList;
    std::mutex packetMutex;
    std::condition_variable packetCon;
};

#endif /* OPPacketQueue_hpp */
