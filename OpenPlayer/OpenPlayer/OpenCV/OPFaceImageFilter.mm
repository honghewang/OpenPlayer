//
//  OPFaceImageFilter.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/20.
//

#import "OPFaceImageFilter.h"


NSString * const kCCGPUImageFisterFilterShaderString = SHADER_STRING
(
 precision highp float;
 
 uniform sampler2D inputImageTexture;
 varying vec2 textureCoordinate;
 
 void main (void) {
     gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
 }

 );

// Hardcode the vertex shader for standard filters, but this can be overridden
NSString *const kGPUImageSecondVertexShaderString = SHADER_STRING
(
 attribute vec4 position;
 attribute vec4 inputTextureCoordinate;
 
 varying vec2 textureCoordinate;
 
 void main()
 {
     gl_Position = position;
     textureCoordinate = inputTextureCoordinate.xy;
 }
 );

NSString *const kCCGPUImageSecondFilterShaderString = SHADER_STRING
(
 precision mediump float;
 
 void main()
 {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
 }
);

@implementation OPFaceImageFilter

- (instancetype)init {
    
    return [self initWithFirstStageVertexShaderFromString:kGPUImageVertexShaderString firstStageFragmentShaderFromString:kCCGPUImageFisterFilterShaderString secondStageVertexShaderFromString:kGPUImageSecondVertexShaderString secondStageFragmentShaderFromString:kCCGPUImageSecondFilterShaderString];
}



#pragma mark Rendering

- (void)renderToTextureWithVertices:(const GLfloat *)vertices textureCoordinates:(const GLfloat *)textureCoordinates;
{
    if (self.preventRendering)
    {
        [firstInputFramebuffer unlock];
        return;
    }
    
    [GPUImageContext setActiveShaderProgram:filterProgram];
    
    outputFramebuffer = [[GPUImageContext sharedFramebufferCache] fetchFramebufferForSize:[self sizeOfFBO] textureOptions:self.outputTextureOptions onlyTexture:NO];
    [outputFramebuffer activateFramebuffer];
    
    [self setUniformsForProgramAtIndex:0];
    
    glClearColor(backgroundColorRed, backgroundColorGreen, backgroundColorBlue, backgroundColorAlpha);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, [firstInputFramebuffer texture]);
    
    glUniform1i(filterInputTextureUniform, 2);
    
    glVertexAttribPointer(filterPositionAttribute, 2, GL_FLOAT, 0, 0, vertices);
    glVertexAttribPointer(filterTextureCoordinateAttribute, 2, GL_FLOAT, 0, 0, textureCoordinates);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    [firstInputFramebuffer unlock];
    firstInputFramebuffer = nil;
    
    [GPUImageContext setActiveShaderProgram:secondFilterProgram];
    if (usingNextFrameForImageCapture)
    {
        [outputFramebuffer lock];
    }

    [self setUniformsForProgramAtIndex:1];
    
    unsigned int count = self.facePoints.count;
    CGSize size = [self sizeOfFBO];
    
    GLfloat *imgVertices = new GLfloat[count];
    
    for (NSInteger i = 0; i < count / 2; i++) {
        imgVertices[i * 2] = self.facePoints[i * 2].floatValue / size.width * 2 - 1;
        imgVertices[i * 2 + 1] = self.facePoints[i * 2 + 1].floatValue / size.height * 2 - 1;
    }
    
    glVertexAttribPointer(secondFilterPositionAttribute, 2, GL_FLOAT, 0, 0, imgVertices);

    glDrawArrays(GL_TRIANGLES, 0, count  / 6 * 3);
    
    if (usingNextFrameForImageCapture)
    {
        dispatch_semaphore_signal(imageCaptureSemaphore);
    }
    
    delete[] imgVertices;
}

- (GPUImageFramebuffer *)framebufferForOutput;
{
    return outputFramebuffer;
}

- (void)removeOutputFramebuffer;
{
    outputFramebuffer = nil;
}

@end
