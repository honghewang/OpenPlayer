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
#include "OPFilterInputMat.hpp"

class OPFilterImageInputFilter : public OPFilterInOut {
    std::shared_ptr<OPFilterFrameBufferBox> renderYUV();
    std::shared_ptr<OPFilterFrameBufferBox> renderRGB();
protected:
    std::shared_ptr<OPFilterFrameBufferBox> render() override;
public:
    std::shared_ptr<OPFilterInputMat> inputMat;
};

#endif /* OPFilterImageInputFilter_hpp */
