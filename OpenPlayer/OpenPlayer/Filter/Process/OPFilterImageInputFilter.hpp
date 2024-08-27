//
//  OPFilterImageInputFilter.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/25.
//

#ifndef OPFilterImageInputFilter_hpp
#define OPFilterImageInputFilter_hpp

#include <stdio.h>
#include "OPFilterInOut.hpp"

class OPFilterImageInputFilter : public OPFilterInOut {
protected:
    std::shared_ptr<OPFilterFrameBufferBox> render() override;
public:
    cv::Mat imageMat;
};

#endif /* OPFilterImageInputFilter_hpp */
