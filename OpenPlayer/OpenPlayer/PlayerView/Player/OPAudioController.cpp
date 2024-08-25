//
//  OPAudioController.cpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/21.
//

#include "OPAudioController.hpp"
#include <SDL_audio.h>
#include <SDL_main.h>
#include <vector>
#include "OPStreamOperation.hpp"
#include "OPPlayer.hpp"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}


//static int64_t audio_callback_time;
/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

static void sdl_audio_callback(void *opaque, Uint8 *stream, int len);


int OPAudioController::play(AVCodecContext *avctx, OPPlayer *opaque) {
    int wanted_sample_rate    = avctx->sample_rate;
    int wanted_nb_channels    = avctx->channels;
    int64_t wanted_channel_layout = avctx->channel_layout;
    
    SDL_SetMainReady();
    SDL_AudioSpec wanted_spec, spec;
    
    const char *env;
    static const int next_nb_channels[] = {0, 0, 1, 6, 2, 6, 4, 6};
    static const int next_sample_rates[] = {0, 44100, 48000, 96000,192000};
    int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) -1;

    env = SDL_getenv("SDL_AUDIO_CHANNELS");
    if (env) { // 若环境变量有设置，优先从环境变量取得声道数和声道布局
        wanted_nb_channels = atoi(env);
        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
    }
    if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) {
        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
        wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
    }
    // 根据channel_layout获取nb_channels，当传⼊参数wanted_nb_channels不匹配时，此处会作修正
    wanted_nb_channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
    wanted_spec.channels = wanted_nb_channels;
    wanted_spec.freq = wanted_sample_rate;
    if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) {
        av_log(NULL, AV_LOG_ERROR, "Invalid sample rate or channelcount!\n");
        return -1;
    }
    while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq)
        next_sample_rate_idx--; // 从采样率数组中找到第⼀个不⼤于传⼊参数wanted_sample_rate的值
    // ⾳频采样格式有两⼤类型：planar和packed，假设⼀个双声道⾳频⽂件，⼀个左声道采样点记作L，⼀个右声道采样点记作R，则：
    // planar存储格式：(plane1)LLLLLLLL...LLLL (plane2)RRRRRRRR...RRRR
    // packed存储格式：(plane1)LRLRLRLR...........................LRLR
    // 在这两种采样类型下，⼜细分多种采样格式，如AV_SAMPLE_FMT_S16、AV_SAMPLE_FMT_S16P等，
    // 注意SDL2.0⽬前不⽀持planar格式
    // channel_layout是int64_t类型，表示⾳频声道布局，每bit代表⼀个特定的声道，参考channel_layout.h中的定义，⼀⽬了然
    // 数据量(bits/秒) = 采样率(Hz) * 采样深度(bit) * 声道数
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.silence = 0;
    /*
     * ⼀次读取多⻓的数据
     * SDL_AUDIO_MAX_CALLBACKS_PER_SEC⼀秒最多回调次数，避免频繁的回调
     * Audio buffer size in samples (power of 2)
     */
    wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
    wanted_spec.callback = sdl_audio_callback;
    wanted_spec.userdata = opaque;
    // 打开⾳频设备并创建⾳频处理线程。期望的参数是wanted_spec，实际得到的硬件参数是spec
    // 1) SDL提供两种使⾳频设备取得⾳频数据⽅法：
    // a. push，SDL以特定的频率调⽤回调函数，在回调函数中取得⾳频数据
    // b. pull，⽤户程序以特定的频率调⽤SDL_QueueAudio()，向⾳频设备提供数据。此种情况wanted_spec.callback=NULL
    // 2) ⾳频设备打开后播放静⾳，不启动回调，调⽤SDL_PauseAudio(0)后启动回调，开始正常播放⾳频
    // SDL_OpenAudioDevice()第⼀个参数为NULL时，等价于SDL_OpenAudio()
    while (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
        av_log(NULL, AV_LOG_WARNING, "SDL_OpenAudio (%d channels, %d Hz): %s\n",
               wanted_spec.channels, wanted_spec.freq, SDL_GetError());
        wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
        if (!wanted_spec.channels) {
            wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
            wanted_spec.channels = wanted_nb_channels;
            if (!wanted_spec.freq) {
                av_log(NULL, AV_LOG_ERROR,
                       "No more combinations to try, audio open failed\n");
                return -1;
            }
        }
        wanted_channel_layout = av_get_default_channel_layout(wanted_spec.channels);
    }
    // 检查打开⾳频设备的实际参数：采样格式
    if (spec.format != AUDIO_S16SYS) {
        av_log(NULL, AV_LOG_ERROR,"SDL advised audio format %d is not supported!\n", spec.format);
        return -1;
    }
    // 检查打开⾳频设备的实际参数：声道数
    if (spec.channels != wanted_spec.channels) {
        wanted_channel_layout = av_get_default_channel_layout(spec.channels);
        if (!wanted_channel_layout) {
            av_log(NULL, AV_LOG_ERROR,"SDL advised channel count %d is not supported!\n", spec.channels);
            return -1;
        }
    }
    
    // wanted_spec是期望的参数，spec是实际的参数，wanted_spec和spec都是SDL中的结构。
    // 此处audioParams是FFmpeg中的参数，输出参数供上级函数使⽤
    // audioParams保存的参数，就是在做重采样的时候要转成的格式。
    tgtParams.fmt = AV_SAMPLE_FMT_S16;
    tgtParams.freq = spec.freq;
    tgtParams.channel_layout = wanted_channel_layout;
    tgtParams.channels = spec.channels;
    /* tgtParams.frame_size这⾥只是计算⼀个采样点占⽤的字节数 */
    tgtParams.frame_size = av_samples_get_buffer_size(NULL,tgtParams.channels,
                                                             1, tgtParams.fmt, 1);
    tgtParams.bytes_per_sec = av_samples_get_buffer_size(NULL, tgtParams.channels,
                                                         tgtParams.freq,
                                                         tgtParams.fmt, 1);
    if (tgtParams.bytes_per_sec <= 0 || tgtParams.frame_size <= 0) {
        av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
        return -1;
    }
    
    SDL_PauseAudio(0);
    // ⽐如2帧数据，⼀帧就是1024个采样点， 1024*2*2 * 2 = 8096字节
    buffSize = spec.size;
    return spec.size; /* 硬件内部缓存的数据字节, samples * channels *byte_per_sample */
}

