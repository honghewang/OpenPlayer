//
//  OPCameraViewController.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/11.
//

#import "OPCameraViewController.h"
#import "OPSystemCapture.h"
#import "OPPlayerEAGLLayer.h"
#import <Masonry/Masonry.h>

@interface OPCameraViewController () <OPSystemCaptureDelegate>

@property (nonatomic, strong) OPSystemCapture *capture;

@property (nonatomic, strong) OPPlayerEAGLLayer *playerLayer;

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
    [OPSystemCapture checkCameraAuthor];
    _capture = [[OPSystemCapture alloc] initWithType:OPSystemCaptureTypeVideo];
    CGSize size = CGSizeMake(self.view.frame.size.width / 2, self.view.frame.size.height / 2);
    [_capture prepareWithPreviewSize:size];  //捕获视频时传入预览层大小
    _capture.preview.frame = CGRectMake(0, 0, size.width, size.height);
    [self.view addSubview:_capture.preview];
    self.capture.delegate = self;
    
    _playerLayer = [[OPPlayerEAGLLayer alloc] initWithFrame:CGRectMake(size.width, 0, size.width, size.height)];
    _playerLayer.backgroundColor = [UIColor blackColor].CGColor;
    [self.view.layer addSublayer:_playerLayer];
    
    UIButton *startBtn = [[UIButton alloc] init];
    [self.view addSubview:startBtn];
    startBtn.backgroundColor = [UIColor grayColor];
    [startBtn setTitle:@"开始" forState:UIControlStateNormal];
    [startBtn addTarget:self action:@selector(startCapture:) forControlEvents:UIControlEventTouchUpInside];
    CGFloat offsetX = (self.view.frame.size.width - 70) / 2;
    startBtn.frame = CGRectMake(offsetX, size.height + 5, 70, 50);
    self.startBtn = startBtn;
}


//开始捕捉
- (void)startCapture:(id)sender {
    if (self.capture.isRunning) {
        [self.capture stop];
        [self.startBtn setTitle:@"开始" forState:UIControlStateNormal];
    } else {
        [self.capture start];
        [self.startBtn setTitle:@"停止" forState:UIControlStateNormal];
    }
}

#pragma mark--delegate
//捕获音视频回调
- (void)captureSampleBuffer:(CMSampleBufferRef)sampleBuffer type:(OPSystemCaptureType)type {
    if (type == OPSystemCaptureTypeAudio) {
        // 音频处理
        //获取pcm数据大小
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
        CFRetain(sampleBuffer);
        dispatch_async(self.refreshQueue, ^{
            CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
            self.playerLayer.pixelBuffer = imageBuffer;
            CFRelease(sampleBuffer);
        });
//        CMTime currentTime = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);
//        NSLog(@"+++++++++++++ %f", 1.0 * currentTime.value / currentTime.timescale);
        // 视频处理
//        CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
//        self.playerLayer.pixelBuffer = imageBuffer;        
//        VTCompressionSessionEncodeFrame
        
//        CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, [[GPUImageContext sharedImageProcessingContext] coreVideoTextureCache], cameraFrame, NULL, GL_TEXTURE_2D, GL_LUMINANCE, bufferWidth, bufferHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0, &luminanceTextureRef);
//
//        luminanceTexture = CVOpenGLESTextureGetName(luminanceTextureRef);
    }
}

- (dispatch_queue_t)refreshQueue{
    if (!_refreshQueue) {
        _refreshQueue = dispatch_queue_create("refresh Queue", NULL);
    }
    return _refreshQueue;
}

@end
