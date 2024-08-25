//
//  OPPlayer.hpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/9.
//

#ifndef OPPlayer_hpp
#define OPPlayer_hpp

#include <stdio.h>
#include <string>
#include <functional>
#include "OPPlayerContext.hpp"
#include "OPStreamOperation.hpp"
#include "OPAudioController.hpp"
#include "OPVideoController.hpp"

class OPPlayer {
    
public:
    // 构造函数
    OPPlayer() {
        context = std::make_shared<OPPlayerContext>();
        std::weak_ptr<OPPlayerContext> weakContext(context);
        streamOperation = std::make_shared<OPStreamOperation>(weakContext);
        audioController = std::make_shared<OPAudioController>(weakContext);
        videoController = std::make_shared<OPVideoController>(weakContext);
        OPPtrManager::registerObj(this);
    };
    ~OPPlayer() {
        OPPtrManager::unregisterObj(this);
        // 保证前释放operation
        context->state = OPPlayerStatePlayClosed;
        streamOperation.reset();
        audioController->stop();
        videoController->stop();        
    }
    // 准备播放
    void prepareToPlay(std::string url);
    bool prepare(std::string filename);
    // 暂停播放
    void pause();
    // 停止播放
    void stop();
    // seek
    void seek(double time);
    // 设置刷新
    void setRefreshFunc(std::function<void(int width, int height, char *yData, char *uData, char *vData)> func);
    void setUpdateTimeFunc(std::function<void(double cur, double totalTime)> func);
    
    std::shared_ptr<OPStreamOperation> streamOperation;
    std::shared_ptr<OPAudioController> audioController;
    std::shared_ptr<OPVideoController> videoController;
    
    std::shared_ptr<OPPlayerContext> context;
    
private:
    
    std::thread read_thread;
    
};

#endif /* OPPlayer_hpp */
