//
//  OPFilterFacePointsFilter.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/25.
//

#include "OPFilterFacePointsFilter.hpp"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>


void OPFilterFacePointsFilter::render() {
    if (infoCenter.expired()) {
        return;
    }
    if (inputInfos.size() == 0 || inputInfos.front()->textureId == 0) {
        return;
    }
    
    auto info = infoCenter.lock();
    frameBufferBox = info->bufferCache->fetchFramebufferForSize(info->width, info->height);
    frameBufferBox->frameBuffer->activateFramebuffer();
    
    //1.设置视口大小
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    renderTexture(info);
    renderFacePoints(info);
    
    outputTexture = frameBufferBox->frameBuffer->texture;
}

void OPFilterFacePointsFilter::renderTexture(std::shared_ptr<OPFilterRenderFilterInfoCenter> info) {
    auto program = info->progamManager->programFromCache(OPFilterVertexShaderString, OPFilterPassthroughFragmentShaderString);
    if (!program->initialized) {
        program->addAttribute("position");
        program->addAttribute("inputTextureCoordinate");
        if (!program->link()) {
            return;
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
    
    
    GLuint position = program->attributeIndex("position");
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, NULL);
    
    GLuint textCoor = program->attributeIndex("inputTextureCoordinate");
    glEnableVertexAttribArray(textCoor);
    glVertexAttribPointer(textCoor, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, (float *)NULL + 3);
    
    GLuint mapUniform = program->uniformIndex("inputImageTexture");
    glActiveTexture(GL_TEXTURE0 + mapUniform);
    glBindTexture(GL_TEXTURE_2D, inputInfos.front()->textureId);
    //12.绘图
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void OPFilterFacePointsFilter::renderFacePoints(std::shared_ptr<OPFilterRenderFilterInfoCenter> info) {
    std::vector<GLfloat> vec;
    std::vector<dlib::full_object_detection> &facePoints = info->aiInfo->facePoints;
    for (auto &shape : facePoints) {
        for (int index = 0; index < shape.num_parts(); index++) {
            vec.push_back(1.0 * shape.part(index).x() / info->width * 2 - 1);
            vec.push_back(1.0 * shape.part(index).y() / info->height * 2 - 1);
        }
    }
    if (vec.size() == 0) {
        return;
    }
    auto program = info->progamManager->programFromCache(OPFilterPointVertexShaderString, OPFilterPointFragmentShaderString);
    if (!program->initialized) {
        program->addAttribute("position");
        if (!program->link()) {
            return;
        }
    }
    program->validate();
    program->use();

    
    GLuint attrBuffer;
    //(2)申请一个缓存区标识符
    glGenBuffers(1, &attrBuffer);
    //(3)将attrBuffer绑定到GL_ARRAY_BUFFER标识符上
    glBindBuffer(GL_ARRAY_BUFFER, attrBuffer);
    //(4)把顶点数据从CPU内存复制到GPU上
    glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(GLfloat), vec.data(), GL_STREAM_DRAW);
    
    
    GLuint position = program->attributeIndex("position");
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, NULL);
    
    //12.绘图
    glDrawArrays(GL_POINTS, 0, vec.size() / 2);
}