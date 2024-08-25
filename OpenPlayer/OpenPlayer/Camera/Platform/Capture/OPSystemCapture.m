//
//  OPSystemCapture.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/13.
//

#import "OPSystemCapture.h"

@interface OPSystemCapture () <AVCaptureAudioDataOutputSampleBufferDelegate, AVCaptureVideoDataOutputSampleBufferDelegate>

@property (nonatomic, assign) OPSystemCaptureType capture;
//会话
@property (nonatomic, strong) AVCaptureSession *captureSession;
//代理队列
@property (nonatomic, strong) dispatch_queue_t captureQueue;

//音频设备
@property (nonatomic, strong) AVCaptureDeviceInput *audioInputDevice;
//输出数据接收
@property (nonatomic, strong) AVCaptureAudioDataOutput *audioDataOutput;
@property (nonatomic, strong) AVCaptureConnection *audioConnection;

//当前使用的视频设备
@property (nonatomic, weak) AVCaptureDeviceInput *videoInputDevice;
//前后摄像头
@property (nonatomic, strong) AVCaptureDeviceInput *frontCamera;
@property (nonatomic, strong) AVCaptureDeviceInput *backCamera;
//输出数据接收
@property (nonatomic, strong) AVCaptureVideoDataOutput *videoDataOutput;
@property (nonatomic, strong) AVCaptureConnection *videoConnection;
//预览层
@property (nonatomic, strong) AVCaptureVideoPreviewLayer *preLayer;
@property (nonatomic, assign) CGSize prelayerSize;

@end


@implementation OPSystemCapture

- (instancetype)initWithType:(OPSystemCaptureType)type {
    self = [super init];
    if (self) {
        _capture = type;
    }
    return self;
}

//准备捕获
- (void)prepare {
    [self prepareWithPreviewSize:CGSizeZero];
}

//准备捕获(视频/音频)
- (void)prepareWithPreviewSize:(CGSize)size {
    _prelayerSize = size;
    if (_capture == OPSystemCaptureTypeAudio) {
        [self setupAudio];
    } else if (_capture == OPSystemCaptureTypeVideo) {
        [self setupVideo];
    } else if (_capture == OPSystemCaptureTypeAll) {
        [self setupAudio];
        [self setupVideo];
    }
}

#pragma mark - Control start/stop capture or change camera
- (void)start {
    if (!self.isRunning) {
        self.isRunning = YES;
        [self.captureSession startRunning];
    }
}

- (void)stop {
    if (self.isRunning) {
        self.isRunning = NO;
        [self.captureSession stopRunning];
    }
}

- (void)changeCamera {
    [self switchCamera];
}

-(void)switchCamera{
    [self.captureSession beginConfiguration];
    [self.captureSession removeInput:self.videoInputDevice];
    if ([self.videoInputDevice isEqual:self.frontCamera]) {
        self.videoInputDevice = self.backCamera;
    } else {
        self.videoInputDevice = self.frontCamera;
    }
    [self.captureSession addInput:self.videoInputDevice];
    [self.captureSession commitConfiguration];
}

#pragma mark-init Audio/video
- (void)setupAudio {
    //麦克风设备
    AVCaptureDevice *audioDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeAudio];
    //将audioDevice ->AVCaptureDeviceInput 对象
    self.audioInputDevice = [AVCaptureDeviceInput deviceInputWithDevice:audioDevice error:nil];
    //音频输出
    self.audioDataOutput = [[AVCaptureAudioDataOutput alloc] init];
    [self.audioDataOutput setSampleBufferDelegate:self queue:self.captureQueue];
    //配置
    [self.captureSession beginConfiguration];
    if ([self.captureSession canAddInput:self.audioInputDevice]) {
        [self.captureSession addInput:self.audioInputDevice];
    }
    if([self.captureSession canAddOutput:self.audioDataOutput]){
        [self.captureSession addOutput:self.audioDataOutput];
    }
    [self.captureSession commitConfiguration];
    
    self.audioConnection = [self.audioDataOutput connectionWithMediaType:AVMediaTypeAudio];
}