void OPAudioController::stop() {
    SDL_CloseAudio();
}

void OPAudioController::pause(bool pause) {
    int tag = pause ? 1 : 0;
    SDL_PauseAudio(tag);
}


void OPAudioController::getAudioData(std::shared_ptr<OPFrameQueue> audioframeQueue, uint8_t *stream, int length, int serial) {
    if (serial != this->bufSerial) {
        this->bufSerial = -1;
        this->audioBufStart = 0;
        this->audioBufSize = 0;
        if (this->audioBuf) {
            free(this->audioBuf);
        }
        this->audioBuf = nullptr;
    }
    memset(stream, 0, length);
    int targetStart = 0;
    
    if (this->audioBufSize - this->audioBufStart >= length) {
        memcpy(stream + targetStart, this->audioBuf + this->audioBufStart, length);
        this->audioBufStart += length;
        return;
    }
    
    if (this->audioBuf != nullptr && this->audioBufStart < this->audioBufSize) {
        memcpy(stream + targetStart, this->audioBuf + this->audioBufStart, this->audioBufSize - this->audioBufStart);
        targetStart = this->audioBufSize - this->audioBufStart;
    }
    
    this->bufSerial = -1;
    this->audioBufStart = 0;
    this->audioBufSize = 0;
    if (this->audioBuf) {
        free(this->audioBuf);
    }
    this->audioBuf = nullptr;
        
    while (targetStart < length) {
        OPFrameItem item = audioframeQueue->popFrame();
        while (item.serial != -1 && serial != item.serial) {
            av_frame_free(&item.frame);
            item = audioframeQueue->popFrame();
        }
        // 暂时没有数据
        if (item.serial == -1 && item.frame == nullptr) {
            if (!weakContext.expired() && weakContext.lock()->queueState == OPQueueStatePacketDone) {
                weakContext.lock()->state = OPPlayerStatePlayFinish;            }
            return;
        }
        
        int64_t dec_channel_layout;
        int wanted_nb_samples;
        
        AVSampleFormat fmt = (AVSampleFormat)item.frame->format;
        int data_size = av_samples_get_buffer_size(NULL, item.frame->channels,
                                               item.frame->nb_samples, fmt, 1);
        dec_channel_layout =
            (item.frame->channel_layout && item.frame->channels == av_get_channel_layout_nb_channels(item.frame->channel_layout)) ?
            item.frame->channel_layout : av_get_default_channel_layout(item.frame->channels);
        
        // TODO 如果不是音频同步，设计到变频
        wanted_nb_samples = item.frame->nb_samples;
        
        bool isFrameData;
        int ret = parseFrameItem(item, dec_channel_layout, wanted_nb_samples, data_size, &isFrameData);
        if (!isFrameData) {
            // 如果是用frame数据，不能释放
            av_frame_free(&item.frame);
        }
        
        
        if (ret < 0) {
            return;
        } else {
            if (this->tmpAudioBufSize <= length - targetStart) {
                memcpy(stream + targetStart, this->tmpAudioBuf, this->tmpAudioBufSize);
                targetStart = targetStart + this->tmpAudioBufSize;
                
                if (isFrameData) {
                    // 此处释放frame数据
                    av_frame_free(&item.frame);
                } else {
                    if (tmpAudioBuf) {
                        av_free(tmpAudioBuf);
                    }
                }
                
            } else {
                int size = (length - targetStart);
                memcpy(stream + targetStart, this->tmpAudioBuf, (length - targetStart));
                targetStart = length;
                if (isFrameData) {
                    // 多余数据保存
                    audioBuf = (uint8_t *)av_malloc(size);
                    memcpy(audioBuf, this->tmpAudioBuf + (length - targetStart), size);
                    audioBufSize = tmpAudioBufSize - size;
                    audioBufStart = 0;
                    // 此处释放frame数据
                    av_frame_free(&item.frame);
                } else {
                    audioBuf = tmpAudioBuf;
                    audioBufSize = tmpAudioBufSize;
                    audioBufStart = this->tmpAudioBufSize - (length - targetStart);
                }
            }
            tmpAudioBuf = nullptr;
            tmpAudioBufSize = 0;
            bufSerial = item.serial;
        }
    }
    
}

