//
//  OPCVEffectBlendVC.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2025/4/4.
//

#import "OPCVEffectBlendVC.h"
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
#pragma pop_macro("YES")
#pragma pop_macro("NO")

@interface OPCVEffectBlendVC ()

@end

@implementation OPCVEffectBlendVC

- (void)viewDidLoad {
    [super viewDidLoad];
}


- (cv::Mat)effect:(cv::Mat&)mat {
    cv::Mat blended;
    
    NSString *imgPath = [[NSBundle mainBundle] pathForResource:@"deeplab" ofType:@"jpg"];
    cv::Mat img2 = cv::imread([imgPath UTF8String]);
    
    if(!img2.empty()) {
        // 调整img2尺寸与输入图像匹配
        cv::resize(img2, img2, mat.size(), 0, 0, cv::INTER_LINEAR);
        // 使用addWeighted进行混合
        cv::addWeighted(mat, 0.6, img2, 0.4, 0.0, blended);
    } else {
        blended = mat.clone();
    }
    
    return blended;
}

@end
