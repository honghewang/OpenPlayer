//
//  OPFilterProgram.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/10.
//

#include "OPFilterProgram.hpp"

OPFilterProgram::OPFilterProgram(std::string vShader, std::string fShader) {
    program = glCreateProgram();
    if (!compileShader(vertShader, GL_VERTEX_SHADER, vShader)) {
        printf("Failed to compile vertex shader");
    }
    if (!compileShader(fragShader, GL_FRAGMENT_SHADER, fShader)) {
        printf("Failed to compile fragment shader");
    }
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
}

OPFilterProgram::~OPFilterProgram() {
    if (vertShader) {
        glDeleteShader(vertShader);
    }
    if (fragShader) {
        glDeleteShader(fragShader);
    }
    if (program) {
        glDeleteProgram(program);
    }
}

void OPFilterProgram::addAttribute(std::string attributeName) {
    GLuint index = 0;
    for (auto it = attributes.begin(); it != attributes.end(); it++) {
        if (attributeName == *it) {
            return;
        }
        index++;
    }
    attributes.push_back(attributeName);
    glBindAttribLocation(program,
                         index,
                         attributeName.c_str());
}

GLuint OPFilterProgram::attributeIndex(std::string attributeName) {
    GLuint index = 0;
    for (auto it = attributes.begin(); it != attributes.end(); it++) {
        if (attributeName == *it) {
            return index;
        }
        index++;
    }
    return -1;
}

GLuint OPFilterProgram::uniformIndex(std::string uniformName) {
    return glGetUniformLocation(program, uniformName.c_str());
}

void OPFilterProgram::validate() {
    GLint logLength;
    glValidateProgram(program);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(program, logLength, &logLength, log);
        printf("validate is %s", log);
        free(log);
    }
}

void OPFilterProgram::use() {
    glUseProgram(program);
}

bool OPFilterProgram::link() {
    GLint status;
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        return false;
    }
    if (vertShader) {
        glDeleteShader(vertShader);
        vertShader = 0;
    }
    if (fragShader) {
        glDeleteShader(fragShader);
        fragShader = 0;
    }
    initialized = true;
    return true;
}

bool OPFilterProgram::compileShader(GLuint &shader, GLenum type, std::string shaderString) {
    if (shaderString.length() == 0) {
        return false;
    }
    const GLchar *source = shaderString.c_str();
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(shader, logLength, &logLength, log);
        printf("compile %i error is %s", type, log);
        free(log);
    }
    return status == GL_TRUE;
}
