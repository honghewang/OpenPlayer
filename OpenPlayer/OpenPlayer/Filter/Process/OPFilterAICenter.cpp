//
//  OPFilterAICenter.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/18.
//

#include "OPFilterAICenter.hpp"


void OPFilterAICenter::detectorImg(std::shared_ptr<OPFilterInputMat> inputMat) {
    if (inputMat.get() == nullptr) {
        return;
    }
    std::vector<dlib::full_object_detection> facePoints;
    if (inputMat->type == OPFilterInputMatType_RGB) {
        facePoints = OPCVUtils::getInstance()->detectorImg(inputMat->imageMat);
    } else {
        facePoints = OPCVUtils::getInstance()->detectorGrayImg(inputMat->imageMat);
    }
    {
        std::lock_guard<std::mutex> locker(mutex);
        preAIInfo = curAIInfo;
        std::shared_ptr<OPFilterAIInfo> aiInfo = std::make_shared<OPFilterAIInfo>();
        aiInfo->facePoints = std::move(facePoints);
        curAIInfo = std::make_shared<OPFilterAIModel>(inputMat, aiInfo);
    }
}

std::shared_ptr<OPFilterAIModel> OPFilterAICenter::getAIInfo(bool isPop) {
    std::lock_guard<std::mutex> locker(mutex);
    if (asyncRender) {
        if (isPop) {
            auto tmpInfo = preAIInfo;
            preAIInfo.reset();
            return tmpInfo;
        } else {
            return preAIInfo;
        }
    }
    if (isPop) {
        auto tmpInfo = curAIInfo;
        curAIInfo.reset();
        return tmpInfo;
    } else {
        return curAIInfo;
    }
}
