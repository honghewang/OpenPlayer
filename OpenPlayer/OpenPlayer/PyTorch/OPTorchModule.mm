//
//  OPTorchModule.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/11/18.
//

#import "OPTorchModule.h"
#import <LibTorch/LibTorch.h>

@interface OPTorchModule() {
    torch::jit::script::Module _impl;
}

@end

@implementation OPTorchModule

- (nullable instancetype)initWithFileAtPath:(NSString*)filePath {
    self = [super init];
    if (self) {
        try {
            _impl = torch::jit::load(filePath.UTF8String);
            _impl.eval();
        } catch (const std::exception& exception) {
            NSLog(@"%s", exception.what());
            return nil;
        }
    }
    return self;
}

- (UIImage *)segmentImage:(UIImage *)img {
    if (img == nil) {
        return nil;
    }
    // 设置缩放后的尺寸
    int width = 250;
    int height = 250;
    
    NSData *imgData = [self rgbaDataFromUIImage:img size:CGSizeMake(width, height)];
    // 创建一个用于存储RGB数据的vector
    int size = width * height;
    NSMutableData *inputData = [NSMutableData dataWithLength:sizeof(float) * 3 * size];
    float *imgBuffer = (float *)[inputData mutableBytes];

    // 遍历每个像素，将BGR转换为RGB
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint8_t buffer[4];
            NSRange range = NSMakeRange((y * width + x) * 4, 4);
            [imgData getBytes:buffer range:range];
            // 注意此处的大小端，下面使用的小端存储
            imgBuffer[x + y * width] = (float(buffer[0]) / 255.0  - 0.485) / 0.229;
            imgBuffer[x + y * width + size] = (float(buffer[1]) / 255.0  - 0.456) / 0.224;
            imgBuffer[x + y * width + size * 2] = (float(buffer[2]) / 255.0  - 0.406) / 0.225;

        }
    }
    NSMutableData *outputData = [self segmentImage:imgBuffer withWidth:width withHeight:height];
    
    return [self imageFromRGBAData:outputData width:width height:height];
}

- (NSMutableData *)segmentImage:(void *)imageBuffer withWidth:(int)width withHeight:(int)height {
    try {
        // see http://host.robots.ox.ac.uk:8080/pascal/VOC/voc2007/segexamples/index.html for the list of classes with indexes
        const int CLASSNUM = 21;
        const int DOG = 12;
        const int PERSON = 15;
        const int SHEEP = 17;

        at::Tensor tensor = torch::from_blob(imageBuffer, { 1, 3, width, height }, at::kFloat);

        float* floatInput = tensor.data_ptr<float>();
        if (!floatInput) {
            return nil;
        }

        c10::InferenceMode guard;
        
        auto outputDict = _impl.forward({ tensor }).toGenericDict();
        
        auto outputTensor = outputDict.at("out").toTensor();

        float* floatBuffer = outputTensor.data_ptr<float>();
        if (!floatBuffer) {
            return nil;
        }
        NSMutableArray* results = [[NSMutableArray alloc] init];
        for (int i = 0; i < CLASSNUM * width * height; i++) {
            [results addObject:@(floatBuffer[i])];
        }

        NSMutableData *data = [NSMutableData dataWithLength:sizeof(unsigned char) * 4 * width * height];
        unsigned char *buffer = (unsigned char *)[data mutableBytes];
        // go through each element in the output of size [WIDTH, HEIGHT] and
        // set different color for different classnum
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < height; k++) {
                int maxi = 0, maxj = 0, maxk = 0;
                float maxnum = -100000.0;
                for (int i = 0; i < CLASSNUM; i++) {
                    if ([results[i * (width * height) + j * width + k] floatValue] > maxnum) {
                        maxnum = [results[i * (width * height) + j * width + k] floatValue];
                        maxi = i; maxj = j; maxk = k;
                    }
                }

                int n = 4 * (maxj * width + maxk);
                // color coding for person (red), dog (green), sheep (blue)
                // black color for background and other classes
                buffer[n] = 0; buffer[n+1] = 0; buffer[n+2] = 0; buffer[n+3] = 255;
                if (maxi == PERSON) {
                    buffer[n] = 255;
                } else if (maxi == DOG) {
                    buffer[n+1] = 255;
                } else if (maxi == SHEEP) {
                    buffer[n+2] = 255;
                }
            }
        }

        return data;
    } catch (const std::exception& exception) {
        NSLog(@"%s", exception.what());
    }
    return nil;
}


