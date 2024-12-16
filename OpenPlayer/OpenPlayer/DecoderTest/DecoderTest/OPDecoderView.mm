//
//  OPDecoderView.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/5/7.
//

#import "OPDecoderView.h"
#import "OPAudioToolBoxPlayer.h"
#import <CoreGraphics/CoreGraphics.h>
#import <Masonry.h>
#include <chrono>
#include <thread>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

@interface OPDecoderView () <OPAudioToolBoxPlayerDelegate>

@property (nonatomic, strong) UIImageView *imgView;

@property (nonatomic, strong) OPAudioToolBoxPlayer *audioPlayer;


@end

@implementation OPDecoderView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self setupUI];
    }
    return self;
}

- (void)setupUI {
//    self.backgroundColor = [UIColor blackColor];
    self.imgView = [[UIImageView alloc] init];
    self.imgView.backgroundColor = [UIColor blackColor];
    [self addSubview:self.imgView];
    CGFloat width = [UIScreen mainScreen].bounds.size.width;
    CGFloat height = width * 9 / 16;
    [self.imgView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.center.mas_equalTo(CGPointZero);
        make.height.mas_equalTo(height);
        make.width.mas_equalTo(width);
    }];
    self.audioPlayer = [[OPAudioToolBoxPlayer alloc] initWithType:@"1k"];
    self.audioPlayer.delegate = self;
}


- (void)playVideoPath:(NSString *)videoPath  {
    __weak OPDecoderView *weakSelf = self;
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        [weakSelf asynPlayVideoPath:videoPath];
    });
}

- (void)stop {
    [_audioPlayer stopPlay];
}

- (void)asynPlayVideoPath:(NSString *)videoPath {
    if (videoPath.length == 0) {
        return;
    }

    // 初始化网络
//    av_register_all();
    avformat_network_init();
    int ret;
    AVFormatContext *formatContext = NULL;
//    AVDictionary *opts = nullptr;
//    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
//    av_dict_set(&opts, "max_delay", "500", 0);
    const char *filename = [videoPath cStringUsingEncoding:NSUTF8StringEncoding];
    ret = avformat_open_input(&formatContext, filename, 0, 0);
    if (ret != 0) {
        NSLog(@"无法打开文件:%d  %s",ret, av_err2str(ret));
        return;
    }
//    ret = avformat_find_stream_info(formatContext, NULL);
//    // 时长
//    long long totalMs = formatContext->duration * 1000 / AV_TIME_BASE;
//    printf("totalMs is %lld", totalMs);
//    // 打印信息
//    av_dump_format(formatContext, 0, filename, 0);
    
    int videoStream = 1;
    int audioStream = 0;
    // 获取音视频流信息
    for (int i = 0; i < formatContext->nb_streams; i++) {
        AVStream *avStream = formatContext->streams[i];
        NSLog(@"stream is %d, rate is %d, width is %d, height is %d", avStream->codecpar->codec_type, avStream->codecpar->sample_rate, avStream->codecpar->width, avStream->codecpar->height);
        if (avStream->avg_frame_rate.den != 0) {
            NSLog(@"frame frame rate is %d", avStream->avg_frame_rate.num / avStream->avg_frame_rate.den);
        }
        if (avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = i;
        } else if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
        }
    }
//    获取Stream
//    av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    
    const AVCodec *vcodec = avcodec_find_decoder(formatContext->streams[videoStream]->codecpar->codec_id);
    if (!vcodec) {
        return;
    }
    AVCodecContext *vCodecContext = avcodec_alloc_context3(vcodec);
    avcodec_parameters_to_context(vCodecContext, formatContext->streams[videoStream]->codecpar);
//    vCodecContext->thread_count = 8;
    ret = avcodec_open2(vCodecContext, nullptr, nullptr);
    if (ret != 0) {
        NSLog(@"解码器无法打开视频");
        return;
    }
    
    const AVCodec *acodec = avcodec_find_decoder(formatContext->streams[audioStream]->codecpar->codec_id);
    if (!acodec) {
        return;
    }
    AVCodecContext *aCodecContext = avcodec_alloc_context3(acodec);
    avcodec_parameters_to_context(aCodecContext, formatContext->streams[audioStream]->codecpar);
