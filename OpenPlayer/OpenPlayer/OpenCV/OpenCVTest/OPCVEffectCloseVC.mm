#import "OPCVEffectCloseVC.h"
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
#pragma pop_macro("YES")
#pragma pop_macro("NO")


@implementation OPCVEffectCloseVC

- (NSString *)imgPath {
    return [[NSBundle mainBundle] pathForResource:@"burr" ofType:@"jpg"];
}

- (cv::Mat)effect:(cv::Mat&)mat {
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5));
    cv::morphologyEx(mat, mat, cv::MORPH_CLOSE, kernel);
    return mat;
}


@end
