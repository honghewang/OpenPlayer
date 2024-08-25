//
//  OPCVPredictor.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/8.
//

#ifndef OPCVPredictor_hpp
#define OPCVPredictor_hpp

#include <stdio.h>
#import <dlib/opencv.h>
#import <dlib/image_processing.h>
#import <dlib/image_processing/frontal_face_detector.h>
#import <dlib/image_processing/render_face_detections.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

class OPCVPredictor {
public:
    dlib::shape_predictor share_predictor;
    cv::CascadeClassifier model;
};

#endif /* OPCVPredictor_hpp */
