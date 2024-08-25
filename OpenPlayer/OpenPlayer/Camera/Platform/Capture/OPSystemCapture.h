//
//  OPSystemCapture.h
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/13.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

//捕获类型
typedef NS_ENUM(int,OPSystemCaptureType){
    OPSystemCaptureTypeVideo = 0,
    OPSystemCaptureTypeAudio,
    OPSystemCaptureTypeAll
};

NS_ASSUME_NONNULL_BEGIN

@protocol OPSystemCaptureDelegate <NSObject>

@optional
- (void)captureSampleBuffer:(CMSampleBufferRef)sampleBuffer type:(OPSystemCaptureType)type;

@end

@interface OPSystemCapture : NSObject

/**预览层*/
@property (nonatomic, strong) UIView *preview;
@property (nonatomic, weak) id<OPSystemCaptureDelegate> delegate;
/**捕获视频的宽*/
@property (nonatomic, assign, readonly) NSUInteger witdh;
/**捕获视频的高*/
@property (nonatomic, assign, readonly) NSUInteger height;

//是否进行
@property (nonatomic, assign) BOOL isRunning;

- (instancetype)initWithType:(OPSystemCaptureType)type;
- (instancetype)init UNAVAILABLE_ATTRIBUTE;

/** 准备工作(只捕获音频时调用)*/
- (void)prepare;
//捕获内容包括视频时调用（预览层大小，添加到view上用来显示）
- (void)prepareWithPreviewSize:(CGSize)size;

/**开始*/
- (void)start;
/**结束*/
- (void)stop;
/**切换摄像头*/
- (void)changeCamera;


//授权检测
+ (int)checkMicrophoneAuthor;
+ (int)checkCameraAuthor;

@end

NS_ASSUME_NONNULL_END
