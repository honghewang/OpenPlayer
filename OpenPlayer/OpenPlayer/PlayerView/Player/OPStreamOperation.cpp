//
//  OPStreamOperation.cpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/10.
//

#include "OPStreamOperation.hpp"
#include <thread>
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

using namespace std;

void prepareReadFrame(std::weak_ptr<OPStreamOperation> weak_Stream, AVMediaType type) {
    int serail = -1;
    while (true) {
        if (weak_Stream.expired()) {
            // 是否已经停止
            return;
        }
        std::shared_ptr<OPStreamOperation> streamOp = weak_Stream.lock();
        if (streamOp->weakContext.expired()) {
            // 多个线程不同步，weak_Stream 造成无法释放
            return;
        } else {
            OPPlayerState state = streamOp->weakContext.lock()->state;
            if (state == OPPlayerStatePlayClosed || state == OPPlayerStatePlayAbort) {
                return;
            }
            if (state == OPPlayerStatePlayPause) {
                this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
        }
        
        AVCodecContext *codecContext = streamOp->getCodecContext(type);
        // 读取数据
        while (true) {
            if (streamOp->getSerial() != serail && serail != -1) {
                avcodec_flush_buffers(streamOp->getCodecContext(type));
                break;
            }
            AVFrame *frame = av_frame_alloc();
            int frameRet = avcodec_receive_frame(codecContext, frame);
            if (frameRet != 0) {
                av_frame_free(&frame);
                break;
            } else {
                OPFrameItem frameItem = OPFrameItem(frame, serail);
                std::shared_ptr<OPFrameQueue> frameQueue;
                if (type == AVMEDIA_TYPE_VIDEO) {
                    frameQueue = streamOp->videoframeQueue;
                } else if (type == AVMEDIA_TYPE_AUDIO) {
                    frameQueue = streamOp->audioframeQueue;
                }
                while (!frameQueue->pushFrame(frameItem)) {
                    // 如果不能插入，判断是否结束
                    streamOp.reset();
                    streamOp = weak_Stream.lock();
                    if (weak_Stream.expired()) {
                        // push会耗时，这里
                        av_frame_free(&frameItem.frame);
                        return;
                    }
                }
            }
        }
        
        // 此处加一个，防止在一些未知情况
        if (weak_Stream.expired()) {
            return;
        }
        // 送数据解码
        OPPacketItem packetItem(nullptr, -1);
        if (type == AVMEDIA_TYPE_VIDEO) {
            packetItem = streamOp->videoPktQueue->popPacket();
        } else if (type == AVMEDIA_TYPE_AUDIO) {
            packetItem = streamOp->audioPktQueue->popPacket();
        }
        // 这里消耗
        if (packetItem.serial != -1) {
            streamOp->signal();
        }
        
        serail = packetItem.serial;
        if (packetItem.pkt == nullptr || packetItem.serial != streamOp->getSerial()) {
            // 暂无
            if (packetItem.pkt) {
                av_packet_free(&packetItem.pkt);
            }
            continue;
        }
        avcodec_send_packet(codecContext, packetItem.pkt);
        av_packet_unref(packetItem.pkt);
    }
}



OPStreamOperation::~OPStreamOperation() {
    signal();
    if (vCodecContext) {
        avcodec_close(vCodecContext);
        avcodec_free_context(&vCodecContext);
    }
    if (aCodecContext) {
        avcodec_close(aCodecContext);
        avcodec_free_context(&aCodecContext);
    }
    videoframeQueue->clear();
    audioframeQueue->clear();
    videoPktQueue->clear();
    audioPktQueue->clear();
};

bool OPStreamOperation::isEnoughPacket() {
    return videoPktQueue->count() > 10 && audioPktQueue->count() > 10;
}

void OPStreamOperation::clear() {
    videoPktQueue->clear();
    audioPktQueue->clear();
    videoframeQueue->clear();
    audioframeQueue->clear();
    signal();
}

void OPStreamOperation::distributePackets(AVPacket *packet, int mediaType, int serail) {
    if (this->getSerial() != serail) {
        av_packet_free(&packet);
        return;
    }
    if (mediaType == AVMEDIA_TYPE_VIDEO) {
        videoPktQueue->pushPacket(OPPacketItem(packet, serail));
    } else if (mediaType == AVMEDIA_TYPE_AUDIO) {
        audioPktQueue->pushPacket(OPPacketItem(packet, serail));
    }
}

bool OPStreamOperation::isFinish() {
    return false;
}


void OPStreamOperation::readStream(std::weak_ptr<OPStreamOperation> weak_Stream) {
    std::shared_ptr<OPPlayerContext> context = weakContext.lock();
    const AVCodec *vcodec = avcodec_find_decoder(context->formatContext->streams[context->videoStream]->codecpar->codec_id);
    if (!vcodec) {
        return;
    }
    AVCodecContext *vCodecContext = avcodec_alloc_context3(vcodec);
    avcodec_parameters_to_context(vCodecContext, context->formatContext->streams[context->videoStream]->codecpar);
    int ret = avcodec_open2(vCodecContext, nullptr, nullptr);
    if (ret != 0) {
        return;
    }
    {
        std::lock_guard<std::mutex> locker(codecMutex);
        this->vCodecContext = vCodecContext;
    }
    videoFrameThread = std::thread(prepareReadFrame, weak_Stream, AVMEDIA_TYPE_VIDEO);
    videoFrameThread.detach();
    
    const AVCodec *acodec = avcodec_find_decoder(context->formatContext->streams[context->audioStream]->codecpar->codec_id);
    if (!acodec) {
        return;
    }
    AVCodecContext *aCodecContext = avcodec_alloc_context3(acodec);
    avcodec_parameters_to_context(aCodecContext, context->formatContext->streams[context->audioStream]->codecpar);
    int aRet = avcodec_open2(aCodecContext, nullptr, nullptr);
    if (aRet != 0) {
        return;
    }
    {
        std::lock_guard<std::mutex> locker(codecMutex);
        this->aCodecContext = aCodecContext;;
    }
    audioFrameThread = std::thread(prepareReadFrame, weak_Stream, AVMEDIA_TYPE_AUDIO);
    audioFrameThread.detach();
}

int OPStreamOperation::getSerial() {
    if (!weakContext.expired()) {
        return weakContext.lock()->serial;
    }
    return -1;
}

AVCodecContext *OPStreamOperation::getCodecContext(int type) {
    std::lock_guard<std::mutex> locker(codecMutex);
    if (type == AVMEDIA_TYPE_VIDEO) {
        return this->vCodecContext;
    } else {
        return this->aCodecContext;
    }
    
}

void OPStreamOperation::wait() {
    std::unique_lock<std::mutex> locker(threadMutex);
    threadCon.wait_for(locker, std::chrono::milliseconds(20));
}

void OPStreamOperation::signal() {
    std::lock_guard<std::mutex> locker(threadMutex);
    threadCon.notify_one();
}

