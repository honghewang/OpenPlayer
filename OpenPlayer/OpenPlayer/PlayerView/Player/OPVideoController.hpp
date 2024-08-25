//
//  OPVideoController.hpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/26.
//

#ifndef OPVideoController_hpp
#define OPVideoController_hpp

#include <stdio.h>
#include "OPPlayerContext.hpp"

class AVCodecContext;
class OPPlayer;
class SwsContext;
class OPVideoController {
public:
    OPVideoController(std::weak_ptr<OPPlayerContext> wc): weakContext(wc)  {}
    ~OPVideoController();
    
    void play(AVCodecContext *avctx, OPPlayer *player);
    void pause(bool pause);
    void stop();
    
    SwsContext *vctx = nullptr;
    
    std::weak_ptr<OPPlayerContext> weakContext;
    
private:
    
    std::thread videoRefreshThread;
};


#endif /* OPVideoController_hpp */
