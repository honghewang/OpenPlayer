//
//  OPFilterGPUImageTest.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/10.
//

#import "OPFilterGPUImageTest.h"
#import <GPUImage/GPUImage.h>
#import <Masonry/Masonry.h>

@interface OPFilterGPUImageTest ()

@property (nonatomic, strong) GPUImagePicture *gpuImg;
@property (nonatomic, strong) GPUImageView *filterView;

@end

@implementation OPFilterGPUImageTest

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setupUI];
}

- (void)setupUI {
    self.view.backgroundColor = [UIColor whiteColor];
    self.filterView = [[GPUImageView alloc] init];
    [self.view addSubview:self.filterView];
    
    CGFloat height = MIN([UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height) * 16 / 9;
    [self.filterView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.centerY.mas_equalTo(0);
        make.height.mas_equalTo(height);
    }];
    
    NSString *path = [[NSBundle mainBundle] pathForResource:@"tomface" ofType:@"jpg"];
    UIImage *img = [UIImage imageWithContentsOfFile:path];
    self.gpuImg = [[GPUImagePicture alloc] initWithImage:img];
    
//    GPUImageOutput<GPUImageInput> *filter = [[GPUImageGrayscaleFilter alloc] init];
    GPUImageOutput<GPUImageInput> *filter = [[GPUImageHalftoneFilter alloc] init];
    [self.gpuImg addTarget:filter];
    
    [filter addTarget:self.filterView];
    
    [self.gpuImg processImage];
//    [self.filterView endProcessing];
}


@end
