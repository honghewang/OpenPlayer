//
//  OPFilterInOut.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/15.
//

#include "OPFilterInOut.hpp"


void OPFilterInOut::addTarget(std::shared_ptr<OPFilterInOut> output) {
    targets.push_back(output);
    auto input = std::make_shared<OPFilterInputTexture>();
    if (output->inputInfos.size() < output->needInputs) {
        input->InputFilter = weak_from_this();
        input->textureId = 0;
        output->inputInfos.push_back(input);
    }
}


void OPFilterInOut::clearTargets() {
    targets.clear();
}

void OPFilterInOut::clearInputs() {
    inputInfos.clear();
}

void OPFilterInOut::setInfoCenter(std::weak_ptr<OPFilterRenderFilterInfoCenter> center) {
    infoCenter = center;
}


void OPFilterInOut::process() {
    // 所有input都准备好才开始
    for (auto input : inputInfos) {
        if (input->textureId <= 0) {
            return;
        }
    }
    // 处理自身
    render();
    clearInputTextureIds();
    if (outputTexture != 0) {
        for (auto target : targets) {
            target->setInfoCenter(infoCenter);
            target->updateInputTextureIds();
            target->process();
        }
    }
    // 清空自身
    outputTexture = 0;
}

void OPFilterInOut::updateInputTextureIds() {
    for (auto input : inputInfos) {
        if (!input->InputFilter.expired() && input->InputFilter.lock()->outputTexture > 0) {
            input->textureId = input->InputFilter.lock()->outputTexture;
        }
    }
}

void OPFilterInOut::clearInputTextureIds() {
    for (auto input : inputInfos) {
        input->textureId = 0;
    }
}

void OPFilterInOut::render() {
    // 需要override
    outputTexture = 0;
}

