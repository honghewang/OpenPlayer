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
void OPFilterProcess::process(cv::Mat& img) {
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
    setImageMat(img);
    isProcessing = true;
    AICondition.notify_one();
}

void OPFilterProcess::startAIProcess() {
    cv::Mat tmpMat = getImageMat();
    if (tmpMat.rows != 0) {
        if (asyncRender) {
            std::shared_ptr<OPFilterAIModel> aiModel = AICenter->getAIInfo(false);
            if (aiModel.get() != nullptr && aiModel->imageMat.rows > 0) {
                renderCondition.notify_one();
            }
        }
        AICenter->detectorImg(tmpMat);
        if (!asyncRender) {
            renderCondition.notify_one();
        }
        setImageMat(cv::Mat(0, 0, CV_32F));
    }
    std::unique_lock<std::mutex> locker(AIMutex);
    AICondition.wait_for(locker, std::chrono::milliseconds(40));
}

void OPFilterProcess::stop() {
    isProcessing = false;
}

void OPFilterProcess::startRenderProcess() {
    std::unique_lock<std::mutex> locker(renderMutex);
    renderCondition.wait_for(locker, std::chrono::milliseconds(40));
    std::shared_ptr<OPFilterAIModel> aiModel = AICenter->getAIInfo(true);
    if (aiModel.get() != nullptr && aiModel->imageMat.rows > 0) {
        renderContext->render(aiModel);
    }
    
}

void OPFilterProcess::setImageMat(cv::Mat mat) {
    std::lock_guard<std::mutex> locker(processMutex);
    imageMat = mat;

}

cv::Mat OPFilterProcess::getImageMat() {
    std::lock_guard<std::mutex> locker(processMutex);
    return imageMat;
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
