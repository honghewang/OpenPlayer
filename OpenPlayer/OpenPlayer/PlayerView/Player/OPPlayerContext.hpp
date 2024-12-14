//
//  OPPlayerContext.hpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/9.
//

#ifndef OPPlayerContext_hpp
#define OPPlayerContext_hpp

#include <stdio.h>
#include <thread>
#include <string>

class AVFormatContext;
class AVPacket;
class AVCodecContext;

/// 音视频同步方式
enum OPPlayerSyncType {
    OPPlayerSyncTypeAudioMaster,  // 音频同步
    OPPlayerSyncTypeVideoMaster,  // 视频同步
    OPPlayerSyncTypeExternalClock // 外部时钟同步
};

enum OPPlayerState {
    OPPlayerStateUnknown = 0,
    OPPlayerStatePlaying,
    OPPlayerStatePlayFinish,
    OPPlayerStatePlayClosed,
    OPPlayerStatePlayPause,
    OPPlayerStatePlayError,
    OPPlayerStatePlayAbort,
};


enum OPQueueState {
    OPQueueStateUnknown = 0,
    OPQueueStatePacketDone,
    OPQueueStateFrameDone
};

class OPPlayerContext {
    
public:
    OPPlayerContext();
    ~OPPlayerContext();
    
    /// 类型转换
    int mediaType(int streamType);
    // 当前播放tag
    std::atomic<int> serial{0};
    
    double getPlayTime();
    void setPlayTime(double time);
    
    std::string url;
    AVFormatContext *formatContext = nullptr;
    
    int videoStream = 0;
    int audioStream = 1;
    
    double vTimeRation = 0;
    double aTimeRation = 0;
    
    std::atomic<OPPlayerState> state;
    std::atomic<OPQueueState> queueState;
    std::atomic<OPPlayerSyncType> syncType;
    
    std::atomic<double> seekTime{-1.0};
    
    // 设置刷新
    std::function<void(int width, int height, char *yData, char *uData, char *vData)> refreshFunc;
    std::function<void(double cur, double totalTime)> updateTimeFunc;
    
    
private:
    std::atomic<int64_t> startTime{-1};
};

#endif /* OPPlayerContext_hpp */
