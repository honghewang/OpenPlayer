//
//  OPCVUtils.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/8.
//

#include "OPCVUtils.hpp"
#include "OPCVPredictor.hpp"

#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/opencv.h>
#include <iostream>
#include <chrono>
#include <OpenGLES/ES2/glext.h>
#include <OpenGLES/ES2/gl.h>

// 静态成员初始化
OPCVUtils *OPCVUtils::instance = nullptr;
std::mutex OPCVUtils::faceMtx;
std::mutex OPCVUtils::facePointMtx;
std::mutex OPCVUtils::instanceMtx;

void printCurrentTime();


OPCVUtils::OPCVUtils() {
    predictor = new OPCVPredictor();
}

OPCVUtils::~OPCVUtils() {
    delete predictor;
}

cv::Mat OPCVUtils::imgWithFile(std::string file) {
    cv::Mat src = cv::imread(file);
    return src;
}

cv::Mat OPCVUtils::imgRGBWithFile(std::string file) {
    cv::Mat src = cv::imread(file);
    cv::Mat des;
    cv::cvtColor(src, des, cv::COLOR_BGR2RGB);
    return des;
}

cv::Mat OPCVUtils::imgRGBWithMat(cv::Mat& mat) {
    cv::Mat des;
    cv::cvtColor(mat, des, cv::COLOR_BGR2RGB);
    return des;
}

cv::Mat OPCVUtils::imgRGBWithYUVMat(cv::Mat& yuv) {
    cv::Mat des;
    cv::cvtColor(yuv, des, cv::COLOR_YUV420sp2BGR);
    return des;
}

cv::Mat OPCVUtils::imgGrayWithYUVMat(cv::Mat& yuv) {
    cv::Mat des;
    cv::cvtColor(yuv, des, cv::COLOR_YUV420sp2GRAY);
    return des;

}

OPCVUtils *OPCVUtils::getInstance() {
    std::lock_guard<std::mutex> lock(OPCVUtils::instanceMtx);
    if (instance == nullptr) {
        instance = new OPCVUtils();
    }
    return instance;
}


void OPCVUtils::loadModel(std::string facepoint, std::string faceModel) {
    {
        std::lock_guard<std::mutex> lock(OPCVUtils::faceMtx);
        dlib::deserialize(faceModel) >> predictor->face_predictor;
    }
    {
        std::lock_guard<std::mutex> lock(OPCVUtils::facePointMtx);
        dlib::deserialize(facepoint) >> predictor->facepoint_predictor;
    }
}


std::vector<dlib::full_object_detection> OPCVUtils::detectorImg(cv::Mat& img) {
    dlib::cv_image<dlib::rgb_pixel> dlibCVImg(img);
    dlib::matrix<dlib::rgb_pixel> dlibMatrix;
    dlib::assign_image(dlibMatrix, dlibCVImg);
    
    std::vector<dlib::mmod_rect> faceDets;
    {
    std::lock_guard<std::mutex> faceLocker(OPCVUtils::faceMtx);
    faceDets = predictor->face_predictor(dlibMatrix);
    }
    std::vector<dlib::full_object_detection> facePoints;
    for (const auto& faceDet : faceDets) {
        dlib::rectangle face = faceDet.rect;
        std::lock_guard<std::mutex> pointLocker(OPCVUtils::facePointMtx);
        dlib::full_object_detection shape = predictor->facepoint_predictor(dlibCVImg, face);
        facePoints.push_back(shape);
    }
    return facePoints;
}

std::vector<dlib::full_object_detection> OPCVUtils::detectorGrayImg(cv::Mat& img) {
    // 将灰度图转换为RGB图
    cv::Mat rgb_img;
    cvtColor(img, rgb_img, cv::COLOR_GRAY2RGB);
    
    dlib::cv_image<dlib::rgb_pixel> dlibCVImg(rgb_img);
    dlib::matrix<dlib::rgb_pixel> dlibMatrix;
    dlib::assign_image(dlibMatrix, dlibCVImg);
    
    std::vector<dlib::mmod_rect> faceDets;
    {
    std::lock_guard<std::mutex> faceLocker(OPCVUtils::faceMtx);
    faceDets = predictor->face_predictor(dlibMatrix);
    }
    std::vector<dlib::full_object_detection> facePoints;
    for (const auto& faceDet : faceDets) {
        dlib::rectangle face = faceDet.rect;
        std::lock_guard<std::mutex> pointLocker(OPCVUtils::facePointMtx);
        dlib::full_object_detection shape = predictor->facepoint_predictor(dlibCVImg, face);
        facePoints.push_back(shape);
    }
    return facePoints;
}

std::vector<dlib::full_object_detection> OPCVUtils::detectorImg2(cv::Mat& img) {
    dlib::frontal_face_detector faceDetector = dlib::get_frontal_face_detector();
    dlib::cv_image<dlib::bgr_pixel> dlibFrame(img);
    std::vector<dlib::rectangle> faces;
    {
    std::lock_guard<std::mutex> locker(OPCVUtils::faceMtx);
    std::vector<dlib::rectangle> faces = faceDetector(dlibFrame);
    }
    std::vector<dlib::full_object_detection> facePoints;
    for (const auto& faceRect : faces) {
        dlib::rectangle face(faceRect.left(), faceRect.top(), faceRect.right(), faceRect.bottom());
        std::lock_guard<std::mutex> pointLocker(OPCVUtils::facePointMtx);
        dlib::full_object_detection shape = predictor->facepoint_predictor(dlibFrame, face);
        facePoints.push_back(shape);
    }
    return facePoints;
}

void OPCVUtils::detectorImgTest(cv::Mat& img, std::vector<float> &vct) {
    dlib::cv_image<dlib::rgb_pixel> dlibCVImg(img);
    dlib::matrix<dlib::rgb_pixel> dlibMatrix;
    dlib::assign_image(dlibMatrix, dlibCVImg);
    
    std::vector<dlib::mmod_rect> faceDets;
    {
    std::lock_guard<std::mutex> faceLocker(OPCVUtils::faceMtx);
    faceDets = predictor->face_predictor(dlibMatrix);
    }
    for (const auto& faceDet : faceDets) {
        dlib::rectangle face = faceDet.rect;
        std::lock_guard<std::mutex> pointLocker(OPCVUtils::facePointMtx);
        dlib::full_object_detection shape = predictor->facepoint_predictor(dlibCVImg, face);
        for (int i = 1; i < 15; i++) {
            vct.push_back(shape.part(28).x());
            vct.push_back(shape.part(28).y());

            vct.push_back(shape.part(i).x());
            vct.push_back(shape.part(i).y());

            vct.push_back(shape.part(i+1).x());
            vct.push_back(shape.part(i+1).y());
        }
        for (int i = 0; i < shape.num_parts(); i++) {
            cv::circle(img, cv::Point(shape.part(i).x(), shape.part(i).y()), 3, cv::Scalar(255, 0, 0));
        }
        cv::rectangle(img, cv::Point(face.left(), face.top()), cv::Point(face.right(), face.bottom()), cv::Scalar(0, 0, 255));
    }
}


void printCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    printf("+++++++++++++++start %s \n", std::ctime(&now_c));
}

