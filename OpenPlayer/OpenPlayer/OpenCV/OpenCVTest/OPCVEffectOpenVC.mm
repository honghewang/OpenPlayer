#import "OPCVEffectOpenVC.h"
#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
#pragma pop_macro("YES")
#pragma pop_macro("NO")

@implementation OPCVEffectOpenVC

- (NSString *)imgPath {
    return [[NSBundle mainBundle] pathForResource:@"burr" ofType:@"jpg"];
}

- (cv::Mat)effect:(cv::Mat&)mat {
    cv::Mat opened;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5));
    cv::morphologyEx(mat, opened, cv::MORPH_OPEN, kernel);
    return opened;
}

@end
