//
//  OPAudioController.hpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/21.
//

#ifndef OPAudioController_hpp
#define OPAudioController_hpp

#include <stdio.h>
#include <thread>
#include "OPFrameQueue.hpp"
#include "OPPlayerContext.hpp"
extern "C" {
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/samplefmt.h>
}

typedef struct OPAudioParams {
    int freq;
    int channels;
    int64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size;
    int bytes_per_sec;
} AudioParams;

class AVCodecContext;
class OPFrameQueue;
class SwrContext;
class OPPlayer;

class OPAudioController {
public:
    OPAudioController(std::weak_ptr<OPPlayerContext> wc): weakContext(wc)  {}
    ~OPAudioController() {
        if (audioBuf) {
            free(audioBuf);
        }
        if (swrCtx) {
            swr_close(swrCtx);
            swr_free(&swrCtx);
        }
    }
    
    int play(AVCodecContext *avctx, OPPlayer *opaque);
    void pause(bool pause);
    void stop();
    
    void getAudioData(std::shared_ptr<OPFrameQueue> audioframeQueue, uint8_t *stream, int length, int serial);
    int parseFrameItem(OPFrameItem item, int64_t dec_channel_layout, int wanted_nb_samples, int data_size, bool *isFrameData);
    
    OPAudioParams tgtParams; // 目标参数
    SwrContext *swrCtx = nullptr;
    OPAudioParams srcParams; // 源参数，避免SwrContext 重复
    
    int bufSerial = -1;
    int audioBufStart = 0;
    uint8_t *audioBuf = nullptr;
    unsigned int audioBufSize = 0; /* in bytes */
    bool audioCached;
    
    int buffSize = 0;
    
private:
    
    uint8_t *tmpAudioBuf = nullptr;
    unsigned int tmpAudioBufSize = 0; /* in bytes */
    std::weak_ptr<OPPlayerContext> weakContext;
};

#endif /* OPAudioController_hpp */
