//
//  OPFilterTextureFilter.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/27.
//

#include "OPFilterTextureFilter.hpp"

std::shared_ptr<OPFilterFrameBufferBox> OPFilterTextureFilter::render() {
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
    return frameBufferBox;
}

void OPFilterTextureFilter::renderTexture(std::shared_ptr<OPFilterRenderFilterInfoCenter> info) {
    auto program = info->progamManager->programFromCache(vshader, fshader);
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
    glActiveTexture(GL_TEXTURE0+mapUniform);
    glBindTexture(GL_TEXTURE_2D, inputInfos.front()->frameBufferBox->frameBuffer->texture);
    //12.绘图
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

