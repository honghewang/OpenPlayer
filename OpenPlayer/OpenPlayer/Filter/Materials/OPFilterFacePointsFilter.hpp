//
//  OPFilterFacePointsFilter.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/25.
//

#ifndef OPFilterFacePointsFilter_hpp
#define OPFilterFacePointsFilter_hpp

#include <stdio.h>
#include "OPFilterTextureFilter.hpp"

class OPFilterFacePointsFilter : public OPFilterTextureFilter {
private:
    void renderFacePoints(std::shared_ptr<OPFilterRenderFilterInfoCenter> info);
protected:
    std::shared_ptr<OPFilterFrameBufferBox> render() override;
   
};

#endif /* OPFilterFacePointsFilter_hpp */
