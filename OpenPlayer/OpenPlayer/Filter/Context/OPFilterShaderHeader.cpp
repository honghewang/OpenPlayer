//
//  OPFilterShaderHeader.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/20.
//

#include "OPFilterShaderHeader.hpp"

std::string OPFilterVertexShaderString = "OPFilterVertexShaderString";
std::string OPFilterPointVertexShaderString = "OPFilterPointVertexShaderString";

std::string OPFilterPointFragmentShaderString = "OPFilterPointFragmentShaderString";
std::string OPFilterGrayFragmentShadeString = "OPFilterGrayFragmentShadeString";
std::string OPFilterPassthroughFragmentShaderString = "OPFilterPassthroughFragmentShaderString";

std::map<std::string, std::string> OPVertexShaderMap = {
    {   OPFilterVertexShaderString,
        R"(
        attribute vec4 position;
        attribute vec4 inputTextureCoordinate;
        varying vec2 textureCoordinate;
        void main()
        {
            gl_Position = position;
            textureCoordinate = inputTextureCoordinate.xy;
        }
        )"
    }, {   OPFilterPointVertexShaderString,
        R"(
        attribute vec4 position;
        void main()
        {
            gl_Position = position;
            gl_PointSize = 5.0;
        }
        )"
    }
};

std::map<std::string, std::string> OPFragmentShaderMap = {
    {   OPFilterPassthroughFragmentShaderString,
        R"(
         varying highp vec2 textureCoordinate;
         uniform sampler2D inputImageTexture;
         void main()
         {
             gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
         }
        )"
    },{ OPFilterPointFragmentShaderString,
        R"(
         void main()
         {
             gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
         }
        )"
        
    },{OPFilterGrayFragmentShadeString,
       R"(
       precision highp float;
       
       varying vec2 textureCoordinate;
       
       uniform sampler2D inputImageTexture;
       
       const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);
       
       void main()
       {
           lowp vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
           float luminance = dot(textureColor.rgb, W);
           
           gl_FragColor = vec4(vec3(luminance), textureColor.a);
       }
       )"
    },
};
