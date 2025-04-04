//
//  OPCVEffectThresholdVC.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2025/4/4.
//

#import "OPCVEffectThresholdVC.h"
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
#pragma pop_macro("YES")
#pragma pop_macro("NO")

@interface OPCVEffectThresholdVC ()

@end

@implementation OPCVEffectThresholdVC

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (cv::Mat)effect:(cv::Mat&)mat {
    cv::Mat gray, dst;
    // 转换为灰度图像
    cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    
    // 应用Otsu二值化
    cv::threshold(gray, dst, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    return dst;
}


@end
