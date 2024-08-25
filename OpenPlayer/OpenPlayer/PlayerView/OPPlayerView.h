//
//  OPPlayerView.h
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/6.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface OPPlayerView : UIView

/// 准备播放
- (void)prepareToPlay:(NSString *)url;

- (void)pause;

- (void)stop;

- (void)seek:(double)time;

@end

NS_ASSUME_NONNULL_END
