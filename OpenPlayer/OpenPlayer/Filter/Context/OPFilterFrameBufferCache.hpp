//
//  OPFilterFrameBufferCache.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/10.
//

#ifndef OPFilterFrameBufferCache_hpp
#define OPFilterFrameBufferCache_hpp

#include <stdio.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <list>
#include "OPFilterFrameBuffer.hpp"

class OPFilterFrameBufferCache;

class OPFilterFrameBufferBox {
public:
    OPFilterFrameBufferBox();
    ~OPFilterFrameBufferBox();
    
    std::shared_ptr<OPFilterFrameBuffer> frameBuffer;
    std::weak_ptr<OPFilterFrameBufferCache> weakCache;
};

class OPFilterFrameBufferCache : public std::enable_shared_from_this<OPFilterFrameBufferCache> {
private:
    std::unordered_map<std::string, std::list<std::shared_ptr<OPFilterFrameBufferBox>>> bufferCache;
    std::string keyWithFrameBuffer(std::shared_ptr<OPFilterFrameBuffer> frameBuffer);
    std::string keyWithFrameBuffer(GLsizei width, GLsizei height, OPFilterTextureOptions options, bool onlyTexture);
public:
    OPFilterFrameBufferCache() {}
    ~OPFilterFrameBufferCache() {}
    
    std::shared_ptr<OPFilterFrameBufferBox> fetchFramebufferForSize(GLsizei width, GLsizei height, bool onlyTexture = false);
    std::shared_ptr<OPFilterFrameBufferBox> fetchFramebufferForSize(GLsizei width, GLsizei height, OPFilterTextureOptions options, bool onlyTexture = false);
    
    void storeInCache(std::shared_ptr<OPFilterFrameBuffer> frameBuffer);
};

#endif /* OPFilterFrameBufferCache_hpp */
