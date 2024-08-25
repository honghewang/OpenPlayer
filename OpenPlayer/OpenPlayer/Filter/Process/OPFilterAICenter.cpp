//
//  OPFilterAICenter.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/18.
//

#include "OPFilterAICenter.hpp"


void OPFilterAICenter::detectorImg(cv::Mat& img) {
    std::vector<dlib::full_object_detection> facePoints = OPCVUtils::getInstance()->detectorImg(img);
    {
        std::lock_guard<std::mutex> locker(mutex);
        preAIInfo = curAIInfo;
        cv::Mat imgClone = OPCVUtils::imgRGBWithMat(img);
        std::shared_ptr<OPFilterAIInfo> aiInfo = std::make_shared<OPFilterAIInfo>();
        aiInfo->facePoints = std::move(facePoints);
        curAIInfo = std::make_shared<OPFilterAIModel>(imgClone, aiInfo);
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
