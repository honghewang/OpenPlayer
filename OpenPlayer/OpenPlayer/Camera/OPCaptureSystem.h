//
//  OPCaptureSystem.h
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/30.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>


NS_ASSUME_NONNULL_BEGIN

//捕获类型
typedef NS_ENUM(int, OPCaptureSystemType){
    OPCaptureSystemTypeVideo = 0,
    OPCaptureSystemTypeAudio,
    OPCaptureSystemTypeAll
};

@protocol OPCaptureSystemDelegate <NSObject>

@optional
- (void)captureSampleBuffer:(CMSampleBufferRef)sampleBuffer type:(OPCaptureSystemType)type;

@end



@interface OPCaptureSystem : NSObject

- (instancetype)initWithType:(OPCaptureSystemType)type;
- (instancetype)init UNAVAILABLE_ATTRIBUTE;

//是否进行
@property (nonatomic, assign) BOOL isRunning;

/**预览层*/
@property (nonatomic, strong) UIView *preview;

@property (nonatomic, weak) id<OPCaptureSystemDelegate> delegate;

/**捕获视频的宽*/
@property (nonatomic, assign, readonly) NSUInteger witdh;
/**捕获视频的高*/
@property (nonatomic, assign, readonly) NSUInteger height;


- (void)start;
- (void)stop;
- (void)changeCamera;

- (void)prepareWithPreviewSize:(CGSize)size;

//授权检测
+ (int)checkMicrophoneAuthor;
+ (int)checkCameraAuthor;

@end

NS_ASSUME_NONNULL_END