- (NSData *)rgbaDataFromUIImage:(UIImage *)image size:(CGSize)size {
    // 创建一个位图上下文
    CGContextRef context = CGBitmapContextCreate(NULL, (size_t)size.width, (size_t)size.height, 8, (size_t)size.width * 4, CGColorSpaceCreateDeviceRGB(), kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast);
    
    // 检查上下文是否成功创建
    if (!context) {
        NSLog(@"Failed to create bitmap context");
        return nil; // 返回nil表示失败
    }
    
    // 清除上下文（设置为透明）
    CGContextClearRect(context, CGRectMake(0, 0, size.width, size.height));
    
    // 将图像绘制到上下文中
    CGContextDrawImage(context, CGRectMake(0, 0, size.width, size.height), image.CGImage);
    
    // 从上下文中获取图像数据
    void *data = CGBitmapContextGetData(context);
    if (!data) {
        NSLog(@"Failed to get data from bitmap context");
        CGContextRelease(context);
        return nil;
    }
    
    // 创建一个NSData对象来存储图像数据
    NSData *imageData = [NSData dataWithBytes:data length:size.width * size.height * 4];
    
    // 释放上下文
    CGContextRelease(context);
    
    return imageData;
}


- (UIImage *)imageFromRGBAData:(NSData *)rgbaData width:(NSInteger)width height:(NSInteger)height {
    // 假设rgbaData包含了紧密打包的RGBA像素数据，每像素4字节，没有额外的行填充
    size_t bytesPerRow = width * 4;
    size_t dataSize = bytesPerRow * height;
    
    // 分配内存来存储RGBA数据
    char *dataPointer = (char *)malloc(dataSize);
    if (!dataPointer) {
        return nil; // 内存分配失败
    }
    
    // 将NSData中的数据复制到新分配的内存中
    [rgbaData getBytes:dataPointer length:dataSize];
    
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast;
    // 创建CGColorSpace（设备RGB空间）
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    // 创建CGBitmapContext来绘制图像
    CGContextRef context = CGBitmapContextCreate(dataPointer, width, height, 8, bytesPerRow, colorSpace, bitmapInfo);
    if (!context) {
        free(dataPointer); // 如果上下文创建失败，释放分配的内存
        CGColorSpaceRelease(colorSpace); // 释放颜色空间
        return nil;
    }
    // 从上下文中创建CGImage
    CGImageRef cgImage = CGBitmapContextCreateImage(context);
    // 释放资源
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    free(dataPointer); // 释放之前分配的内存
    // 从CGImage创建UIImage
    UIImage *image = [UIImage imageWithCGImage:cgImage];
    // 释放CGImage
    CGImageRelease(cgImage);
    return image;
}

- (nullable NSArray<NSNumber *> *)normalizedWithImg:(UIImage *)img {
    CGImageRef cgImage = img.CGImage;
    if (!cgImage) {
        return nil;
    }
    
    size_t w = CGImageGetWidth(cgImage);
    size_t h = CGImageGetHeight(cgImage);
    size_t bytesPerPixel = 4;
    size_t bytesPerRow = bytesPerPixel * w;
    size_t totalBytes = bytesPerRow * h;
    
    // Allocate memory for raw bytes
    uint8_t *rawBytes = (uint8_t *)malloc(totalBytes);
    if (!rawBytes) {
        return nil; // Memory allocation failed
    }
    
    // Create a bitmap context
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast;
    CGContextRef context = CGBitmapContextCreate(rawBytes, w, h, 8, bytesPerRow, colorSpace, bitmapInfo);
    CGColorSpaceRelease(colorSpace);
    
    if (!context) {
        free(rawBytes); // Free allocated memory if context creation failed
        return nil;
    }
    
    // Draw the image into the context
    CGRect rect = CGRectMake(0, 0, w, h);
    CGContextDrawImage(context, rect, cgImage);
    
    // Normalize the pixel buffer
    NSMutableArray<NSNumber *> *normalizedBuffer = [NSMutableArray arrayWithCapacity:w * h * 3];
    for (size_t y = 0; y < h; y++) {
        for (size_t x = 0; x < w; x++) {
            size_t index = y * bytesPerRow + x * bytesPerPixel;
            float red = (rawBytes[index + 0] / 255.0f - 0.485f) / 0.229f;
            float green = (rawBytes[index + 1] / 255.0f - 0.456f) / 0.224f;
            float blue = (rawBytes[index + 2] / 255.0f - 0.406f) / 0.225f;
            
            [normalizedBuffer addObject:@(red)];
            [normalizedBuffer addObject:@(green)];
            [normalizedBuffer addObject:@(blue)];
        }
    }
    
    // Clean up
    CGContextRelease(context);
    free(rawBytes);
    
    return normalizedBuffer;
}

@end
