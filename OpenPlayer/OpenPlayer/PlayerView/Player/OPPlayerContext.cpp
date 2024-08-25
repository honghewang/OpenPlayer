//
//  OPPlayerContext.cpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/9.
//

#include "OPPlayerContext.hpp"
#include "OPFFmpegManager.hpp"
#include <thread>
#include <mutex>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/time.h>
}

OPPlayerContext::OPPlayerContext() {}


OPPlayerContext::~OPPlayerContext() {
    if (formatContext) {
        avformat_close_input(&formatContext);
    }
}


int OPPlayerContext::mediaType(int streamType) {
    if (streamType == videoStream) {
        return AVMEDIA_TYPE_VIDEO;
    } else if (streamType == audioStream) {
        return AVMEDIA_TYPE_AUDIO;
    }
    return AVMEDIA_TYPE_UNKNOWN;
}

double OPPlayerContext::getPlayTime() {
    if (startTime != -1) {
        int64_t curTime = av_gettime_relative() - startTime;
        double time = curTime / 1000000.0;
        return time;
    }
    return 0;
}

void OPPlayerContext::setPlayTime(double time) {
    if (time >= 0 && updateTimeFunc && formatContext) {
        double totalTime = formatContext->duration / AV_TIME_BASE;
        updateTimeFunc(time, totalTime);
    }
    startTime = av_gettime_relative() - time * 1000000.0;
}



