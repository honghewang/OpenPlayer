//
//  OPFilterProgram.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/10.
//

#ifndef OPFilterProgram_hpp
#define OPFilterProgram_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <OpenGLES/ES2/gl.h>

class OPFilterProgram {
private:
    GLuint program;
    std::vector<std::string> attributes;
    std::vector<GLuint> uniforms;
    
    GLuint vertShader;
    GLuint fragShader;
    
    bool compileShader(GLuint &shader, GLenum type, std::string shaderString);
    
public:
    OPFilterProgram(std::string vShader, std::string fShader);
    ~OPFilterProgram();
    
    bool initialized = false;
    
    void addAttribute(std::string attributeName);
    GLuint attributeIndex(std::string attributeName);
    GLuint uniformIndex(std::string uniformName);
    void validate();
    void use();
    bool link();
};

#endif /* OPFilterProgram_hpp */
