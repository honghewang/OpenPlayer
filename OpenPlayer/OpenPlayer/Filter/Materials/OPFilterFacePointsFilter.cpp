//
//  OPFilterFacePointsFilter.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/25.
//

#include "OPFilterFacePointsFilter.hpp"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>


std::shared_ptr<OPFilterFrameBufferBox> OPFilterFacePointsFilter::render() {
    if (infoCenter.expired()) {
        return nullptr;
    }
    if (inputInfos.size() == 0 || inputInfos.front()->frameBufferBox.get() == nullptr) {
        return nullptr;
    }
    
    auto info = infoCenter.lock();
    std::shared_ptr<OPFilterFrameBufferBox> frameBufferBox = info->bufferCache->fetchFramebufferForSize(info->width, info->height);
    frameBufferBox->frameBuffer->activateFramebuffer();
    
    //1.设置视口大小
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    renderTexture(info);
    renderFacePoints(info);
    return frameBufferBox;
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
