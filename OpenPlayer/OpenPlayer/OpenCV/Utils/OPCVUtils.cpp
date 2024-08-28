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
std::mutex  OPCVUtils::mtx;

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

OPCVUtils *OPCVUtils::getInstance() {
    std::lock_guard<std::mutex> lock(mtx);
    if (instance == nullptr) {
        instance = new OPCVUtils();
    }
    return instance;
}


void OPCVUtils::loadModel(std::string facepoint, std::string faceModel) {
    dlib::deserialize(facepoint) >> predictor->share_predictor;
    predictor->model.load(faceModel.c_str());
}


std::vector<dlib::full_object_detection> OPCVUtils::detectorImg(cv::Mat& img) {
    cv::Mat des;
    cv::cvtColor(img, des, cv::COLOR_BGR2GRAY);
    std::vector<cv::Rect> faces;
    dlib::cv_image<dlib::bgr_pixel> dlibFrame(img);
    
    {
        std::lock_guard<std::mutex> lock(mtx);
        predictor->model.detectMultiScale(des, faces);
    }
    std::vector<dlib::full_object_detection> facePoints;
    for (const auto& faceRect : faces) {
        dlib::rectangle face(faceRect.x, faceRect.y, faceRect.x + faceRect.width, faceRect.y + faceRect.height);
        dlib::full_object_detection shape = predictor->share_predictor(dlibFrame, face);
        facePoints.push_back(shape);
    }
    return facePoints;
}

std::vector<dlib::full_object_detection> OPCVUtils::detectorImg2(cv::Mat& img) {
    dlib::frontal_face_detector faceDetector = dlib::get_frontal_face_detector();
    dlib::cv_image<dlib::bgr_pixel> dlibFrame(img);
    std::vector<dlib::rectangle> faces = faceDetector(dlibFrame);
    std::vector<dlib::full_object_detection> facePoints;
    for (const auto& faceRect : faces) {
        dlib::rectangle face(faceRect.left(), faceRect.top(), faceRect.right(), faceRect.bottom());
        dlib::full_object_detection shape = predictor->share_predictor(dlibFrame, face);
        facePoints.push_back(shape);
    }
    return facePoints;
}

void OPCVUtils::detectorImgTest(cv::Mat& img, std::vector<float> &vct) {
    cv::Mat des;
    cv::cvtColor(img, des, cv::COLOR_BGR2GRAY);
    std::vector<cv::Rect> faces;
    dlib::cv_image<dlib::bgr_pixel> dlibFrame(img);
    
    predictor->model.detectMultiScale(des, faces);
    for (const auto& faceRect : faces) {
        dlib::rectangle face(faceRect.x, faceRect.y, faceRect.x + faceRect.width, faceRect.y + faceRect.height);
        dlib::full_object_detection shape = predictor->share_predictor(dlibFrame, face);
        
        for (int i = 1; i < 15; i++) {
            vct.push_back(shape.part(28).x());
            vct.push_back(shape.part(28).y());
            
            vct.push_back(shape.part(i).x());
            vct.push_back(shape.part(i).y());
            
            vct.push_back(shape.part(i+1).x());
            vct.push_back(shape.part(i+1).y());
        }
        for (int i = 0; i < shape.num_parts(); i++) {
            cv::circle(img, cv::Point(shape.part(i).x(), shape.part(i).y()), 3, cv::Scalar(0, 0, 255));
        }
        cv::rectangle(img, cv::Point(face.left(), face.top()), cv::Point(face.right(), face.bottom()), cv::Scalar(255, 0, 0));
    }
}


void printCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    printf("+++++++++++++++start %s \n", std::ctime(&now_c));
}

