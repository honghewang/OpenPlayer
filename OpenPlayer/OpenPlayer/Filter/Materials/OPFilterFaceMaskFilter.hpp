//
//  OPFilterFaceMask.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/25.
//

#ifndef OPFilterFaceMask_hpp
#define OPFilterFaceMask_hpp

#include <stdio.h>
#include "OPFilterInOut.hpp"

class OPFilterFaceMaskFilter : public OPFilterInOut {
private:
    void renderTexture(std::shared_ptr<OPFilterRenderFilterInfoCenter> info);
    void renderFaceMask(std::shared_ptr<OPFilterRenderFilterInfoCenter> info);
protected:
    std::shared_ptr<OPFilterFrameBufferBox> render() override;
    
};

#endif /* OPFilterFaceMask_hpp */
