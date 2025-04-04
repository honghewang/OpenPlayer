//
//  OPCVEffectFilterVC.m
//  OpenPlayer
//
#import "OPCVEffectFilterVC.h"
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
#pragma pop_macro("YES")
#pragma pop_macro("NO")

@implementation OPCVEffectFilterVC

- (cv::Mat)effect:(cv::Mat&)mat {
    cv::Mat blurred;
//    // 应用高斯滤波
//    cv::GaussianBlur(mat, blurred, cv::Size(5,5), 0);
//    cv::boxFilter(mat, blurred, -1, cv::Size(5,5));
    cv::medianBlur(mat, blurred, 3);
    return blurred;
}

@end
