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
    
    // 创建零矩阵并合并三通道
    cv::Mat zeroMat = cv::Mat::zeros(mat.size(), CV_8UC1);
    std::vector<cv::Mat> mergedChannels{zeroMat, zeroMat, channels[2]};
    cv::Mat des;
    cv::merge(mergedChannels, des);
    
    return des;
}


@end
