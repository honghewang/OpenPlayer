//
//  OPCameraViewController.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/29.
//

#import "OPCameraViewController.h"
#import "OPCaptureSystem.h"
#import "OPCapturePreview.h"
#import "OPCaptureSystem.h"
#include "OPCVUtils.hpp"
#include "OPFilterFacePointsFilter.hpp"
#include "OPFilterFaceMaskFilter.hpp"
#include "OPFilterTextureFilter.hpp"

@interface OPCameraViewController () <OPCaptureSystemDelegate>

@property (nonatomic, strong) OPCaptureSystem *captureSystem;

@property (nonatomic, strong) OPCapturePreview *playerView;

@property (nonatomic, strong) UIButton *startBtn;

@property (nonatomic, strong) dispatch_queue_t refreshQueue;

@end

@implementation OPCameraViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setupUI];
    [self setupCapture];
}

- (void)setupUI {
    self.view.backgroundColor = [UIColor whiteColor];
}

- (void)setupCapture {
    [OPCaptureSystem checkCameraAuthor];
    self.captureSystem = [[OPCaptureSystem alloc] initWithType:OPCaptureSystemTypeVideo];
    CGSize size = CGSizeMake(self.view.frame.size.width, self.view.frame.size.height);
    [self.captureSystem prepareWithPreviewSize:CGSizeMake(size.width / 3, size.height / 3)];
    self.captureSystem.delegate = self;
    
    self.playerView = [[OPCapturePreview alloc] initWithAsyn:YES];
    self.playerView.frame = CGRectMake(0, 0, size.width, size.height);
    self.playerView.backgroundColor = [UIColor blackColor];
//    std::shared_ptr<OPFilterRenderFilterLink> pointLink = std::make_shared<OPFilterRenderFilterLink>();
//    std::shared_ptr<OPFilterFacePointsFilter> facePointsFilter = std::make_shared<OPFilterFacePointsFilter>();
//    pointLink->input = facePointsFilter;
//    pointLink->output = facePointsFilter;
//    [self.playerView setFilterLinks:pointLink];
    [self.view addSubview:self.playerView];
    
    
    UIButton *startBtn = [[UIButton alloc] init];
    [startBtn setBackgroundColor:UIColor.grayColor];
    [self.view addSubview:startBtn];
    startBtn.backgroundColor = [UIColor grayColor];
    [startBtn setTitle:@"开始" forState:UIControlStateNormal];
    [startBtn addTarget:self action:@selector(startCapture:) forControlEvents:UIControlEventTouchUpInside];
    CGFloat offsetX = (self.view.frame.size.width - 70) / 2;
    startBtn.frame = CGRectMake(offsetX, size.height - 70, 70, 50);
    self.startBtn = startBtn;
}

//开始捕捉
- (void)startCapture:(id)sender {
    if (self.captureSystem.isRunning) {
        [self.captureSystem stop];
        [self.startBtn setTitle:@"开始" forState:UIControlStateNormal];
    } else {
        [self.captureSystem start];
        [self.startBtn setTitle:@"停止" forState:UIControlStateNormal];
    }
}

#pragma mark--delegate
//捕获音视频回调
- (void)captureSampleBuffer:(CMSampleBufferRef)sampleBuffer type:(OPCaptureSystemType)type {
    if (type == OPCaptureSystemTypeAudio) {
        // 音频处理
        // 获取PCM数据
        //        size_t size = CMSampleBufferGetTotalSampleSize(sampleBuffer);
        //        //分配空间
        //        int8_t *audio_data = (int8_t *)malloc(size);
        //        memset(audio_data, 0, size);
        //
        //        //获取CMBlockBuffer, 这里面保存了PCM数据
        //        CMBlockBufferRef blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
        //        //将数据copy到我们分配的空间中
        //        CMBlockBufferCopyDataBytes(blockBuffer, 0, size, audio_data);
        //        //PCM data->NSData
        //        NSData *data = [NSData dataWithBytes:audio_data length:size];
        //        free(audio_data);

    } else {
        // 视频处理
        CFRetain(sampleBuffer);
        dispatch_async(self.refreshQueue, ^{
            CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
            [self.playerView loadImgBuffer:imageBuffer];
            CFRelease(sampleBuffer);
        });

    }
}

- (dispatch_queue_t)refreshQueue {
    if (!_refreshQueue) {
        _refreshQueue = dispatch_queue_create("refresh Queue", NULL);
    }
    return _refreshQueue;
}





@end
