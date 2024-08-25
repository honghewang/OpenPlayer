//
//  OPFilterAIInfo.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/25.
//

#ifndef OPFilterAIInfo_hpp
#define OPFilterAIInfo_hpp

#include <stdio.h>
#include "OPCVUtils.hpp"

class OPFilterAIInfo {
public:
    /// 人脸点
    std::vector<dlib::full_object_detection> facePoints;
};

#endif /* OPFilterAIInfo_hpp */
