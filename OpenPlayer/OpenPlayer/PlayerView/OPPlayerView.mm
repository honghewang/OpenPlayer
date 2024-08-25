//
//  OPPlayerView.m
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/6.
//

#import "OPPlayerView.h"
#include "OPPlayer.hpp"
#include "OPPtrManager.hpp"
#import "OPPlayerLayer.h"
#import <OpenGLES/ES2/gl.h>
#import <Masonry/Masonry.h>


static void refreshVideo(void *opaque, int width, int height, char *yData, char *uData, char *vData);
static void updateTime(void *opaque, double cur, double totalTime);

@interface OPPlayerView () {
    std::shared_ptr<OPPlayer> player;
}

@property (nonatomic, strong) OPPlayerLayer *playerLayer;

@property (nonatomic, strong) UILabel *timeLbl;

@end

@implementation OPPlayerView

#pragma mark - Life Cycle

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor blackColor];
        [self setupUI];
    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    if (!CGRectEqualToRect(self.playerLayer.frame, self.bounds)) {
        self.playerLayer.frame = self.bounds;
    }
}


- (void)setupUI {
    OPPlayerLayer *playerLayer = [[OPPlayerLayer alloc] init];
    [self.layer addSublayer:playerLayer];
    playerLayer.frame = self.bounds;
    self.playerLayer = playerLayer;
    
    UILabel *timeLbl = [[UILabel alloc] init];
    timeLbl.backgroundColor = [UIColor redColor];
    timeLbl.font = [UIFont systemFontOfSize:11];
    timeLbl.textColor = [UIColor whiteColor];
    timeLbl.textAlignment = NSTextAlignmentRight;
    [self addSubview:timeLbl];
    [timeLbl mas_makeConstraints:^(MASConstraintMaker *make) {
        make.bottom.right.mas_equalTo(0);
        make.height.mas_equalTo(15);
        make.width.mas_equalTo(50);
    }];
    self.timeLbl = timeLbl;
}


#pragma mark - Public Methods

- (void)prepareToPlay:(NSString *)url {
    player = std::make_shared<OPPlayer>();
    void *opaque = (__bridge void *)self;
    OPPtrManager::registerObj(opaque);
    
    const char *filename = [url cStringUsingEncoding:NSUTF8StringEncoding];
    player->prepareToPlay(std::string(filename));
    player->setRefreshFunc([=] (int width, int height, char *yData, char *uData, char *vData) {
        refreshVideo(opaque, width, height, yData, uData, vData);
    });
    player->setUpdateTimeFunc([=] (double cur, double totalTime) {
        updateTime(opaque, cur, totalTime);
    });
}

- (void)pause {
    if (player.get()) {
        player->pause();
    }
}


- (void)stop {
    player.reset();
}

- (void)seek:(double)time {
    if (player.get()) {
        player->seek(time);
    }
}

- (void)updateTime:(double)cur totalTime:(double)totalTime {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.timeLbl.text = [NSString stringWithFormat:@"%.1f/%.1f", cur, totalTime];
    });
}


@end

void refreshVideo(void *opaque, int width, int height, char *yData, char *uData, char *vData) {
    @autoreleasepool {
        OPPlayerView *playerView = (__bridge OPPlayerView *)opaque;
        if (playerView) {
            [playerView.playerLayer refreshWithWidth:width height:height yData:yData uData:uData vData:vData];
        }
    }
}

void updateTime(void *opaque, double cur, double totalTime) {
    @autoreleasepool {
        OPPlayerView *playerView = (__bridge OPPlayerView *)opaque;
        if (playerView) {
            [playerView updateTime:cur totalTime:totalTime];
        }
    }
}

