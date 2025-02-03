//
//  OPFFmpegManager.cpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/10.
//

#include "OPFFmpegManager.hpp"
#include <thread>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

std::once_flag g_ffmpeg;

void initFFmpegUtils() {
    // 初始化网络
//    av_register_all();
    avformat_network_init();
    SDL_SetMainReady();
}


void OPFFmpegManager::initFFmpeg() {
    std::call_once(g_ffmpeg, initFFmpegUtils);
}

