//
//  OPFilterInputMat.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/9/15.
//

#ifndef OPFilterInputMat_hpp
#define OPFilterInputMat_hpp

#include <stdio.h>

enum OPFilterInputMatType {
    OPFilterInputMatType_RGB,
    OPFilterInputMatType_YUV,
};

class OPFilterInputMat {
public:
    OPFilterInputMatType type;
    
};

#endif /* OPFilterInputMat_hpp */
