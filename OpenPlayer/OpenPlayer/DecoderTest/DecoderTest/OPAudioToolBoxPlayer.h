//
//  OPAudioToolBoxPlayer.h
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/5/20.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol OPAudioToolBoxPlayerDelegate <NSObject>

/// 准备好了，可以开始播放了，回调
- (void)readlyToPlay;

/// 播放完成回调
- (void)playFinished;

/// 播放暂停回调
- (void)playPausedIfNeed;

/// 播放开始回调
- (void)playResumeIfNeed;

/// 播放错误的回调
- (void)playerCallBackFaiure:(NSString *)errorStr;


@end

@interface OPAudioToolBoxPlayer : NSObject

@property(nonatomic,weak) id<OPAudioToolBoxPlayerDelegate> delegate;
/// 当前的播放状态
@property(nonatomic,assign,getter=isPlayerPlaying) BOOL playerPlaying;

/// 是否准备好开始播放
@property(nonatomic,assign,getter=isReadyToPlay) BOOL readyToPlay;

/// 是否缓存完成
@property(nonatomic,assign,getter=isFinished) BOOL finished;

/// 初始化sdk
/// @param audioType 传入audioType
- (instancetype)initWithType:(NSString *)audioType;

/// 开始播放
- (void)startPlay;
/// 暂停播放
- (void)pausePlay;
// 恢复播放
- (void)resumePlay;
/// 停止播放
- (void)stopPlay;

///回调当次buffer的数据
/// @param data 当次buffer获取到的数据
/// @param length 数据的总长度
- (void)appendData:(NSData *)data totalDatalength:(float)length endFlag:(BOOL)endflag;

@end

NS_ASSUME_NONNULL_END
