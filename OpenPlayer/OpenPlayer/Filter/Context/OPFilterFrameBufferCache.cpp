//
//  OPFilterFrameBufferCache.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/10.
//

#include "OPFilterFrameBufferCache.hpp"
#include <sstream>


OPFilterFrameBufferBox::OPFilterFrameBufferBox() {
    
}

OPFilterFrameBufferBox::~OPFilterFrameBufferBox() {
    // 释放时 存入缓存
    if (!weakCache.expired() && frameBuffer.get() != nullptr) {
        weakCache.lock()->storeInCache(frameBuffer);
    }
}

std::shared_ptr<OPFilterFrameBufferBox> OPFilterFrameBufferCache::fetchFramebufferForSize(GLsizei width, GLsizei height, bool onlyTexture) {
    OPFilterTextureOptions options;
    return fetchFramebufferForSize(width, height, options, onlyTexture);
}

std::shared_ptr<OPFilterFrameBufferBox> OPFilterFrameBufferCache::fetchFramebufferForSize(GLsizei width, GLsizei height, OPFilterTextureOptions options, bool onlyTexture) {
    std::string key = keyWithFrameBuffer(width, height, options, onlyTexture);
    auto it = bufferCache.find(key);
    if (it != bufferCache.end()) {
        // 从cache中取出
        auto &frameBufferBoxList = it->second;
        if (frameBufferBoxList.size() > 0) {
            auto frameBufferBox = frameBufferBoxList.front();
            frameBufferBoxList.pop_front();
            return frameBufferBox;
        }
    }
    // 创建
    std::shared_ptr<OPFilterFrameBufferBox> frameBufferBox = std::make_shared<OPFilterFrameBufferBox>();
    frameBufferBox->frameBuffer = std::make_shared<OPFilterFrameBuffer>(width, height, options, onlyTexture);
    frameBufferBox->weakCache = weak_from_this();
    return frameBufferBox;
}

void OPFilterFrameBufferCache::storeInCache(std::shared_ptr<OPFilterFrameBuffer> frameBuffer) {
    if (frameBuffer.get() == nullptr) {
        return;
    }
    
    std::shared_ptr<OPFilterFrameBufferBox> frameBufferBox = std::make_shared<OPFilterFrameBufferBox>();
    frameBufferBox->frameBuffer = frameBuffer;
    frameBufferBox->weakCache = weak_from_this();

    std::string key = keyWithFrameBuffer(frameBuffer);
    if (bufferCache.find(key) == bufferCache.end()) {
        std::list<std::shared_ptr<OPFilterFrameBufferBox>> boxList;
        boxList.push_back(frameBufferBox);
        bufferCache[key] = boxList;
    } else {
        auto &boxList = bufferCache[key];
        boxList.push_back(frameBufferBox);
    }
}

std::string OPFilterFrameBufferCache::keyWithFrameBuffer(GLsizei width, GLsizei height, OPFilterTextureOptions options, bool onlyTexture) {
    std::string result;
     
    // 将宽度和高度转换为字符串并拼接
    result += std::to_string(width) + ":" + std::to_string(height) + "-";
  
    // 添加纹理选项
    result += std::to_string(static_cast<int>(options.minFilter)) + ":";
    result += std::to_string(static_cast<int>(options.magFilter)) + ":";
    result += std::to_string(static_cast<int>(options.wrapS)) + ":";
    result += std::to_string(static_cast<int>(options.wrapT)) + ":";
  
    // 假设 internalFormat, format, type 已经是整数类型或可以被 std::to_string 直接处理的类型
    result += std::to_string(options.internalFormat) + ":";
    result += std::to_string(options.format) + ":";
    result += std::to_string(options.type);
  
    // 如果 onlyTexture 为真，则添加 "-NOFB"
    if (onlyTexture) {
        result += "-NOFB";
    }
  
    return result;
    
}

std::string OPFilterFrameBufferCache::keyWithFrameBuffer(std::shared_ptr<OPFilterFrameBuffer> frameBuffer) {
    return keyWithFrameBuffer(frameBuffer->size.width, frameBuffer->size.height, frameBuffer->textureOptions, frameBuffer->onlyTexture);
}

