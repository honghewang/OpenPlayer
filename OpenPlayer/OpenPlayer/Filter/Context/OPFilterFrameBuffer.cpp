//
//  OPFilterFrameBuffer.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/10.
//

#include "OPFilterFrameBuffer.hpp"


void OPFilterFrameBuffer::generateTexture() {
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureOptions.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureOptions.magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureOptions.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureOptions.wrapT);
}

void OPFilterFrameBuffer::generateFramebuffer() {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    generateTexture();
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, textureOptions.internalFormat, size.width, size.height, 0, textureOptions.format, textureOptions.type, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OPFilterFrameBuffer::destroyFramebuffer() {
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }
    glDeleteTextures(1, &texture);
}

OPFilterFrameBuffer::OPFilterFrameBuffer(GLsizei width, GLsizei height, bool onlyTexture) {    
    this->onlyTexture = onlyTexture;
    size.width = width;
    size.height = height;
    initFrameBuffer(onlyTexture);
}

OPFilterFrameBuffer::OPFilterFrameBuffer(GLsizei width, GLsizei height, OPFilterTextureOptions options, bool onlyTexture) {
    textureOptions = options;
    this->onlyTexture = onlyTexture;
    size.width = width;
    size.height = height;
    initFrameBuffer(onlyTexture);
}

OPFilterFrameBuffer::~OPFilterFrameBuffer() {
    destroyFramebuffer();
}

void OPFilterFrameBuffer::activateFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, size.width, size.height);
}

void OPFilterFrameBuffer::initFrameBuffer(bool onlyTexture) {
    if (onlyTexture) {
        generateTexture();
    } else {
        generateFramebuffer();
    }
}

