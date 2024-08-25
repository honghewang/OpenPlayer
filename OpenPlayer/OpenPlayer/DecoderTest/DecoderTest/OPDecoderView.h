//
//  OPDecoderView.h
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/5/7.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface OPDecoderView : UIView

- (void)playVideoPath:(NSString *)videoPath;

- (void)stop;

@end

NS_ASSUME_NONNULL_END
