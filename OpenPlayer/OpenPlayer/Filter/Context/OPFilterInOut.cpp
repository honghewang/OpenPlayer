//
//  OPFilterInOut.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/15.
//

#include "OPFilterInOut.hpp"


void OPFilterInOut::addTarget(std::shared_ptr<OPFilterInOut> output) {
    if (output->inputInfos.size() < output->needInputs) {
        int index = output->inputInfos.size();
        auto input = std::make_shared<OPFilterInputInfo>();
        input->InputFilter = shared_from_this();
        input->frameBufferBox.reset();
        output->inputInfos.push_back(input);
        
        std::shared_ptr<OPFilterOutputInfo> target = std::make_shared<OPFilterOutputInfo>();
        target->outputFilter = output;
        target->index = index;
        targets.push_back(target);
        
    } else {
        assert("滤镜使用错误，大于了所需输入");
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
        if (input->frameBufferBox.get() == nullptr || input->frameBufferBox->frameBuffer.get() == nullptr) {
            return;
        }
    }
    // 处理自身
    std::shared_ptr<OPFilterFrameBufferBox> frameBufferBox = render();
    clearInputTextureIds();
    if (frameBufferBox.get() != nullptr && frameBufferBox->frameBuffer.get() != nullptr) {
        for (auto target : targets) {
            target->outputFilter->setInfoCenter(infoCenter);
            if (target->index < target->outputFilter->inputInfos.size()) {
                target->outputFilter->inputInfos[target->index]->frameBufferBox = frameBufferBox;

            }
            target->outputFilter->process();
        }
    }
}


void OPFilterInOut::clearInputTextureIds() {
    for (auto input : inputInfos) {
        input->frameBufferBox.reset();
    }
}

std::shared_ptr<OPFilterFrameBufferBox> OPFilterInOut::render() {
    // 需要override
    return nullptr;
}

