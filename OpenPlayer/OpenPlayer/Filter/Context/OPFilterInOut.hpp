//
//  OPFilterInOut.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/15.
//

#ifndef OPFilterInOut_hpp
#define OPFilterInOut_hpp

#include <stdio.h>
#include "OPFilterProgramManager.hpp"
#include "OPFilterFrameBufferCache.hpp"
#include "OPFilterAIInfo.hpp"

class OPFilterInOut;

class OPFilterRenderFilterInfoCenter {
public:
    std::shared_ptr<OPFilterFrameBufferCache> bufferCache = std::make_shared<OPFilterFrameBufferCache>();
    std::shared_ptr<OPFilterProgramManager> progamManager = std::make_shared<OPFilterProgramManager>();
    std::shared_ptr<OPFilterAIInfo> aiInfo;
    int width;
    int height;

};

class OPFilterInputTexture {
public:
    std::weak_ptr<OPFilterInOut> InputFilter;
    int textureId = 0;
};

class OPFilterInOut : public std::enable_shared_from_this<OPFilterInOut> {
    
private:
    void updateInputTextureIds();
    void clearInputTextureIds();
protected:
    std::vector<std::shared_ptr<OPFilterInOut>> targets;
    std::vector<std::shared_ptr<OPFilterInputTexture>> inputInfos;
    std::weak_ptr<OPFilterRenderFilterInfoCenter> infoCenter;
    
    int outputTexture;
    
    virtual void render();
    
public:
    OPFilterInOut() {}
    ~OPFilterInOut() {}
    
    int needInputs = 1;
    
    void addTarget(std::shared_ptr<OPFilterInOut> output);
    void clearTargets();
    void clearInputs();
    
    void setInfoCenter(std::weak_ptr<OPFilterRenderFilterInfoCenter> center);
    void process();
};

#endif /* OPFilterInOut_hpp */
