//
//  OPPlayer.cpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/9.
//

#include "OPPlayer.hpp"
#include <thread>
#include <chrono>
#include "OPFFmpegManager.hpp"
#include "OPPtrManager.hpp"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

using namespace std;

static int interrupt_callback(void *context) {
    // TODO
    return 0;
}

inline double r2d(AVRational r) {
    return r.den == 0 ? 0 : (double)r.num / r.den;
}

void readThreadOp(OPPlayer *player) {
    // 初始化
    bool isAbort = false;
    OPPtrManager::getState(player, [&] (bool isReleased) {
        if (!isReleased) {
            string playingUrl = player->context->url;
            if (playingUrl.size() == 0) {
                isAbort = true;
                return;
            }
            OPFFmpegManager::initFFmpeg();
            if (player->prepare(playingUrl)) {
                // 开启播放
                player->context->state = OPPlayerStatePlaying;
            } else {
                player->context->state = OPPlayerStatePlayError;
                isAbort = true;
            }
        }
    });
    if (isAbort) {
        return;
    }
    while (true) {
        std::shared_ptr<OPPlayerContext> context;
        std::shared_ptr<OPStreamOperation> streamOperation;
        std::shared_ptr<OPAudioController> audioController;
        OPPtrManager::getState(player, [&] (bool isReleased) {
            if (!isReleased) {
                context = player->context;
                streamOperation = player->streamOperation;
                audioController = player->audioController;
            }
        });
        
        if (context.get() == nullptr || streamOperation.get() == nullptr || audioController.get() == nullptr) {
            // 是否已经停止
            return;
        }
        
        OPPlayerState state = context->state;
        if (state == OPPlayerStatePlayAbort || state == OPPlayerStatePlayClosed) {
            // 停止
            return;
        }
        
        if (context->seekTime >= 0) {
            int64_t seekTime = context->seekTime * AV_TIME_BASE;
            int ret = avformat_seek_file(context->formatContext, -1, INT64_MIN, seekTime, INT64_MAX, 0);
            context->serial++;
            streamOperation->clear();
            if (ret < 0) {
                av_log(NULL, AV_LOG_WARNING, "could not seek to position\n");
            }
            context->seekTime = -1;
        }
        
        int serial = context->serial;
        if (state == OPPlayerStatePlayFinish) {
            audioController->pause(true);
            context->state = OPPlayerStatePlayClosed;
        } else if (streamOperation->isEnoughPacket() || state != OPPlayerStatePlaying) {
            // 等待，此处提前释放
            streamOperation->wait();
        } else {
            if (state == OPPlayerStatePlaying && context->queueState != OPQueueStatePacketDone) {
                AVPacket *packet = av_packet_alloc();
                int ret = av_read_frame(context->formatContext, packet);
                if (ret == 0) {
                    streamOperation->distributePackets(packet, context->mediaType(packet->stream_index), serial);
                } else {
                    av_packet_free(&packet);
                    if (ret == AVERROR_EOF) {
//                        avcodec_flush_buffers;
                        // packets发送完成
                        context->queueState = OPQueueStatePacketDone;
                    }
                }
            } else {
                // 等待其他操作
                this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
}

void OPPlayer::prepareToPlay(std::string url) {
    context->url = url;
    // 没有初始化
    read_thread = std::thread(readThreadOp, this);
    read_thread.detach();
}

void OPPlayer::pause() {
    if (context->state == OPPlayerStatePlaying) {
        context->state = OPPlayerStatePlayPause;
        audioController->pause(true);
        videoController->pause(true);
    } else if (context->state == OPPlayerStatePlayPause) {
        context->state = OPPlayerStatePlaying;
        audioController->pause(false);
        videoController->pause(false);
    }
}

void OPPlayer::stop() {
    
}

void OPPlayer::seek(double time) {
    context->seekTime = time;
}

void OPPlayer::setRefreshFunc(std::function<void(int width, int height, char *yData, char *uData, char *vData)> func) {
    context->refreshFunc = func;
}
void OPPlayer::setUpdateTimeFunc(std::function<void(double cur, double totalTime)> func) {
    context->updateTimeFunc = func;
}

bool OPPlayer::prepare(std::string filename) {
    const char *fileUrl = filename.c_str();
    if (context->formatContext) {
        avformat_close_input(&context->formatContext);
        context->formatContext = nullptr;
    }
    int ret = avformat_open_input(&context->formatContext, fileUrl, 0, 0);
    if (ret != 0) {
        printf("无法打开文件:%s, error is %s",fileUrl, av_err2str(ret));
        return false;
    }
    // 设置回调
    context->formatContext->interrupt_callback.callback = interrupt_callback;
    context->formatContext->opaque = context.get();
    // 在某些情况下，输入流可能缺少这些时间戳，或者时间戳可能不正确。通过设置 AVFMT_FLAG_GENPTS，FFmpeg 能够尝试自动修复这些问题
    context->formatContext->flags |= AVFMT_FLAG_GENPTS;
    // 这些信息不是音视频数据本身的一部分，但可能对解码、显示或其他处理步骤有用
    av_format_inject_global_side_data(context->formatContext);
    // 这里可以调用avformat_find_stream_info，设置一些配置，探测数据流相关一些探测流信息时所需的选项
    
    if (context->formatContext->pb) {
        // 是否为文件结尾为0
        context->formatContext->pb->eof_reached = 0;
    }
    
    // 这里判断是否是ogg格式
    if (context->seek_by_bytes < 0) {
        context->seek_by_bytes = !!(context->formatContext->iformat->flags & AVFMT_TS_DISCONT) && strcmp("ogg", context->formatContext->iformat->name);
    }
    context->max_frame_duration = (context->formatContext->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;
        
    //  获取Stream
    // av_guess_sample_aspect_ratio 猜测视频宽高比
    context->videoStream = av_find_best_stream(context->formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream *vStream = context->formatContext->streams[context->videoStream];
    context->vTimeRation = r2d(vStream->time_base);
    context->audioStream = av_find_best_stream(context->formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    AVStream *aStream = context->formatContext->streams[context->audioStream];
    context->aTimeRation = r2d(aStream->time_base);
    
    // 此处判断模式
    
    // 准备读取
    context->serial++;
    streamOperation->readStream(std::weak_ptr<OPStreamOperation>(streamOperation));
    videoController->play(streamOperation->getCodecContext(AVMEDIA_TYPE_VIDEO), this);
    audioController->play(streamOperation->getCodecContext(AVMEDIA_TYPE_AUDIO), this);
    return true;
}
