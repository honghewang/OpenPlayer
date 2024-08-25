//
//  OPPlayerVC.m
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/5.
//

#import "OPPlayerVC.h"
#import "OPPlayerView.h"
#import <Masonry/Masonry.h>

@interface OPPlayerVC ()

@property (nonatomic, strong) OPPlayerView *playerView;

@end

@implementation OPPlayerVC

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setupUI];
}

- (void)setupUI {
    self.view.backgroundColor = [UIColor whiteColor];
    self.title = @"播放器";
    
    CGFloat screenWidth = MIN([UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
    CGRect playerFrame = CGRectMake(0, 0, screenWidth, screenWidth * 9 / 16);
    self.playerView = [[OPPlayerView alloc] initWithFrame:playerFrame];
    [self.view addSubview:self.playerView];
    [self.playerView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.mas_equalTo(160);
        make.width.mas_equalTo(screenWidth);
        make.height.mas_equalTo(screenWidth * 9 / 16);
    }];
    
    UIButton *btn = [[UIButton alloc] init];
    [btn setTitle:@"暂停/播放" forState:UIControlStateNormal];
    [btn addTarget:self action:@selector(pauseAction:) forControlEvents:UIControlEventTouchUpInside];
    btn.backgroundColor = [UIColor grayColor];
    [self.view addSubview:btn];
    
    [btn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.mas_equalTo(self.playerView.mas_bottom).offset(10);
        make.left.mas_equalTo(5);
        make.width.mas_equalTo(100);
    }];
    
    UIButton *seekbtn = [[UIButton alloc] init];
    [seekbtn setTitle:@"seek到5秒" forState:UIControlStateNormal];
    [seekbtn addTarget:self action:@selector(seekAction:) forControlEvents:UIControlEventTouchUpInside];
    seekbtn.backgroundColor = [UIColor grayColor];
    [self.view addSubview:seekbtn];
    [seekbtn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.mas_equalTo(self.playerView.mas_bottom).offset(10);
        make.right.mas_equalTo(-5);
        make.width.mas_equalTo(100);
    }];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    NSString *path = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"mp4"];
    [self.playerView prepareToPlay:path];
}


- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.playerView stop];
}


- (void)pauseAction:(UIButton *)sender {
    [self.playerView pause];
}

- (void)seekAction:(UIButton *)sender {
    [self.playerView seek:5.0];
}

@end
