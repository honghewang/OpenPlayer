//
//  OPFrameQueue.hpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/15.
//

#ifndef OPFrameQueue_hpp
#define OPFrameQueue_hpp

#include <stdio.h>
#include <list>
#include <thread>
#include <condition_variable>

class AVFrame;

struct OPFrameItem {
    AVFrame *frame;
    int serial;
    
    OPFrameItem(AVFrame *f, int s):frame(f), serial(s) {}
    ~OPFrameItem() {}
};

class OPFrameQueue {
    
public:
    OPFrameQueue() {}
    ~OPFrameQueue() { clear(); }
    
    int count();
    void clear();
    bool pushFrame(OPFrameItem packetItem);
    OPFrameItem popFrame();
    OPFrameItem topFrame();
    
private:
    std::list<OPFrameItem> frameList;
    std::mutex frameMutex;
    std::condition_variable emptyCon;
    std::condition_variable fullCon;
};

#endif /* OPFrameQueue_hpp */
