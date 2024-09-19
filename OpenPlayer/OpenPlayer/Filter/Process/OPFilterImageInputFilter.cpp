//
//  OPFilterImageInputFilter.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/25.
//

#include "OPFilterImageInputFilter.hpp"

//YUV->RGB
//颜色转换常量（yuv到rgb），包括从16-235/16-240（视频范围）进行调整
static const GLfloat kColorConversion601[] = {
    1.164,  1.164, 1.164,
    0.0, -0.392, 2.017,
    1.596, -0.813,   0.0,
};

// BT.709, 这是高清电视的标准
static const GLfloat kColorConversion709[] = {
    1.164,  1.164, 1.164,
    0.0, -0.213, 2.112,
    1.793, -0.533,   0.0,
};


std::shared_ptr<OPFilterFrameBufferBox> OPFilterImageInputFilter::render() {
    if (infoCenter.expired() || inputMat.get() == nullptr) {
        return nullptr;
    }
    if (inputMat->imageMat.cols == 0 || inputMat->imageMat.rows == 0) {
        return nullptr;
    }
    if (inputMat->type == OPFilterInputMatType_YUV) {
        return renderYUV();
    }
    return renderRGB();
}
//std::shared_ptr<OPFilterFrameBufferBox> OPFilterImageInputFilter::renderYUV() {
//    std::shared_ptr<OPFilterFrameBufferBox> frameBufferBoxY = infoCenter.lock()->bufferCache->fetchFramebufferForSize(inputMat->imageMat.cols, inputMat->imageMat.rows, true);
//    GLuint textureY = frameBufferBoxY->frameBuffer->texture;
////    GLuint uniformY = program->uniformIndex("SamplerY");
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, textureY);
//    cv::Mat matY = inputMat->imageMat;
//    if (!matY.isContinuous()) {
//        matY = inputMat->imageMat.clone();
//    }
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT, matY.cols, matY.rows, 0, GL_RED_EXT, GL_UNSIGNED_BYTE, matY.data);
//    return frameBufferBoxY;
//}

std::shared_ptr<OPFilterFrameBufferBox> OPFilterImageInputFilter::renderYUV() {

    auto info = infoCenter.lock();
    std::shared_ptr<OPFilterFrameBufferBox> frameBufferBox = info->bufferCache->fetchFramebufferForSize(info->width, info->height);
    frameBufferBox->frameBuffer->activateFramebuffer();

    //1.设置视口大小
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);


    auto program = info->progamManager->programFromCache(OPFilterVertexShaderString, OPFilterYUVFragmentShaderString);
    if (!program->initialized) {
        program->addAttribute("position");
        program->addAttribute("inputTextureCoordinate");
        if (!program->link()) {
            return nullptr;
        }
    }
    program->validate();
    program->use();


    //6.设置顶点、纹理坐标
    //前3个是顶点坐标，后2个是纹理坐标
    GLfloat attrArr[] =
    {
        1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,

        1.0f, 1.0f, 0.0f,      1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
    };
    GLuint attrBuffer;
    //(2)申请一个缓存区标识符
    glGenBuffers(1, &attrBuffer);
    //(3)将attrBuffer绑定到GL_ARRAY_BUFFER标识符上
    glBindBuffer(GL_ARRAY_BUFFER, attrBuffer);
    //(4)把顶点数据从CPU内存复制到GPU上
    glBufferData(GL_ARRAY_BUFFER, sizeof(attrArr), attrArr, GL_STREAM_DRAW);


    std::shared_ptr<OPFilterFrameBufferBox> frameBufferBoxY = infoCenter.lock()->bufferCache->fetchFramebufferForSize(inputMat->imageMat.cols, inputMat->imageMat.rows, true);
    GLuint textureY = frameBufferBoxY->frameBuffer->texture;
    GLuint uniformY = program->uniformIndex("SamplerY");
    glActiveTexture(GL_TEXTURE0 + uniformY);
    glBindTexture(GL_TEXTURE_2D, textureY);
    cv::Mat matY = inputMat->imageMat;
    if (!matY.isContinuous()) {
        matY = inputMat->imageMat.clone();
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, matY.cols, matY.rows, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, matY.data);

    std::shared_ptr<OPFilterFrameBufferBox> frameBufferBoxUV = infoCenter.lock()->bufferCache->fetchFramebufferForSize(inputMat->imageMat2.cols, inputMat->imageMat2.rows, true);

    GLuint textureUV = frameBufferBoxUV->frameBuffer->texture;
    GLuint uniformUV = program->uniformIndex("SamplerUV");
    glActiveTexture(GL_TEXTURE0 + uniformUV);
    glBindTexture(GL_TEXTURE_2D, textureUV);
    cv::Mat matUV = inputMat->imageMat2;
    if (!matUV.isContinuous()) {
        matUV = inputMat->imageMat.clone();
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, matUV.cols, matUV.rows, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, matUV.data);

    GLuint position = program->attributeIndex("position");
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, NULL);

    GLuint textCoor = program->attributeIndex("inputTextureCoordinate");
    glEnableVertexAttribArray(textCoor);
    glVertexAttribPointer(textCoor, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (float *)NULL + 3);
    
    const GLfloat *conversion = inputMat->isHighStandard ? kColorConversion709 : kColorConversion601;
    GLuint uniformMatrix = program->uniformIndex("colorConversionMatrix");
    glUniformMatrix3fv(uniformMatrix, 1, GL_FALSE, conversion);

    //12.绘图
    glDrawArrays(GL_TRIANGLES, 0, 6);

    return frameBufferBox;
}


std::shared_ptr<OPFilterFrameBufferBox> OPFilterImageInputFilter::renderRGB() {
    std::shared_ptr<OPFilterFrameBufferBox> frameBufferBox = infoCenter.lock()->bufferCache->fetchFramebufferForSize(inputMat->imageMat.cols, inputMat->imageMat.rows, true);

    GLuint texture = frameBufferBox->frameBuffer->texture;
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
    cv::Mat mat = inputMat->imageMat;
    if (!mat.isContinuous()) {
        mat = inputMat->imageMat.clone();
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mat.cols, mat.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, mat.data);
    return frameBufferBox;
}
