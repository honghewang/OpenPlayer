//
//  OPFilterAICenter.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/18.
//

#ifndef OPFilterAICenter_hpp
#define OPFilterAICenter_hpp

#include <stdio.h>
#include "OPCVUtils.hpp"
#include "OPFilterAIInfo.hpp"
#include "OPFilterInputMat.hpp"


struct OPFilterAIModel {
    std::shared_ptr<OPFilterInputMat> inputMat;
    std::shared_ptr<OPFilterAIInfo> aiInfo;
    // 此处会直接move facepoints
    OPFilterAIModel(std::shared_ptr<OPFilterInputMat> input, std::shared_ptr<OPFilterAIInfo> ai): inputMat(input), aiInfo(ai) {
    }
    
    ~OPFilterAIModel() {
        
    }
};

class OPFilterAICenter {
private:
    
    bool asyncRender;
    
    std::mutex mutex;
    
    std::shared_ptr<OPFilterAIModel> curAIInfo;
public:
    OPFilterAICenter(bool async) : asyncRender(async) {}
    ~OPFilterAICenter() {}
    
    void detectorImg(std::shared_ptr<OPFilterInputMat> inputMat);
    
    std::shared_ptr<OPFilterAIModel> getAIInfo(bool isPop);
};

#endif /* OPFilterAICenter_hpp */
