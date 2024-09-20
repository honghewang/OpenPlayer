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
std::string OPFilterYUVFragmentShaderString = "OPFilterYUVFragmentShaderString";

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
    },{OPFilterYUVFragmentShaderString,
       R"(
       precision mediump float;
       varying vec2 textureCoordinate;
       uniform sampler2D SamplerY;
       uniform sampler2D SamplerUV;
       uniform mat3 colorConversionMatrix;
       void main() {
         mediump vec3 yuv;
         lowp vec3 rgb;
         yuv.x = (texture2D(SamplerY, textureCoordinate).r - (16.0/255.0));
         yuv.yz = (texture2D(SamplerUV, textureCoordinate).ra - vec2(0.5, 0.5));
         rgb = colorConversionMatrix * yuv;
         gl_FragColor = vec4(rgb, 1);
       }
       )"
    },
};
