//
//  OPFilterProcess.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/18.
//

#ifndef OPFilterProcess_hpp
#define OPFilterProcess_hpp

#include <stdio.h>
#include <list>
#include <thread>
#include <string>
#include <condition_variable>
#include "OPFilterAICenter.hpp"
#include "OPFilterRenderContext.hpp"
#include <opencv2/core.hpp>
#include <memory>

class OPFilterProcess : public std::enable_shared_from_this<OPFilterProcess> {
public:
    bool asyncRender;
    std::atomic<bool> isProcessing= false;
    
    std::shared_ptr<OPFilterAICenter> AICenter;
    std::shared_ptr<OPFilterRenderContext> renderContext = std::make_shared<OPFilterRenderContext>();
    
    OPFilterProcess(bool async = false) : asyncRender(async) {};
    ~OPFilterProcess() {};
    
    // 开始处理
    void process(cv::Mat& img);
    
    void stop();
    
    void startAIProcess();
    
    void startRenderProcess();
    
    void setImageMat(cv::Mat mat);
    
    cv::Mat getImageMat();
    
    void setFilterLinks(std::shared_ptr<OPFilterRenderFilterLink> link);
    
    // 设置刷新
    void setContextFunc(std::function<void()> contextFunc);
    void setRefreshFunc(std::function<void(int texture)> refreshFunc);
    
private:
    cv::Mat imageMat;
    std::mutex processMutex;
    
    std::mutex AIMutex;
    std::condition_variable AICondition;
    
    std::mutex renderMutex;
    std::condition_variable renderCondition;
    
    std::thread AIThread;
    std::thread renderTread;
};

#endif /* OPFilterProcess_hpp */