//
//  OPCVUtils.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/8.
//

#ifndef OPCVUtils_hpp
#define OPCVUtils_hpp

#include <stdio.h>
#include <opencv2/core.hpp>
#include <vector>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>

class OPCVPredictor;

class OPCVUtils {
private:
    OPCVPredictor *predictor;
    
    OPCVUtils();
    ~OPCVUtils();
  
    // 禁止拷贝
    OPCVUtils(const OPCVUtils&) = delete;
    OPCVUtils& operator=(const OPCVUtils&) = delete;
    
public:
    static OPCVUtils *instance;
    static std::mutex mtx;
    static OPCVUtils *getInstance();
    
    static cv::Mat imgWithFile(std::string file);
    static cv::Mat imgRGBWithFile(std::string file);
    static cv::Mat imgRGBWithMat(cv::Mat& mat);
    static cv::Mat imgRGBWithYUVMat(cv::Mat& yuv);
    static cv::Mat imgGrayWithYUVMat(cv::Mat& yuv);
    
    void loadModel(std::string facepoint, std::string faceModel);
    
    std::vector<dlib::full_object_detection> detectorImg(cv::Mat& img);
    std::vector<dlib::full_object_detection> detectorGrayImg(cv::Mat& img);
    // frontal_face_detector 检测人脸，运行速度慢，可能是模拟器原因
    std::vector<dlib::full_object_detection> detectorImg2(cv::Mat& img);
    
    void detectorImgTest(cv::Mat& img, std::vector<float> &vct);
};


#endif /* OPCVUtils_hpp */
