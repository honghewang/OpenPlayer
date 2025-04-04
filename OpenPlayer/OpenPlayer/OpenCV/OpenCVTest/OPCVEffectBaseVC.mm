//
//  OPCVEffectBaseVC.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2025/4/3.
//

#import "OPCVEffectBaseVC.h"
#import <Masonry/Masonry.h>

// 在包含OpenCV头文件前：
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
// 包含后恢复宏（如需在后续代码中使用Objective-C的YES/NO）
#pragma pop_macro("YES")
#pragma pop_macro("NO")


@interface OPCVEffectBaseVC ()


@end

@implementation OPCVEffectBaseVC

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor whiteColor];
    _imageView = [[UIImageView alloc] init];
    _imageView.contentMode = UIViewContentModeScaleAspectFit;
    [self.view addSubview:_imageView];
    [_imageView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.view);
    }];
    __weak OPCVEffectBaseVC *weakSelf = self;
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        cv::Mat mat = cv::imread([[weakSelf imgPath] UTF8String]);
        [weakSelf displayCVMat:[weakSelf effect:mat]];
    });
}

- (NSString *)imgPath {
    NSString *imgPath = [[NSBundle mainBundle] pathForResource:@"dog" ofType:@"jpg"];
    return imgPath;
}

- (cv::Mat)effect:(cv::Mat&)mat {
    return mat;
}

- (void)displayCVMat:(cv::Mat)mat {
    // 转换颜色空间从BGR到RGB
    cv::Mat rgbMat;
    if (mat.channels() == 1) {
        cvtColor(mat, rgbMat, cv::COLOR_GRAY2RGB);
    } else {
        cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
    }
    
    // 创建UIImage对象
    UIImage *image = [self matToUIImage:rgbMat];
    
    // 主线程更新UI
    __weak OPCVEffectBaseVC *weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        weakSelf.imageView.image = image;
    });
}

- (UIImage *)matToUIImage:(const cv::Mat&)mat {
    NSData *data = [NSData dataWithBytes:mat.data length:mat.elemSize() * mat.total()];
    CGColorSpaceRef colorSpace = mat.channels() == 1 ? CGColorSpaceCreateDeviceGray() : CGColorSpaceCreateDeviceRGB();
    
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)data);
    CGImageRef imageRef = CGImageCreate(mat.cols, mat.rows, 8, 8 * mat.channels(), mat.step[0], colorSpace, kCGImageAlphaNone|kCGBitmapByteOrderDefault, provider, NULL, false, kCGRenderingIntentDefault);
    
    UIImage *image = [UIImage imageWithCGImage:imageRef];
    
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
    
    return image;
}

@end
