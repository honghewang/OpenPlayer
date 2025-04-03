//
//  OPCVEffectROIVC.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2025/4/3.
//

#import "OPCVEffectROIVC.h"
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

@interface OPCVEffectROIVC ()

@end

@implementation OPCVEffectROIVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (cv::Mat)effect:(cv::Mat&)mat {
    int cols = mat.cols;
    int rows = mat.rows;
    
    if (cols < 200 || rows < 200) {
        return mat.clone();
    }
    
    int x = (cols - 200) / 2;
    int y = (rows - 200) / 2;
    cv::Rect roi_rect(x, y, 200, 200);
    return mat(roi_rect);
}


@end
