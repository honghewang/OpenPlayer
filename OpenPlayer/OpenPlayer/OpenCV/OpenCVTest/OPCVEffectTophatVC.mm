//
//  OPCVEffectTophatVC.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2025/4/4.
//

#import "OPCVEffectTophatVC.h"
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
#pragma pop_macro("YES")
#pragma pop_macro("NO")

@interface OPCVEffectTophatVC ()

@end

@implementation OPCVEffectTophatVC


- (NSString *)imgPath {
    return [[NSBundle mainBundle] pathForResource:@"burr" ofType:@"jpg"];
}

- (cv::Mat)effect:(cv::Mat &)mat {
    cv::Mat result;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
    cv::morphologyEx(mat, result, cv::MORPH_BLACKHAT, kernel);
    return result;
}

@end
