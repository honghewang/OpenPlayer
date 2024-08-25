//
//  OPPlayerEAGLLayer.h
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/16.
//

#import <QuartzCore/QuartzCore.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface OPPlayerEAGLLayer : CAEAGLLayer

@property (nonatomic, assign) CVPixelBufferRef pixelBuffer;

- (id)initWithFrame:(CGRect)frame;

@end

NS_ASSUME_NONNULL_END
