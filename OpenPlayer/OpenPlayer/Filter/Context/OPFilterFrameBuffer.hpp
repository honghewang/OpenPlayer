//
//  OPFilterFrameBuffer.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/10.
//

#ifndef OPFilterFrameBuffer_hpp
#define OPFilterFrameBuffer_hpp

#include <stdio.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

struct OPFilterTextureOptions {
    GLenum minFilter;
    GLenum magFilter;
    GLenum wrapS;
    GLenum wrapT;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
    
    OPFilterTextureOptions() {
        minFilter = GL_LINEAR;
        magFilter = GL_LINEAR;
        wrapS = GL_CLAMP_TO_EDGE;
        wrapT = GL_CLAMP_TO_EDGE;
        internalFormat = GL_RGBA;
        format = GL_BGRA;
        type = GL_UNSIGNED_BYTE;
    }
    ~OPFilterTextureOptions() {
        
    }
};

struct OPFilterSize {
    GLsizei width;
    GLsizei height;
};

class OPFilterFrameBuffer {
    
private:
    
    void generateTexture();
    void generateFramebuffer();
    void destroyFramebuffer();
    
    void initFrameBuffer(bool onlyTexture);
public:
    
    GLuint framebuffer;
    GLuint texture;
    OPFilterTextureOptions textureOptions;
    OPFilterSize size;
    bool onlyTexture;
    
    OPFilterFrameBuffer(GLsizei width, GLsizei height, bool onlyTexture = false);
    
    OPFilterFrameBuffer(GLsizei width, GLsizei height, OPFilterTextureOptions options, bool onlyT = false);
    ~OPFilterFrameBuffer();
    void activateFramebuffer();
};

#endif /* OPFilterFrameBuffer_hpp */
