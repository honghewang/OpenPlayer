//
//  OPFilterRenderContext.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/21.
//

#ifndef OPFilterRenderContext_hpp
#define OPFilterRenderContext_hpp

#include <stdio.h>
#include "OPFilterProgramManager.hpp"
#include "OPFilterAICenter.hpp"
#include "OPFilterFrameBufferCache.hpp"
#include "OPFilterInOut.hpp"
#include "OPFilterShowingFilter.hpp"
#include "OPFilterImageInputFilter.hpp"

class OPFilterRenderFilterLink {
public:
    std::shared_ptr<OPFilterInOut> input;
    std::shared_ptr<OPFilterInOut> output;
};

class OPFilterRenderContext {
private:
    std::function<void()> contextFunc;
    std::shared_ptr<OPFilterImageInputFilter> inputFilter = std::make_shared<OPFilterImageInputFilter>();
    std::shared_ptr<OPFilterShowingFilter> showingFilter = std::make_shared<OPFilterShowingFilter>();
    
    std::shared_ptr<OPFilterRenderFilterLink> filterLink;
    
public:
    OPFilterRenderContext() {
        inputFilter->addTarget(showingFilter);
    }
    ~OPFilterRenderContext() {}
    std::shared_ptr<OPFilterRenderFilterInfoCenter> infoCenter = std::make_shared<OPFilterRenderFilterInfoCenter>();        
    void setFilterLinks(std::shared_ptr<OPFilterRenderFilterLink> link);
    
    void setContextFunc(std::function<void()> contextFunc);
    void setRefreshFunc(std::function<void(int texture)> refreshFunc);
    
    void render(std::shared_ptr<OPFilterAIModel> faceInfo);
};

#endif /* OPFilterRenderContext_hpp */