int OPAudioController::parseFrameItem(OPFrameItem item, int64_t dec_channel_layout, int wanted_nb_samples, int data_size, bool *isFrameData) {
    int resampled_data_size;
    if (item.frame->format != tgtParams.fmt ||
        dec_channel_layout != tgtParams.channel_layout ||
        item.frame->sample_rate != tgtParams.freq ||
        wanted_nb_samples != item.frame->nb_samples) {
        *isFrameData = false;
        if (!swrCtx || srcParams.channel_layout != dec_channel_layout ||
            srcParams.channels != item.frame->channels ||
            srcParams.freq != item.frame->sample_rate ||
            srcParams.fmt != (AVSampleFormat)item.frame->format) {
            swr_free(&swrCtx);
            swrCtx = swr_alloc_set_opts(NULL,
                                        tgtParams.channel_layout,
                                        tgtParams.fmt,
                                        tgtParams.freq,
                                        dec_channel_layout,
                                        (AVSampleFormat)item.frame->format,
                                        item.frame->sample_rate,
                                             0, NULL);
            if (!swrCtx || swr_init(swrCtx) < 0) {
                av_log(NULL, AV_LOG_ERROR,
                       "Cannot create sample rate converter for conversion of %d Hz %d channels to %d Hz %s %d channels!\n",
                        item.frame->sample_rate, item.frame->channels,
                       tgtParams.freq, av_get_sample_fmt_name(tgtParams.fmt), tgtParams.channels);
                swr_free(&swrCtx);
                return -1;
            }
            srcParams.channel_layout = dec_channel_layout;
            srcParams.channels       = item.frame->channels;
            srcParams.freq = item.frame->sample_rate;
            srcParams.fmt = (AVSampleFormat)item.frame->format;
        }
    
        const uint8_t **in = (const uint8_t **)item.frame->extended_data;
        
        uint8_t **out = &tmpAudioBuf;
        int out_count = (int)wanted_nb_samples * tgtParams.freq / item.frame->sample_rate + 256;
        int out_size  = av_samples_get_buffer_size(NULL, tgtParams.channels, out_count, tgtParams.fmt, 0);
        int len2;
        if (out_size < 0) {
            av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
            return -1;
        }
        if (wanted_nb_samples != item.frame->nb_samples) {
            if (swr_set_compensation(swrCtx, (wanted_nb_samples - item.frame->nb_samples) * tgtParams.freq / item.frame->sample_rate,
                                        wanted_nb_samples * tgtParams.freq / item.frame->sample_rate) < 0) {
                av_log(NULL, AV_LOG_ERROR, "swr_set_compensation() failed\n");
                return -1;
            }
        }
        av_fast_malloc(&tmpAudioBuf, &tmpAudioBufSize, out_size);
        if (!tmpAudioBuf)
            return AVERROR(ENOMEM);
        len2 = swr_convert(swrCtx, out, out_count, in, item.frame->nb_samples);
        if (len2 < 0) {
            av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
            return -1;
        }
        if (len2 == out_count) {
            av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
            if (swr_init(swrCtx) < 0)
                swr_free(&swrCtx);
        }
        resampled_data_size = len2 * tgtParams.channels * av_get_bytes_per_sample(tgtParams.fmt);
    } else {
        *isFrameData = true;
        tmpAudioBuf = item.frame->data[0];
        resampled_data_size = data_size;
    }
    tmpAudioBufSize = resampled_data_size;
    return resampled_data_size;
}


static void sdl_audio_callback(void *opaque, Uint8 *stream, int len) {
    
    std::shared_ptr<OPStreamOperation> streamOp;
    std::shared_ptr<OPAudioController> audioController;
    OPPtrManager::getState(opaque, [&] (bool isReleased){
        if (!isReleased) {
            // 延长生命周期
            if (!((OPPlayer *)opaque)->streamOperation->weakContext.expired()) {
                streamOp = ((OPPlayer *)opaque)->streamOperation;
                audioController = ((OPPlayer *)opaque)->audioController;
            }
        }
    });
    if (streamOp.get() && audioController.get()) {
        int serial = streamOp->getSerial();
        audioController->getAudioData(streamOp->audioframeQueue, stream, len, serial);
        // 同步时间
        if (!audioController->audioCached) {
            audioController->audioCached = true;
        } else {
            OPFrameItem item = streamOp->audioframeQueue->topFrame();
            if (item.serial != -1 && item.frame) {
                std::shared_ptr<OPPlayerContext> context = streamOp->weakContext.lock();
                double curTime = item.frame->pts * context->aTimeRation - ((double)(2 * audioController->buffSize + audioController->audioBufSize - audioController->audioBufStart)) / audioController->tgtParams.bytes_per_sec;
                context->setPlayTime(curTime);
            }
        }
    }
}


