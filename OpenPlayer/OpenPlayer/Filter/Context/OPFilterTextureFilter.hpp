//
//  OPFilterTextureFilter.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/27.
//

#ifndef OPFilterTextureFilter_hpp
#define OPFilterTextureFilter_hpp

#include <stdio.h>
#include "OPFilterInOut.hpp"

class OPFilterTextureFilter : public OPFilterInOut {
private:
    std::string vshader;
    std::string fshader;
protected:
    void renderTexture(std::shared_ptr<OPFilterRenderFilterInfoCenter> info);
    std::shared_ptr<OPFilterFrameBufferBox> render() override;
public:
    OPFilterTextureFilter(std::string v = OPFilterVertexShaderString, std::string f = OPFilterPassthroughFragmentShaderString) : vshader(v), fshader(f) {}
    ~OPFilterTextureFilter() {}
};

#endif /* OPFilterTextureFilter_hpp */
