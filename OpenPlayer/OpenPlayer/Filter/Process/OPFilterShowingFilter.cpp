//
//  OPFilterShowingFilter.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/25.
//

#include "OPFilterShowingFilter.hpp"


void OPFilterShowingFilter::render() {
    if (refreshFunc == nullptr || inputInfos.size() == 0 || inputInfos.front()->textureId == 0) {
        return;
    }
    refreshFunc(inputInfos.front()->textureId);
}
