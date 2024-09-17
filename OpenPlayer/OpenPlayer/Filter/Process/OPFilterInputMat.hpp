//
//  OPFilterInputMat.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/9/15.
//

#ifndef OPFilterInputMat_hpp
#define OPFilterInputMat_hpp

#include <stdio.h>
#include "OPCVUtils.hpp"

enum OPFilterInputMatType {
    OPFilterInputMatType_BGR,
    OPFilterInputMatType_YUV,
};

class OPFilterInputMat {
public:
    OPFilterInputMatType type;
    cv::Mat imageMat;
    cv::Mat imageMat2;
    bool isHighStandard;
    
    OPFilterInputMat(cv::Mat& mat): imageMat(mat), type(OPFilterInputMatType_BGR){}
    OPFilterInputMat(cv::Mat& matY, cv::Mat& matUV, bool high = true): imageMat(matY), imageMat2(matUV), isHighStandard(high), type(OPFilterInputMatType_YUV){}
    ~OPFilterInputMat() {}
};

#endif /* OPFilterInputMat_hpp */
