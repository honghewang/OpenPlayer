//
//  OPCapturePreview.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/9/14.
//

#import "OPCapturePreview.h"
#import <CoreFoundation/CoreFoundation.h>
#include <CoreVideo/CoreVideo.h>
#include <iostream>
#import <OpenGLES/ES2/gl.h>
#import <UIKit/UIKit.h>
#include <OpenGLES/ES2/glext.h>
#include "OPFilterProgramManager.hpp"


static void refresh(std::string identity, int texture);

static void prepareContext(std::string identity);

NSString *const OPFilterContextNotification = @"OPFilterContextNotification";
NSString *const OPFilterRefreshNotification = @"OPFilterRefreshNotification";

@interface OPCapturePreview () {
    std::shared_ptr<OPFilterProcess> process;
    std::shared_ptr<OPFilterProgramManager> progamManager;
    
    GLint _renderWidth;
    GLint _renderHeight;
}

@property(nonatomic,strong) EAGLContext *context;

@property(nonatomic,assign) GLuint colorRenderBuffer;
@property(nonatomic,assign) GLuint colorFrameBuffer;

@property (nonatomic, strong) NSString *identity;
@property (nonatomic, assign) CGRect preFrame;

@property(nonatomic,weak) CAEAGLLayer *gpuLayer;

@end

@implementation OPCapturePreview

- (instancetype)initWithAsyn:(BOOL)isAsyn {
    if (self = [super init]) {
        [self setupLayer];
        [self addNotification];
        process = std::make_shared<OPFilterProcess>(isAsyn ? true : false);
        self.identity = [NSString stringWithFormat:@"%p", self];
        std::string identity = self.identity.UTF8String;
        process->setContextFunc([=]() {
            prepareContext(identity);
        });
        process->setRefreshFunc([=](int texture) {
            refresh(identity, texture);
        });
        progamManager = std::make_shared<OPFilterProgramManager>();
    }
    return self;
}

- (void)addNotification {
    [NSNotificationCenter.defaultCenter addObserver:self selector:@selector(handleContextNotification:) name:OPFilterContextNotification object:nil];
    [NSNotificationCenter.defaultCenter addObserver:self selector:@selector(handleRefreshNotification:) name:OPFilterRefreshNotification object:nil];
}

- (void)loadImgBuffer:(CVImageBufferRef)imgBuffer {
    size_t planeCount = CVPixelBufferGetPlaneCount(imgBuffer);
    OSType pixelFormat = CVPixelBufferGetPixelFormatType(imgBuffer);
    if (pixelFormat == kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange ||
        pixelFormat == kCVPixelFormatType_420YpCbCr8BiPlanarFullRange) {
        CFTypeRef colorAttachments = CVBufferGetAttachment(imgBuffer, kCVImageBufferYCbCrMatrixKey, NULL);
        CFStringRef colorAttachmentString = (CFStringRef)colorAttachments;
        if (planeCount == 2) {
            CFComparisonResult result = CFStringCompare(colorAttachmentString, kCVImageBufferYCbCrMatrix_ITU_R_601_4, 0);
            if (result == kCFCompareEqualTo) {
                CVPixelBufferLockBaseAddress(imgBuffer, kCVPixelBufferLock_ReadOnly);
                size_t planeIndexY = 0;
                void *planeBaseAddressY = CVPixelBufferGetBaseAddressOfPlane(imgBuffer, planeIndexY);
                int heightY = (int)CVPixelBufferGetHeightOfPlane(imgBuffer, planeIndexY);
//                int widthY = (int)CVPixelBufferGetWidthOfPlane(imgBuffer, planeIndexY);
                int perRowY = (int)CVPixelBufferGetBytesPerRowOfPlane(imgBuffer, planeIndexY);
                cv::Mat matY = cv::Mat(perRowY, heightY, CV_8UC1, planeBaseAddressY).clone();
                
                size_t planeIndexUV = 1;
                void *planeBaseAddressUV = CVPixelBufferGetBaseAddressOfPlane(imgBuffer, planeIndexUV);
                int heightUV = (int)CVPixelBufferGetHeightOfPlane(imgBuffer, planeIndexUV);
//                int widthUV = (int)CVPixelBufferGetWidthOfPlane(imgBuffer, planeIndexUV);
                int perRowUV = (int)CVPixelBufferGetBytesPerRowOfPlane(imgBuffer, planeIndexUV);
                cv::Mat matUV = cv::Mat(perRowUV, heightUV, CV_8UC2, planeBaseAddressUV).clone();
                CVPixelBufferUnlockBaseAddress(imgBuffer, kCVPixelBufferLock_ReadOnly);
                
            } else {
//                _preferredConversion = kColorConversion709;
                
            }
        }
    } else {
        // TODO
    }
}

- (void)setFilterLinks:(std::shared_ptr<OPFilterRenderFilterLink>)link {
    process->setFilterLinks(link);
}

