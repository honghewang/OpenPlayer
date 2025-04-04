//
//  OPCVEffectErodeVC.m
//  OpenPlayer
//

#import "OPCVEffectErodeVC.h"
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
#pragma pop_macro("YES")
#pragma pop_macro("NO")

@implementation OPCVEffectErodeVC

- (NSString *)imgPath {
    NSString *imgPath = [[NSBundle mainBundle] pathForResource:@"burr" ofType:@"jpg"];
    return imgPath;
}

- (cv::Mat)effect:(cv::Mat&)mat {
//    cv::Mat not_mat;
//    cv::bitwise_not(mat, not_mat);
    cv::Mat eroded;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3)); // Create 3x3 rectangular kernel (MORPH_RECT) for erosion operation
    cv::erode(mat, eroded, kernel);
    return eroded;
}

@end
