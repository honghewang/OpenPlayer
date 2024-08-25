//
//  OPFilterFacePointsFilter.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/25.
//

#ifndef OPFilterFacePointsFilter_hpp
#define OPFilterFacePointsFilter_hpp

#include <stdio.h>
#include "OPFilterInOut.hpp"

class OPFilterFacePointsFilter : public OPFilterInOut {
private:
    void renderTexture(std::shared_ptr<OPFilterRenderFilterInfoCenter> info);
    void renderFacePoints(std::shared_ptr<OPFilterRenderFilterInfoCenter> info);
    std::shared_ptr<OPFilterFrameBufferBox> frameBufferBox;
protected:
   void render() override;
   
};

#endif /* OPFilterFacePointsFilter_hpp */