- (void)setupVideo {
    //所有video设备
    AVCaptureDeviceDiscoverySession *captureDeviceDiscoverySession = [AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeBuiltInWideAngleCamera]
                                          mediaType:AVMediaTypeVideo
                                           position: AVCaptureDevicePositionUnspecified];
    NSArray *videoDevices = [captureDeviceDiscoverySession devices];
    //前置摄像头
    self.frontCamera = [AVCaptureDeviceInput deviceInputWithDevice:videoDevices.lastObject error:nil];
    self.backCamera = [AVCaptureDeviceInput deviceInputWithDevice:videoDevices.firstObject error:nil];
    //设置当前设备为前置
    self.videoInputDevice = self.backCamera;
    //视频输出
    self.videoDataOutput = [[AVCaptureVideoDataOutput alloc] init];
    [self.videoDataOutput setSampleBufferDelegate:self queue:self.captureQueue];
    [self.videoDataOutput setAlwaysDiscardsLateVideoFrames:YES];
    //kCVPixelBufferPixelFormatTypeKey它指定像素的输出格式，这个参数直接影响到生成图像的成功与否
   // kCVPixelFormatType_420YpCbCr8BiPlanarFullRange  YUV420格式.
    
    [self.videoDataOutput setVideoSettings:@{
                                             (__bridge NSString *)kCVPixelBufferPixelFormatTypeKey:@(kCVPixelFormatType_420YpCbCr8BiPlanarFullRange)
                                             }];
    //配置
    [self.captureSession beginConfiguration];
    if ([self.captureSession canAddInput:self.videoInputDevice]) {
        [self.captureSession addInput:self.videoInputDevice];
    }
    if([self.captureSession canAddOutput:self.videoDataOutput]){
        [self.captureSession addOutput:self.videoDataOutput];
    }
    //分辨率
    [self setVideoPreset];
    [self.captureSession commitConfiguration];
    //commit后下面的代码才会有效
    self.videoConnection = [self.videoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    //设置视频输出方向
    self.videoConnection.videoOrientation = AVCaptureVideoOrientationPortrait;
    //fps
    [self updateFps:25];
    //设置预览
    [self setupPreviewLayer];
}

/**设置分辨率**/
- (void)setVideoPreset {
//    if ([self.captureSession canSetSessionPreset:AVCaptureSessionPreset1920x1080])  {
//        self.captureSession.sessionPreset = AVCaptureSessionPreset1920x1080;
//        _witdh = 1080; _height = 1920;
//    } else if ([self.captureSession canSetSessionPreset:AVCaptureSessionPreset1280x720]) {
//        self.captureSession.sessionPreset = AVCaptureSessionPreset1280x720;
//        _witdh = 720; _height = 1280;
//    }else{
//        self.captureSession.sessionPreset = AVCaptureSessionPreset640x480;
//        _witdh = 480; _height = 640;
//    }
    self.captureSession.sessionPreset = AVCaptureSessionPreset640x480;
    _witdh = 480; _height = 640;
}

- (void)updateFps:(NSInteger)fps {
    //获取当前capture设备
    AVCaptureDeviceDiscoverySession *captureDeviceDiscoverySession = [AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeBuiltInWideAngleCamera]
                                          mediaType:AVMediaTypeVideo
                                           position: AVCaptureDevicePositionUnspecified];
    NSArray *videoDevices = [captureDeviceDiscoverySession devices];
    
    //遍历所有设备（前后摄像头）
    NSInteger curFPS = 120;
    for (AVCaptureDevice *vDevice in videoDevices) {
        //获取当前支持的最大fps
        float maxRate = [(AVFrameRateRange *)[vDevice.activeFormat.videoSupportedFrameRateRanges objectAtIndex:0] maxFrameRate];
        //如果想要设置的fps小于或等于做大fps，就进行修改
        if (maxRate >= fps && maxRate < curFPS) {
            curFPS = fps;
            //实际修改fps的代码
            if ([vDevice lockForConfiguration:NULL]) {
                vDevice.activeVideoMinFrameDuration = CMTimeMake(10, (int)(fps * 10));
                vDevice.activeVideoMaxFrameDuration = vDevice.activeVideoMinFrameDuration;
                [vDevice unlockForConfiguration];
            }
        }
    }
}
/**设置预览层**/
- (void)setupPreviewLayer{
    self.preLayer = [AVCaptureVideoPreviewLayer layerWithSession:self.captureSession];
    self.preLayer.frame = CGRectMake(0, 0, self.prelayerSize.width, self.prelayerSize.height);
    //设置满屏
    self.preLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    [self.preview.layer addSublayer:self.preLayer];
}

