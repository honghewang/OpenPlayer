//
//  OPFilterImageInputFilter.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/25.
//

#include "OPFilterImageInputFilter.hpp"


void OPFilterImageInputFilter::render() {
    std::shared_ptr<OPFilterFrameBufferBox> frameBufferBox;
    if (infoCenter.expired()) {
        return;
    }
    if (imageMat.cols == 0 || imageMat.rows == 0) {
        return;
    }
    frameBufferBox = infoCenter.lock()->bufferCache->fetchFramebufferForSize(imageMat.cols, imageMat.rows, true);
    
    GLuint texture = frameBufferBox->frameBuffer->texture;
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
    cv::Mat mat = imageMat;
    if (!mat.isContinuous()) {
        mat = imageMat.clone();
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mat.cols, mat.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, mat.data);
    outputTexture = texture;
}
