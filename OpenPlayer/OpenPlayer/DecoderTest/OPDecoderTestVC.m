//
//  OPDecoderTestVC.m
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/5/7.
//

#import "OPDecoderTestVC.h"
#import "OPDecoderView.h"
#import <Masonry/Masonry.h>

@interface OPDecoderTestVC ()

@property (nonatomic, strong) OPDecoderView *playerView;

@end

@implementation OPDecoderTestVC

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setupUI];
}

- (void)setupUI {
    self.view.backgroundColor = [UIColor whiteColor];
    CGFloat screenWidth = MIN([UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
    CGRect playerFrame = CGRectMake(0, 0, screenWidth, screenWidth * 16 / 9);
    self.playerView = [[OPDecoderView alloc] initWithFrame:playerFrame];
    [self.view addSubview:self.playerView];
    [self.playerView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.center.mas_equalTo(CGPointZero);
        make.width.mas_equalTo(screenWidth);
        make.height.mas_equalTo(screenWidth * 16 / 9);
    }];
    
    NSString *path = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"mp4"];
    [self.playerView playVideoPath:path];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.playerView stop];
}



@end
