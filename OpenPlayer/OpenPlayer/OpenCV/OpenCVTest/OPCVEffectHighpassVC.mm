//
//  OPCVEffectHighpassVC.mm
//  OpenPlayer
//
#import "OPCVEffectHighpassVC.h"
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
#pragma pop_macro("YES")
#pragma pop_macro("NO")

@implementation OPCVEffectHighpassVC

- (cv::Mat)effect:(cv::Mat&)mat {
    // Convert to grayscale for frequency processing
    cv::Mat gray;
    cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);

    // 强制对齐到偶数尺寸
    int evenRows = (gray.rows + 1) & -2;
    int evenCols = (gray.cols + 1) & -2;

    cv::Mat channelPadded;
    cv::copyMakeBorder(gray, channelPadded,
                     0, evenRows - gray.rows,
                     0, evenCols - gray.cols,
                     cv::BORDER_CONSTANT, cv::Scalar::all(0));
    
    cv::Mat filter = cv::Mat::ones(channelPadded.size(), CV_32F);    

    for(int i=0; i<100; i++) {
        for(int j=0; j<100; j++) {
            filter.at<float>(i,j) = 0.0;
        }
    }

    // DCT变换处理
    cv::Mat dctMat;
    channelPadded.convertTo(dctMat, CV_32F);
    
    // 前向DCT变换
    cv::dct(dctMat, dctMat);
    
    // 应用DCT域滤波器
    cv::multiply(dctMat, filter, dctMat);
    
    // 逆向DCT变换
    cv::idct(dctMat, dctMat);
    
    // 转换为8位并裁剪
    cv::Mat result;
    dctMat.convertTo(result, CV_8U);
    result = result(cv::Rect(0, 0, gray.cols, gray.rows));
    cv::bitwise_not(result, result);
    
    return result;
}

@end
