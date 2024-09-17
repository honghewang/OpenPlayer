//
//  OPFilterRenderContext.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/21.
//

#include "OPFilterRenderContext.hpp"


void OPFilterRenderContext::render(std::shared_ptr<OPFilterAIModel> faceInfo) {
    
    if (contextFunc == nullptr) {
        return;
    }
    contextFunc();
    infoCenter->aiInfo = faceInfo->aiInfo;
    infoCenter->width = faceInfo->inputMat->imageMat.cols;
    infoCenter->height = faceInfo->inputMat->imageMat.rows;
    inputFilter->setInfoCenter(infoCenter);
    inputFilter->inputMat = faceInfo->inputMat;
    inputFilter->process();
}

void OPFilterRenderContext::setFilterLinks(std::shared_ptr<OPFilterRenderFilterLink> link) {
    inputFilter->clearTargets();
    showingFilter->clearInputs();
    if (link.get() == nullptr) {
        inputFilter->addTarget(showingFilter);
    } else {
        inputFilter->addTarget(link->input);
        link->output->addTarget(showingFilter);
    }
    filterLink = link;
}

void OPFilterRenderContext::setContextFunc(std::function<void()> contextFunc) {
    
    this->contextFunc = contextFunc;
}

void OPFilterRenderContext::setRefreshFunc(std::function<void(int texture)> refreshFunc) {
    showingFilter->refreshFunc = refreshFunc;
}
