//
//  OPFilterProcess.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/18.
//

#include "OPFilterProcess.hpp"
#include <memory>
#include <chrono>
#include "OPFilterFrameBuffer.hpp"

void render_thread_op(std::weak_ptr<OPFilterProcess> weakProcess);

void ai_thread_op(std::weak_ptr<OPFilterProcess> weakProcess);


// 开始处理
void OPFilterProcess::process(std::shared_ptr<OPFilterInputMat> input) {
    if (AICenter == nullptr) {
        AICenter = std::make_shared<OPFilterAICenter>(asyncRender);
        
        std::weak_ptr<OPFilterProcess> weakProcess = weak_from_this();
        if (!AIThread.joinable()) {
            AIThread = std::thread(ai_thread_op, weakProcess);
            AIThread.detach();
        }
        if (!renderTread.joinable()) {
            renderTread = std::thread(render_thread_op, weakProcess);
            renderTread.detach();
        }
    }
    setMatInput(input);
    isProcessing = true;
    AICondition.notify_one();
}


void OPFilterProcess::startAIProcess() {
    std::shared_ptr<OPFilterInputMat> inputMat = getMatInput();
    if (inputMat.get() != nullptr) {
        if (inputMat->imageMat.rows != 0) {
            if (asyncRender) {
                std::shared_ptr<OPFilterAIModel> aiModel = AICenter->getAIInfo(false);
                if (aiModel.get() != nullptr && aiModel->inputMat.get() != nullptr && aiModel->inputMat->imageMat.rows > 0) {
                    printf("startAIProcess ++++++++++++++++2 \n");
                    renderCondition.notify_one();
                    
                }
            }
            printf("startAIProcess ++++++++++++++++1 \n");
            AICenter->detectorImg(inputMat);
            if (!asyncRender) {
                renderCondition.notify_one();
            }
            clearMatInput();
        }
    }
    std::unique_lock<std::mutex> locker(AIMutex);
    AICondition.wait(locker);
}

void OPFilterProcess::stop() {
    isProcessing = false;
    AICondition.notify_one();
    renderCondition.notify_one();
}

void OPFilterProcess::startRenderProcess() {
    std::unique_lock<std::mutex> locker(renderMutex);
    renderCondition.wait(locker);
    std::shared_ptr<OPFilterAIModel> aiModel = AICenter->getAIInfo(true);
    if (aiModel.get() != nullptr && aiModel->inputMat.get() != nullptr && aiModel->inputMat->imageMat.rows > 0) {
        renderContext->render(aiModel);
    }
}

void OPFilterProcess::setMatInput(std::shared_ptr<OPFilterInputMat> input) {
    std::lock_guard<std::mutex> locker(processMutex);
    this->inputMat = input;
}

void OPFilterProcess::clearMatInput() {
    std::lock_guard<std::mutex> locker(processMutex);
    inputMat.reset();
}

std::shared_ptr<OPFilterInputMat> OPFilterProcess::getMatInput() {
    std::lock_guard<std::mutex> locker(processMutex);
    return inputMat;
}

void OPFilterProcess::setFilterLinks(std::shared_ptr<OPFilterRenderFilterLink> link) {
    renderContext->setFilterLinks(link);
}

void OPFilterProcess::setContextFunc(std::function<void()> contextFunc) {
    renderContext->setContextFunc(contextFunc);
}

void OPFilterProcess::setRefreshFunc(std::function<void(int texture)> refreshFunc) {
    renderContext->setRefreshFunc(refreshFunc);
}


void render_thread_op(std::weak_ptr<OPFilterProcess> weakProcess) {
    while (true) {
        if (weakProcess.expired()) {
            return;
        }
        auto process = weakProcess.lock();
        if (process.get() == nullptr || !process->isProcessing) {
            return;
        }
        process->startRenderProcess();
    }
}

void ai_thread_op(std::weak_ptr<OPFilterProcess> weakProcess) {
    while (true) {
        if (weakProcess.expired()) {
            return;
        }
        // 启动
        auto process = weakProcess.lock();
        if (process.get() == nullptr || !process->isProcessing) {
            return;
        }
        process->startAIProcess();
    }
}