#pragma mark-懒加载
- (AVCaptureSession *)captureSession{
    if (!_captureSession) {
        _captureSession = [[AVCaptureSession alloc] init];
    }
    return _captureSession;
}

- (dispatch_queue_t)captureQueue{
    if (!_captureQueue) {
        _captureQueue = dispatch_queue_create("TMCapture Queue", NULL);
    }
    return _captureQueue;
}

- (UIView *)preview{
    if (!_preview) {
        _preview = [[UIView alloc] init];
    }
    return _preview;
}


- (void)dealloc{
    NSLog(@"capture销毁。。。。");
    [self destroyCaptureSession];
}

#pragma mark-销毁会话
- (void)destroyCaptureSession{
    if (self.captureSession) {
        if (_capture == OPSystemCaptureTypeAudio) {
            [self.captureSession removeInput:self.audioInputDevice];
            [self.captureSession removeOutput:self.audioDataOutput];
        } else if (_capture == OPSystemCaptureTypeVideo) {
            [self.captureSession removeInput:self.videoInputDevice];
            [self.captureSession removeOutput:self.videoDataOutput];
        } else if (_capture == OPSystemCaptureTypeAll) {
            [self.captureSession removeInput:self.audioInputDevice];
            [self.captureSession removeOutput:self.audioDataOutput];
            [self.captureSession removeInput:self.videoInputDevice];
            [self.captureSession removeOutput:self.videoDataOutput];
        }
    }
    self.captureSession = nil;
}

#pragma mark-输出代理
- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection{
    if (connection == self.audioConnection) {
        [_delegate captureSampleBuffer:sampleBuffer type:OPSystemCaptureTypeAudio];
    } else if (connection == self.videoConnection) {
        [_delegate captureSampleBuffer:sampleBuffer type:OPSystemCaptureTypeVideo];
//        CMSampleTimingInfo timingInfo;
//        memset(&timingInfo, 0, sizeof(timingInfo));
//        OSStatus status = CMSampleBufferGetSampleTimingInfo(sampleBuffer, 0, &timingInfo);
//        if (status == 0) {
//            // 成功获取时间信息
//            CMTime timeStamp = timingInfo.presentationTimeStamp;
//            NSLog(@"1++++++++++++++++++++%f", 1.0 * timeStamp.value/timeStamp.timescale);
//        }
    }
}

- (void)captureOutput:(AVCaptureOutput *)output didDropSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection {
    NSLog(@"++++++++++++didDropSampleBuffer");
}


#pragma mark-授权相关
/**
 *  麦克风授权
 *  0 ：未授权 1:已授权 -1：拒绝
 */
+ (int)checkMicrophoneAuthor{
    int result = 0;
    //麦克风
    AVAudioSessionRecordPermission permissionStatus = [[AVAudioSession sharedInstance] recordPermission];
    switch (permissionStatus) {
        case AVAudioSessionRecordPermissionUndetermined:
            //    请求授权
            [[AVAudioSession sharedInstance] requestRecordPermission:^(BOOL granted) {
            }];
            result = 0;
            break;
        case AVAudioSessionRecordPermissionDenied://拒绝
            result = -1;
            break;
        case AVAudioSessionRecordPermissionGranted://允许
            result = 1;
            break;
        default:
            break;
    }
    return result;
}
/**
 *  摄像头授权
 *  0 ：未授权 1:已授权 -1：拒绝
 */
+ (int)checkCameraAuthor{
    int result = 0;
    AVAuthorizationStatus videoStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    switch (videoStatus) {
        case AVAuthorizationStatusNotDetermined://第一次
            //    请求授权
            [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
                
            }];
            break;
        case AVAuthorizationStatusAuthorized://已授权
            result = 1;
            break;
        default:
            result = -1;
            break;
    }
    return result;
}

@end
