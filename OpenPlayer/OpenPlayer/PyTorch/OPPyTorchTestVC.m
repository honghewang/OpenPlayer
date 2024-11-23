//
//  OPPyTorchTestVC.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/11/18.
//

#import "OPPyTorchTestVC.h"
#import "OPSegTorchModule.h"
#import <Masonry/Masonry.h>

@interface OPPyTorchTestVC ()

@property (nonatomic, strong) UIImageView *imgView;

@end

@implementation OPPyTorchTestVC

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setupUI];
    [self segTorchTest];
//    [self animteTorchTest];
}

- (void)setupUI {
    self.view.backgroundColor = [UIColor whiteColor];
    self.imgView = [[UIImageView alloc] init];
    self.imgView.contentMode = UIViewContentModeScaleAspectFit;
    [self.view addSubview:self.imgView];
    [self.imgView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.top.bottom.mas_equalTo(0);
    }];
}

- (void)segTorchTest {
    NSString *path = [[NSBundle mainBundle] pathForResource:@"segment.pt" ofType:@""];
    OPSegTorchModule *torchModule = [[OPSegTorchModule alloc] initWithFileAtPath:path];
    NSString *imgPath = [[NSBundle mainBundle] pathForResource:@"deeplab" ofType:@"jpg"];
    UIImage *img = [torchModule segmentImage:[UIImage imageWithContentsOfFile:imgPath]];
    self.imgView.image = img;
}


- (void)animteTorchTest {
    NSString *path = [[NSBundle mainBundle] pathForResource:@"model.pt" ofType:@""];
    OPSegTorchModule *torchModule = [[OPSegTorchModule alloc] initWithFileAtPath:path];
    NSString *imgPath = [[NSBundle mainBundle] pathForResource:@"dog" ofType:@"jpg"];
    UIImage *img = [torchModule segmentImage:[UIImage imageWithContentsOfFile:imgPath]];
    self.imgView.image = img;
}



@end
