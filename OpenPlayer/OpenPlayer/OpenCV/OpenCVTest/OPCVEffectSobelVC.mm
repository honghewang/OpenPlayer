#import "OPCVEffectSobelVC.h"
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

@implementation OPCVEffectSobelVC

- (cv::Mat)effect:(cv::Mat &)mat {
    cv::Mat gray;
    cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    
    cv::Mat grad_x, grad_y;
    cv::Sobel(gray, grad_x, CV_16S, 1, 0, 3);
    cv::Sobel(gray, grad_y, CV_16S, 0, 1, 3);
    
    cv::convertScaleAbs(grad_x, grad_x);
    cv::convertScaleAbs(grad_y, grad_y);
    
    cv::Mat combined;
    cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, combined);
    cv::cvtColor(combined, mat, cv::COLOR_GRAY2BGR);
    return mat;
}

@end
