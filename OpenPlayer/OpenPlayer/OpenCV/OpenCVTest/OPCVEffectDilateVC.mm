#import "OPCVEffectDilateVC.h"
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
#pragma pop_macro("YES")
#pragma pop_macro("NO")

@implementation OPCVEffectDilateVC

- (NSString *)imgPath {
    return [[NSBundle mainBundle] pathForResource:@"burr" ofType:@"jpg"];
}

- (cv::Mat)effect:(cv::Mat&)mat {
    cv::Mat dilated;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
    cv::dilate(mat, dilated, kernel);
    return dilated;
}

@end