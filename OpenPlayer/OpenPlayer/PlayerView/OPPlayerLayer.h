//
//  OPPlayerLayer.h
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/15.
//

#import <QuartzCore/QuartzCore.h>

NS_ASSUME_NONNULL_BEGIN

@interface OPPlayerLayer : CAEAGLLayer

- (void)refreshWithWidth:(int)width height:(int)height yData:(char *)yData uData:(char *)uData vData:(char *)vData;


@end

NS_ASSUME_NONNULL_END
