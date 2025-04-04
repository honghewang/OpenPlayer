//
//  OPCVEffectChannelVC.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2025/4/3.
//

#import "OPCVEffectChannelVC.h"
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
#pragma pop_macro("YES")
#pragma pop_macro("NO")

@implementation OPCVEffectChannelVC

- (cv::Mat)effect:(cv::Mat&)mat {
    std::vector<cv::Mat> channels;
    cv::split(mat, channels);    
    // 返回红色通道（BGR格式的第三个通道）
    return channels[2];
}


@end
