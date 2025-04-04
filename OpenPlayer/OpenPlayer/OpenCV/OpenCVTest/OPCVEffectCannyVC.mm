//
//  OPCVEffectCannyVC.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2025/4/3.
//

#import "OPCVEffectCannyVC.h"
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

@interface OPCVEffectCannyVC ()

@end

@implementation OPCVEffectCannyVC

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (cv::Mat)effect:(cv::Mat&)mat {
    cv::Mat gray, blurred, edges;
    
    // 转换为灰度图
    if (mat.channels() == 3) {
        cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = mat;
    }
    
    // 高斯模糊降噪
    cv::GaussianBlur(gray, blurred, cv::Size(5,5), 0);
    
    // Canny边缘检测
    cv::Canny(blurred, edges, 100, 200);
    
    return edges;
}

@end