//    aCodecContext->thread_count = 8;
    ret = avcodec_open2(aCodecContext, nullptr, nullptr);
    if (ret != 0) {
        NSLog(@"解码器无法打开音频");
        return;
    }
    
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    SwsContext *vctx = nullptr;
    unsigned char *rgb = nullptr;
    
    unsigned char *pcm = nullptr;
    SwrContext *actx = swr_alloc();
    int64_t targetLayout = av_get_default_channel_layout(2);
    int64_t srcLayout = av_get_default_channel_layout(aCodecContext->channels);
    actx = swr_alloc_set_opts(actx,
                              targetLayout,
                              AV_SAMPLE_FMT_S16,
                              aCodecContext->sample_rate,
                              srcLayout,
                              aCodecContext->sample_fmt,
                              aCodecContext->sample_rate, 0, 0);
    ret = swr_init(actx);
    if (ret != 0) {
        return;
    }
    
    NSString *name = @"";
    while (YES) {
        int frameRet = av_read_frame(formatContext, packet);
        NSLog(@"+++++++++++++++++++++++++++++packet, %p", packet);
        if (frameRet != 0) {
            break;
        }
        AVCodecContext *context = nullptr;
        if (packet->stream_index == videoStream) {
            context = vCodecContext;
            name = @"图像";
        } else if (packet->stream_index == audioStream) {
            context = aCodecContext;
            name = @"音频";
        }
        double ration = [OPDecoderView ration2Double:formatContext->streams[packet->stream_index]->time_base];
        NSLog(@"%@, size:%d, pts:%f, dts:%f", name, packet->size, packet->pts * ration, packet->dts * ration);
        
        frameRet = avcodec_send_packet(context, packet);
        av_packet_unref(packet);
        if (frameRet != 0) {
            NSLog(@"发送失败");
            return;
        }
        while (YES) {
            frameRet = avcodec_receive_frame(context, frame);
            if (frameRet != 0) {
//                NSLog(@"%@接受失败，%s", name, av_err2str(frameRet));
                break;
            }
//            NSLog(@"receive frame %d, frameline: %d", frame->format, frame->linesize[0]);
            
            if (context == vCodecContext) {
                NSLog(@"+++++++++++++++++++++++++++++视频开始");
                vctx = sws_getCachedContext(vctx,
                                            frame->width,
                                            frame->height,
                                            (AVPixelFormat)frame->format,
                                            frame->width,
                                            frame->height,
                                            AV_PIX_FMT_RGBA,
                                            SWS_BILINEAR,
                                            0, 0, 0);
                if (vctx) {
                    if (!rgb) rgb = new unsigned char[frame->width*frame->height * 4];
                    uint8_t *data[2] = { 0 };
                    data[0] = rgb;
                    int lines[2] = {0};
                    lines[0] = frame->width * 4;
                    int height = sws_scale(vctx, frame->data, frame->linesize, 0, frame->height, data, lines);
                    NSLog(@"sws_scale %d", height);
                    UIImage *image = [self imageWithRGBA:rgb width:frame->width height:frame->height];
                    __weak OPDecoderView *weakSelf = self;
                    dispatch_async(dispatch_get_main_queue(), ^{
                        weakSelf.imgView.image = image;
                    });
                    av_frame_unref(frame);
                }
            } else if (context == aCodecContext) {
                NSLog(@"+++++++++++++++++++++++++++++音频开始");
                uint8_t *data[2] = { 0 };
                NSInteger length = frame->nb_samples * 2 * 2;
                if (!pcm) pcm = new uint8_t[length];
                data[0] = pcm;
                frameRet = swr_convert(actx,
                    data, frame->nb_samples,
                    (const uint8_t**)frame->data,frame->nb_samples
                    );

                NSLog(@"swr_convert %d", frameRet);
                av_frame_unref(frame);
                NSData *pcmData = [NSData dataWithBytes:pcm length:length];
                [self.audioPlayer appendData:pcmData totalDatalength:length endFlag:NO];
            }
        }
    }
    
    av_frame_free(&frame);
    av_packet_free(&packet);
    if (rgb) {
        free(rgb);
    }
    
    if (actx) {
        swr_close(actx);
        swr_free(&actx);
    }
    if (vctx) {
        sws_freeContext(vctx);
    }
    
    avcodec_close(aCodecContext);
    avcodec_free_context(&aCodecContext);
    avcodec_close(vCodecContext);
    avcodec_free_context(&vCodecContext);
    if (formatContext) {
        avformat_close_input(&formatContext);
    }
    
}


+ (double)ration2Double:(AVRational)r {
    return r.den == 0 ? 0 : (double)r.num / r.den;
}

- (UIImage *)imageWithRGBA:(unsigned char *)rgbaData width:(NSInteger)width height:(NSInteger)height {
    // 创建位图上下文
    size_t bitsPerComponent = 8; // RGBA每个分量的位数
    size_t bytesPerRow = width * 4; // 每行字节数（RGBA，每个分量1字节）
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB(); // 创建RGB颜色空间
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast; // 字节顺序和alpha通道信息
    CGContextRef context = CGBitmapContextCreate((void *)rgbaData, width, height, bitsPerComponent, bytesPerRow, colorSpace, bitmapInfo);
    // 如果需要，可以在这里对上下文进行进一步的绘制操作
    // ...
      
    // 从上下文创建CGImage
    CGImageRef imageRef = CGBitmapContextCreateImage(context);
      
    // 释放资源
    CGColorSpaceRelease(colorSpace);
    CGContextRelease(context);
      
    UIImage *image = [UIImage imageWithCGImage:imageRef];
    CGImageRelease(imageRef);
    return image;
}

- (void)readlyToPlay {
    [self.audioPlayer startPlay];
}

-  (void)updateBufferPositon:(float)bufferPosition {
    NSLog(@"-----------------%f", bufferPosition);
}

@end
