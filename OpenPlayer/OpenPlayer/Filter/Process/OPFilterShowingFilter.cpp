//
//  OPFilterShowingFilter.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/25.
//

#include "OPFilterShowingFilter.hpp"


std::shared_ptr<OPFilterFrameBufferBox> OPFilterShowingFilter::render() {
    if (refreshFunc == nullptr || inputInfos.size() == 0 || inputInfos.front()->frameBufferBox.get() == nullptr) {
        return nullptr;
    }
    refreshFunc(inputInfos.front()->frameBufferBox->frameBuffer->texture);
    return nullptr;
}
