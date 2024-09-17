//
//  OPCaptureSystem.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/30.
//

#import "OPCaptureSystem.h"

@interface OPCaptureSystem () <AVCaptureVideoDataOutputSampleBufferDelegate, AVCaptureAudioDataOutputSampleBufferDelegate>

@property (nonatomic, strong) AVCaptureSession *captureSession;
//代理队列
@property (nonatomic, strong) dispatch_queue_t captureQueue;


@property (nonatomic, assign) OPCaptureSystemType capture;

@property (nonatomic, strong) AVCaptureVideoPreviewLayer *preLayer;
@property (nonatomic, assign) CGSize prelayerSize;

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

@end

@implementation OPCaptureSystem

- (instancetype)initWithType:(OPCaptureSystemType)type {
    self = [super init];
    if (self) {
        _capture = type;
    }
    return self;
}

- (void)prepare {
    [self prepareWithPreviewSize:CGSizeZero];
}

//准备捕获(视频/音频)
- (void)prepareWithPreviewSize:(CGSize)size {
    _prelayerSize = size;
    if (_capture == OPCaptureSystemTypeAudio) {
        [self setupAudio];
    } else if (_capture == OPCaptureSystemTypeVideo) {
        [self setupVideo];
    } else if (_capture == OPCaptureSystemTypeAll) {
        [self setupAudio];
        [self setupVideo];
    }
}

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

- (void)switchCamera {
    [self.captureSession beginConfiguration];
    [self.captureSession removeInput:self.videoInputDevice];
    if ([self.videoInputDevice isEqual: self.frontCamera]) {
        self.videoInputDevice = self.backCamera;
    }else{
        self.videoInputDevice = self.frontCamera;
    }
    [self.captureSession addInput:self.videoInputDevice];
    [self.captureSession commitConfiguration];
}

#pragma mark - init Audio/video
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
    NSArray *videoDevices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
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
    
    [self updateFps:25];
    //设置预览
//    [self setupPreviewLayer];
}

#pragma mark-懒加载
- (AVCaptureSession *)captureSession {
    if (!_captureSession) {
        _captureSession = [[AVCaptureSession alloc] init];
    }
    return _captureSession;
}

- (dispatch_queue_t)captureQueue {
    if (!_captureQueue) {
        _captureQueue = dispatch_queue_create("capture Queue", NULL);
    }
    return _captureQueue;
}

- (UIView *)preview {
    if (!_preview) {
        _preview = [[UIView alloc] init];
        _preview.backgroundColor = [UIColor blackColor];
    }
    return _preview;
}

- (void)dealloc {
    [self destroyCaptureSession];
}

#pragma mark-销毁会话
-(void) destroyCaptureSession{
    if (self.captureSession) {
        if (self.capture == OPCaptureSystemTypeAudio) {
            [self.captureSession removeInput:self.audioInputDevice];
            [self.captureSession removeOutput:self.audioDataOutput];
        }else if (self.capture == OPCaptureSystemTypeVideo) {
            [self.captureSession removeInput:self.videoInputDevice];
            [self.captureSession removeOutput:self.videoDataOutput];
        }else if (self.capture == OPCaptureSystemTypeAll) {
            [self.captureSession removeInput:self.audioInputDevice];
            [self.captureSession removeOutput:self.audioDataOutput];
            [self.captureSession removeInput:self.videoInputDevice];
            [self.captureSession removeOutput:self.videoDataOutput];
        }
    }
    self.captureSession = nil;
}

#pragma mark- 输出代理
- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection{
    if (connection == self.audioConnection) {
        [_delegate captureSampleBuffer:sampleBuffer type:OPCaptureSystemTypeAudio];
    } else if (connection == self.videoConnection) {
        [_delegate captureSampleBuffer:sampleBuffer type:OPCaptureSystemTypeVideo];
    }
}


#pragma mark-授权相关

+ (int)checkMicrophoneAuthor{
    int result = 0;
    //麦克风
    AVAudioSessionRecordPermission permissionStatus = [[AVAudioSession sharedInstance] recordPermission];
    switch (permissionStatus) {
        case AVAudioSessionRecordPermissionUndetermined:
            // 请求授权
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

+ (int)checkCameraAuthor {
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


/**设置分辨率**/
- (void)setVideoPreset {
//    if ([self.captureSession canSetSessionPreset:AVCaptureSessionPreset1920x1080])  {
//        self.captureSession.sessionPreset = AVCaptureSessionPreset1920x1080;
//        _witdh = 1080; _height = 1920;
//    } else if ([self.captureSession canSetSessionPreset:AVCaptureSessionPreset1280x720]) {
//        self.captureSession.sessionPreset = AVCaptureSessionPreset1280x720;
//        _witdh = 720; _height = 1280;
//    } else {
//        self.captureSession.sessionPreset = AVCaptureSessionPreset640x480;
//        _witdh = 480; _height = 640;
//    }
    self.captureSession.sessionPreset = AVCaptureSessionPreset640x480;
    _witdh = 480; _height = 640;
}

- (void)updateFps:(NSInteger)fps{
    //获取当前capture设备
    NSArray *videoDevices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    //遍历所有设备（前后摄像头）
    for (AVCaptureDevice *vDevice in videoDevices) {
        //获取当前支持的最大fps
        float maxRate = [(AVFrameRateRange *)[vDevice.activeFormat.videoSupportedFrameRateRanges objectAtIndex:0] maxFrameRate];
        //如果想要设置的fps小于或等于做大fps，就进行修改
        if (maxRate >= fps) {
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
- (void)setupPreviewLayer {
    self.preLayer = [AVCaptureVideoPreviewLayer layerWithSession:self.captureSession];
    self.preLayer.frame =  CGRectMake(0, 0, self.prelayerSize.width, self.prelayerSize.height);
    //设置满屏
    self.preLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    [self.preview.layer addSublayer:self.preLayer];
}


@end
