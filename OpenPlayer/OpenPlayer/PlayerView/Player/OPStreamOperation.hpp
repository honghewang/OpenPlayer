//
//  OPStreamOperation.hpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/10.
//

#ifndef OPStreamOperation_hpp
#define OPStreamOperation_hpp

#include <stdio.h>
#include <thread>
#include "OPPacketQueue.hpp"
#include "OPFrameQueue.hpp"
#include "OPPlayerContext.hpp"
#include "OPPtrManager.hpp"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

class AVPacket;
class AVCodecContext;

class OPStreamOperation {
public:
    OPStreamOperation(std::weak_ptr<OPPlayerContext> wc): weakContext(wc) {};
    ~OPStreamOperation();
    
    bool isEnoughPacket();
    void clear();
    
    void wait(); // 阻塞
    void signal();
    
    void distributePackets(AVPacket *packet, int mediaType, int serail);
    
    bool isFinish();
    
    void readStream(std::weak_ptr<OPStreamOperation> weak_Stream);
    
    int getSerial();
    
    std::weak_ptr<OPPlayerContext> weakContext;
    
    std::shared_ptr<OPPacketQueue> videoPktQueue = std::make_shared<OPPacketQueue>();
    std::shared_ptr<OPPacketQueue> audioPktQueue = std::make_shared<OPPacketQueue>();
    
    std::shared_ptr<OPFrameQueue> videoframeQueue = std::make_shared<OPFrameQueue>();
    std::shared_ptr<OPFrameQueue> audioframeQueue = std::make_shared<OPFrameQueue>();
    
    AVCodecContext *getCodecContext(int type);
    
private:
    std::thread videoFrameThread;
    std::thread audioFrameThread;
    
    // 解码上下文
    AVCodecContext *vCodecContext = nullptr;
    AVCodecContext *aCodecContext = nullptr;
    std::mutex codecMutex;
    
    // 主流程
    std::mutex threadMutex;
    std::condition_variable threadCon;
        
};

#endif /* OPStreamOperation_hpp */
