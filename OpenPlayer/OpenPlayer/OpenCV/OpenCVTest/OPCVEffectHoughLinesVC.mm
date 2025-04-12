//
//  OPCVEffectHoughLinesVC.mm
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/12.
//

#import "OPCVEffectHoughLinesVC.h"
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
#pragma pop_macro("YES")
#pragma pop_macro("NO")

@implementation OPCVEffectHoughLinesVC


- (NSString *)imgPath {
    return [[NSBundle mainBundle] pathForResource:@"longzi" ofType:@"jpg"];
}

- (cv::Mat)effect:(cv::Mat &)mat {
    cv::Mat result;
    cv::Mat gray, blurred, edges;
    cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    // 高斯模糊降噪
    cv::GaussianBlur(gray, blurred, cv::Size(5,5), 0);
    // Canny边缘检测
    cv::Canny(blurred, edges, 80, 150);
    
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(edges, lines, 1, CV_PI/180, 80, 30, 10);
    
    mat.copyTo(result);
    for (const auto& line : lines) {
        cv::line(result, cv::Point(line[0], line[1]),
                 cv::Point(line[2], line[3]), cv::Scalar(0,0,255), 2);
    }
    
    return result;
}


@end