- (void)dealloc {
    process->stop();
    [self deletBuffer];
}

- (void)setupOpenGLContext {
    [self deletBuffer];
    [self setupRenderBuffer];
    [self setupFrameBuffer];
}

- (void)handleContextNotification:(NSNotification *)notification {
    NSString *identity = notification.userInfo[@"id"];
    if ([identity isEqualToString:self.identity]) {
        [self setupContext];
    }
}

- (void)handleRefreshNotification:(NSNotification *)notification {
    NSString *identity = notification.userInfo[@"id"];
    int textureId = [notification.userInfo[@"texture"] intValue];
    if ([identity isEqualToString:self.identity]) {
        if (self.colorFrameBuffer == 0 || self.colorRenderBuffer == 0) {
            [self setupOpenGLContext];
        }
        [self renderTexture:textureId];
    }
}

- (void)renderTexture:(int)textureId {
    //2.设置视口
    glBindFramebuffer(GL_FRAMEBUFFER, self.colorFrameBuffer);
    glViewport(0, 0, _renderWidth, _renderHeight);
    //1.设置视口大小
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    auto program = progamManager->programFromCache(OPFilterVertexShaderString, OPFilterPassthroughFragmentShaderString);
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
        1.0f, -1.0f, 0.0f,     1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,    0.0f, 1.0f,
        
        1.0f, 1.0f, 0.0f,      1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,     1.0f, 1.0f,
    };
    
    
    //7.-----处理顶点数据--------
    //(1)顶点缓存区
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
    glActiveTexture(GL_TEXTURE0 + mapUniform);
    glBindTexture(GL_TEXTURE_2D, textureId);
    //12.绘图
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //13.从渲染缓存区显示到屏幕上
    glBindRenderbuffer(GL_RENDERBUFFER, self.colorRenderBuffer);
    [self.context presentRenderbuffer:GL_RENDERBUFFER];
}

#pragma mark - C++
void refresh(std::string identity, int texture) {
    [NSNotificationCenter.defaultCenter postNotificationName:OPFilterRefreshNotification
                                                      object:nil
                                                    userInfo:@{
        @"id" : [NSString stringWithCString:identity.c_str() encoding:NSUTF8StringEncoding],
        @"texture" : @(texture)
    }];
}

void prepareContext(std::string identity) {
    [NSNotificationCenter.defaultCenter postNotificationName:OPFilterContextNotification
                                                      object:nil
                                                    userInfo:@{
        @"id" : [NSString stringWithCString:identity.c_str() encoding:NSUTF8StringEncoding],
    }];
}

#pragma mark - layer

+ (Class)layerClass {
    return CAEAGLLayer.class;
}

#pragma mark - render

//5.设置frameBuffer
-(void)setupFrameBuffer {
    //1.定义一个缓存区
    GLuint buffer;
    //2.申请一个缓存区标志
    glGenFramebuffers(1, &buffer);
    //3.
    self.colorFrameBuffer = buffer;
    //4.设置当前的framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, self.colorFrameBuffer);
    //5.将_colorRenderBuffer 装配到GL_COLOR_ATTACHMENT0 附着点上

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_renderWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_renderHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, self.colorRenderBuffer);
}

//4.设置renderBuffer
- (void)setupRenderBuffer {
    //1.定义一个缓存区
    GLuint buffer;
    //2.申请一个缓存区标志
    glGenRenderbuffers(1, &buffer);
    //3.
    self.colorRenderBuffer = buffer;
    //4.将标识符绑定到GL_RENDERBUFFER
    glBindRenderbuffer(GL_RENDERBUFFER, self.colorRenderBuffer);
    [self.context renderbufferStorage:GL_RENDERBUFFER fromDrawable:self.gpuLayer];
    
}

//3.清空缓存区
- (void)deletBuffer {
    if (_colorRenderBuffer != 0) {
        glDeleteRenderbuffers(1, &_colorRenderBuffer);
        _colorRenderBuffer = 0;
    }
    if (_colorFrameBuffer != 0) {
        glDeleteFramebuffers(1, &_colorFrameBuffer);
        _colorFrameBuffer = 0;
    }
}


//2.设置上下文
- (void)setupContext {
    if (!_context) {
        EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
        EAGLContext *context = [[EAGLContext alloc] initWithAPI:api];
        if (!context) {
            NSLog(@"Create Context Failed");
            return;
        }
        _context = context;
    }
    if (![EAGLContext setCurrentContext:_context]) {
        NSLog(@"Set Current Context Failed");
        return;
    }
}


//1.设置图层
- (void)setupLayer {
    self.gpuLayer = (CAEAGLLayer *)self.layer;
    [self.gpuLayer setContentsScale:[[UIScreen mainScreen] scale]];
    self.opaque = YES;
    self.gpuLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
}

@end
