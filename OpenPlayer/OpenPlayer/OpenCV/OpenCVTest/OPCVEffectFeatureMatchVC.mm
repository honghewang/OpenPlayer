#import "OPCVEffectFeatureMatchVC.h"

#pragma push_macro("YES")
#pragma push_macro("NO")
#undef YES
#undef NO

#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#endif

#pragma pop_macro("YES")
#pragma pop_macro("NO")

@implementation OPCVEffectFeatureMatchVC


//- (NSString *)imgPath {
//    return [[NSBundle mainBundle] pathForResource:@"query" ofType:@"jpg"];
//}


- (NSString *)scenePath {
    return [[NSBundle mainBundle] pathForResource:@"scene" ofType:@"jpg"];
}

- (NSString *)queryPath {
    return [[NSBundle mainBundle] pathForResource:@"query" ofType:@"jpg"];
}

- (cv::Mat)effect:(cv::Mat&)mat {
    // 加载场景图像
    cv::Mat queryMat = cv::imread([[self queryPath] UTF8String], 0);
    
     cv::Mat sceneMat = cv::imread([[self scenePath] UTF8String], 0);
     if (sceneMat.empty()) return queryMat.clone();
     
     // 初始化SIFT检测器
     auto detector = cv::SIFT::create();
     
     // 检测关键点和计算描述子
     std::vector<cv::KeyPoint> queryKpts, sceneKpts;
     cv::Mat queryDesc, sceneDesc;
     detector->detectAndCompute(queryMat, cv::noArray(), queryKpts, queryDesc);
     detector->detectAndCompute(sceneMat, cv::noArray(), sceneKpts, sceneDesc);
     
     // 特征匹配
     auto matcher = cv::BFMatcher::create(cv::NORM_L2);
     std::vector<cv::DMatch> matches;
     matcher->match(queryDesc, sceneDesc, matches);
     
     // 筛选最佳匹配
     std::sort(matches.begin(), matches.end());
     const int keepMatches = matches.size() * 0.05;
     std::vector<cv::DMatch> goodMatches(matches.begin(), matches.begin() + keepMatches);
     
     // 可视化匹配结果
     cv::Mat resultMat;
    
     cv::drawMatches(queryMat, queryKpts,
                    sceneMat, sceneKpts,
                    goodMatches, resultMat,
                    cv::Scalar::all(-1), cv::Scalar::all(-1),
                    std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
     
     return resultMat;
}


@end
