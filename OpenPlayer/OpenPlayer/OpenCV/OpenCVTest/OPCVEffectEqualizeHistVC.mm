#import "OPCVEffectEqualizeHistVC.h"
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
#pragma pop_macro("YES")
#pragma pop_macro("NO")

@implementation OPCVEffectEqualizeHistVC

- (NSString *)imgPath {
    return [[NSBundle mainBundle] pathForResource:@"hist" ofType:@"jpeg"];
}

- (cv::Mat)effect:(cv::Mat&)mat {
    cv::Mat gray, equalized;
    cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(gray, equalized);
    
    // 计算原始和均衡化直方图
    cv::Mat origHist, eqHist;
    int histSize = 256;
    float range[] = {0,256};
    const float* histRange = {range};
    cv::calcHist(&gray, 1, 0, cv::Mat(), origHist, 1, &histSize, &histRange);
    cv::calcHist(&equalized, 1, 0, cv::Mat(), eqHist, 1, &histSize, &histRange);
    
    // 创建直方图可视化
    int hist_w = 512, hist_h = 400;
    cv::Mat histImage1(hist_h, hist_w, CV_8UC3, cv::Scalar(0));
    cv::Mat histImage2(hist_h, hist_w, CV_8UC3, cv::Scalar(0));
    
    cv::normalize(origHist, origHist, 0, histImage1.rows, cv::NORM_MINMAX);
    cv::normalize(eqHist, eqHist, 0, histImage2.rows, cv::NORM_MINMAX);
    
    int binWidth = cvRound((double)hist_w/histSize);
    for(int i = 1; i < histSize; i++) {
        cv::line(histImage1,
            cv::Point(binWidth*(i-1), hist_h - cvRound(origHist.at<float>(i-1))),
            cv::Point(binWidth*(i), hist_h - cvRound(origHist.at<float>(i))),
            cv::Scalar(0, 0, 255), 2);
        cv::line(histImage2,
            cv::Point(binWidth*(i-1), hist_h - cvRound(eqHist.at<float>(i-1))),
            cv::Point(binWidth*(i), hist_h - cvRound(eqHist.at<float>(i))),
            cv::Scalar(0, 0, 255), 2);
    }
    
    // 合并显示原图、均衡图及直方图
    cv::Mat grayBGR, equalizedBGR;
    cv::cvtColor(gray, grayBGR, cv::COLOR_GRAY2BGR);
    cv::cvtColor(equalized, equalizedBGR, cv::COLOR_GRAY2BGR);
    
    cv::resize(grayBGR, grayBGR, histImage1.size());
    cv::resize(equalizedBGR, equalizedBGR, histImage2.size());
    
    cv::Mat top, bottom;
    cv::hconcat(grayBGR, histImage1, top);
    cv::hconcat(equalizedBGR, histImage2, bottom);
    cv::vconcat(top, bottom, grayBGR);
    
    return grayBGR;
}

@end
