//
//  OPPlayerLayer.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/15.
//

#import "OPPlayerLayer.h"
#import <OpenGLES/ES2/gl.h>
#import <UIKit/UIKit.h>
#include <OpenGLES/ES2/glext.h>

static const GLfloat vertices[12] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f, 0.0f
};

static const GLfloat texcoords[8] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f
};

@interface OPPlayerLayer () {
    
    GLuint plane_textures[3];//texture_id
    GLuint av4_position;
    GLuint av2_texcoord;
    GLuint um4_matrix;
    
    GLuint us2_sampler[3];
    
    GLint _renderWidth;
    GLint _renderHeight;
}

@property(nonatomic,strong) EAGLContext *context;

@property(nonatomic,assign) GLuint colorRenderBuffer;
@property(nonatomic,assign) GLuint colorFrameBuffer;

@property(nonatomic,assign) GLuint program;

@property(nonatomic,assign) CVPixelBufferRef pixelBuffer;

@end

@implementation OPPlayerLayer

- (instancetype)init {
    self = [super init];
    if (self) {
        [self setupLayer];
    }
    return self;
}

- (void)dealloc {
    [self deletBuffer];
    if (self.program != 0) {
        glDeleteProgram(self.program);
    }
}

- (void)setupOpenGLContext {
    
    [self setupContext];
    [self deletBuffer];
    [self setupRenderBuffer];
    [self setupFrameBuffer];
}

- (void)refreshWithWidth:(int)width height:(int)height yData:(char *)yData uData:(char *)uData vData:(char *)vData {
    
    [self setupOpenGLContext];
    
    //3.获取顶点着色程序、片元着色器程序文件位置
    NSString* vertFile = [[NSBundle mainBundle] pathForResource:@"shaderv" ofType:@"glsl"];
    NSString* fragFile = [[NSBundle mainBundle] pathForResource:@"shaderf" ofType:@"glsl"];

    //4.判断self.program是否存在，存在则清空其文件
    if (self.program) {

        glDeleteProgram(self.program);
        self.program = 0;
    }

    //5.加载程序到program中来。
    self.program = [self loadShader:vertFile frag:fragFile];

    //6.链接
    glLinkProgram(self.program);
    GLint linkSuccess;

    //7.获取链接状态
    glGetProgramiv(self.program, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetProgramInfoLog(self.program, sizeof(messages), 0, &messages[0]);
        NSString *messageString = [NSString stringWithUTF8String:messages];
        NSLog(@"error%@", messageString);

        return ;
    } else {
        glUseProgram(self.program);
    }
    
    // 获取vertex shader中attribute变量的引用
    av4_position = glGetAttribLocation(self.program, "av4_position");
    av2_texcoord = glGetAttribLocation(self.program, "av2_texcoord");
    // 获取fragment shader中uniform变量的引用
    us2_sampler[0] = glGetUniformLocation(self.program, "samplerY");
    us2_sampler[1] = glGetUniformLocation(self.program, "samplerU");
    us2_sampler[2] = glGetUniformLocation(self.program, "samplerV");
    
    glEnableVertexAttribArray(av4_position);
    glEnableVertexAttribArray(av2_texcoord);
    glVertexAttribPointer(av4_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), vertices);
    glVertexAttribPointer(av2_texcoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), texcoords);
    
    glGenTextures(3, plane_textures);//创建texture对象
    [self createTex:yData width:width height:height index:0 texture:plane_textures[0]];
    glUniform1i(us2_sampler[0], 0);
    [self createTex:uData width:width / 2 height:height / 2 index:1 texture:plane_textures[1]];
    glUniform1i(us2_sampler[1], 1);
    [self createTex:vData width:width / 2 height:height / 2 index:2 texture:plane_textures[2]];
    glUniform1i(us2_sampler[2], 2);
    
    glClearColor(0.0, 0.0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    //2.设置视口
    glViewport(0, 0, _renderWidth, _renderHeight);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindRenderbuffer(GL_RENDERBUFFER, self.colorRenderBuffer);
    [self.context presentRenderbuffer:GL_RENDERBUFFER];
    for (int i = 0; i < 3; ++i) {
        if (plane_textures[i]) {
            glDeleteTextures(1, &plane_textures[i]);
            plane_textures[i] = 0;
        }
    }
}


- (void)createTex:(char *)data width:(int)width height:(int)height index:(int)index texture:(GLuint)texture {
    glActiveTexture(GL_TEXTURE0 + index);//设置当前活动的纹理单元
    glBindTexture(GL_TEXTURE_2D, texture);//texture绑定
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

//- (void)setFrame:(CGRect)frame {
//    [super setFrame:frame];
//    if (frame.size.width > 0 && frame.size.height > 0) {
//        [self setupOpenGLContext];
//    }
//}

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
    [self.context renderbufferStorage:GL_RENDERBUFFER fromDrawable:self];
    
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
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
    EAGLContext *context = [[EAGLContext alloc]initWithAPI:api];
    if (!context) {
        NSLog(@"Create Context Failed");
        return;
    }
    if (![EAGLContext setCurrentContext:context]) {
        NSLog(@"Set Current Context Failed");
        return;
    }
    self.context = context;
    
}


//1.设置图层
- (void)setupLayer {
    [self setContentsScale:[[UIScreen mainScreen] scale]];
    self.opaque = YES;
    self.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
 
}

#pragma mark -- Shader
- (GLuint)loadShader:(NSString *)vert frag:(NSString *)frag {
    //创建2个临时的变量，verShader,fragShader
    GLuint verShader,fragShader;
    //创建一个Program
    GLuint program = glCreateProgram();
    if (program == 0) {
        GLenum error = glGetError();
        // 处理错误
        NSLog(@"error is %d", error);
    }
    
    //编译文件
    //编译顶点着色程序、片元着色器程序
    //参数1：编译完存储的底层地址
    //参数2：编译的类型，GL_VERTEX_SHADER（顶点）、GL_FRAGMENT_SHADER(片元)
    //参数3：文件路径
    [self compileShader:&verShader type:GL_VERTEX_SHADER file:vert];
    [self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:frag];
    
    //创建最终的程序
    glAttachShader(program, verShader);
    glAttachShader(program, fragShader);
    
    //释放不需要的shader
    glDeleteProgram(verShader);
    glDeleteProgram(fragShader);
    
    return program;
    
}

//链接shader
-(void)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file {
     //读取文件路径字符串
    NSString *content = [NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil];
    //获取文件路径字符串，C语言字符串
    const GLchar *source = (GLchar *)[content UTF8String];
    
    //创建一个shader（根据type类型）
    *shader = glCreateShader(type);
    
    //将顶点着色器源码附加到着色器对象上。
    //参数1：shader,要编译的着色器对象 *shader
    //参数2：numOfStrings,传递的源码字符串数量 1个
    //参数3：strings,着色器程序的源码（真正的着色器程序源码）
    //参数4：lenOfStrings,长度，具有每个字符串长度的数组，或NULL，这意味着字符串是NULL终止的
    glShaderSource(*shader, 1, &source, NULL);
    
    //把着色器源代码编译成目标代码
    glCompileShader(*shader);
    
}

@end
