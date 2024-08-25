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

struct OPFilterAIModel {
    cv::Mat imageMat;
    std::shared_ptr<OPFilterAIInfo> aiInfo;
    // 此处会直接move facepoints
    OPFilterAIModel(cv::Mat mat, std::shared_ptr<OPFilterAIInfo> ai): imageMat(mat), aiInfo(ai) {
    }
    
    ~OPFilterAIModel() {
        
    }
};

class OPFilterAICenter {
private:
    
    bool asyncRender;
    
    std::mutex mutex;
    
    std::shared_ptr<OPFilterAIModel> preAIInfo;
    std::shared_ptr<OPFilterAIModel> curAIInfo;
public:
    OPFilterAICenter(bool async) : asyncRender(async) {}
    ~OPFilterAICenter() {}
    
    void detectorImg(cv::Mat& img);
    
    std::shared_ptr<OPFilterAIModel> getAIInfo(bool isPop);
};

#endif /* OPFilterAICenter_hpp */
