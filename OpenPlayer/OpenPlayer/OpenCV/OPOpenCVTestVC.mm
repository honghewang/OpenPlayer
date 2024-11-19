//
//  OPOpenCVTestVC.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/7.
//

#import "OPOpenCVTestVC.h"
#import <Masonry/Masonry.h>
#include "OPCVUtils.hpp"
#import <GPUImage/GPUImage.h>
#import "OPFaceImageFilter.h"

@interface OPOpenCVTestVC ()

@property (nonatomic, strong) UIImageView *imgView;
@property (nonatomic, strong) UIActivityIndicatorView *loadingView;

@property (nonatomic, strong) GPUImagePicture *gpuImg;
@property (nonatomic, strong) GPUImageView *processView;

@end

@implementation OPOpenCVTestVC

- (instancetype)init {
    if (self = [super init]) {
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setupUI];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    [self.gpuImg processImageWithCompletionHandler:^{
        NSLog(@"++++++++++++");
    }];
}

- (void)setupUI {
    self.view.backgroundColor = [UIColor whiteColor];
    
    NSString *path = [[NSBundle mainBundle] pathForResource:@"face" ofType:@"jpeg"];
    UIImage *img = [UIImage imageWithContentsOfFile:path];
    
    UIImageView *imgView = [[UIImageView alloc] init];
    [self.view addSubview:imgView];
    imgView.image = img;
    imgView.contentMode = UIViewContentModeScaleAspectFill;
    imgView.clipsToBounds = YES;
    self.imgView = imgView;
    
    CGFloat width = MIN([UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
    CGFloat height = width * 9 / 16;
    [imgView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.mas_equalTo(0);
        make.height.mas_equalTo(height);
        make.bottom.mas_equalTo(self.view.mas_centerY);
    }];
    
    GPUImageView *processView = [[GPUImageView alloc] init];
    [self.view addSubview:processView];
    processView.contentMode = UIViewContentModeScaleAspectFill;
    processView.clipsToBounds = YES;
    self.processView = processView;
    [processView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.mas_equalTo(0);
        make.height.mas_equalTo(height);
        make.top.mas_equalTo(self.view.mas_centerY);
    }];
    UIImage *image = [UIImage imageNamed:@"face.jpeg"];
    self.gpuImg = [[GPUImagePicture alloc] initWithImage:image];
    [self.gpuImg addTarget:self.processView];
//    [self.gpuImg processImageWithCompletionHandler:^{
//        NSLog(@"++++++++++++");
//    }];
    
    self.loadingView = [[UIActivityIndicatorView alloc] init];
    [self.view addSubview:self.loadingView];
    self.loadingView.hidesWhenStopped = YES;
    [self.loadingView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.center.mas_equalTo(CGPointZero);
    }];
    
    UIButton *btn = [[UIButton alloc] init];
    btn.backgroundColor = [UIColor grayColor];
    [btn setTitle:@"人脸点" forState:UIControlStateNormal];
    [self.view addSubview:btn];
    [btn addTarget:self action:@selector(detectorFaces) forControlEvents:UIControlEventTouchUpInside];
    [btn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.centerX.mas_equalTo(0);
        make.width.mas_equalTo(80);
        make.height.mas_equalTo(40);
        make.bottom.mas_equalTo(-40);
    }];
}

- (void)detectorFaces {
    [self.loadingView startAnimating];
    __weak OPOpenCVTestVC *weakSelf = self;
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        OPOpenCVTestVC *strongSelf = weakSelf;
        NSString *path = [[NSBundle mainBundle] pathForResource:@"face" ofType:@"jpeg"];
        auto img = OPCVUtils::imgRGBWithFile(path.UTF8String);
        std::vector<float> facePoint;
        OPCVUtils::getInstance()->detectorImgTest(img, facePoint);
        NSMutableArray *faceList = [NSMutableArray array];
        for (auto item: facePoint) {
            [faceList addObject:@(item)];
        }
        
        [strongSelf processFacePoints:faceList];
        
        UIImage *faceImg = [strongSelf UIImageFromCVMat:img];
        dispatch_async(dispatch_get_main_queue(), ^{
            strongSelf.imgView.image = faceImg;
            [strongSelf.loadingView stopAnimating];
        });
    });
}

- (void)processFacePoints:(NSArray<NSNumber *> *)facePoints {
    
    NSString *path = [[NSBundle mainBundle] pathForResource:@"face" ofType:@"jpeg"];
    UIImage *img = [UIImage imageWithContentsOfFile:path];
    self.gpuImg = [[GPUImagePicture alloc] initWithImage:img];

    OPFaceImageFilter *filter = [[OPFaceImageFilter alloc] init];
    filter.facePoints = facePoints;
    [self.gpuImg addTarget:filter];
    [filter addTarget:self.processView];
    [self.gpuImg processImage];
}

#pragma mark - mat-image

- (cv::Mat)cvMatFromUIImage:(UIImage *)image {
    CGColorSpaceRef colorSpace = CGImageGetColorSpace(image.CGImage);
    CGFloat cols = image.size.width;
    CGFloat rows = image.size.height;

    cv::Mat cvMat(rows, cols, CV_8UC4); // 8 bits per component, 4 channels (color channels + alpha)

    CGContextRef contextRef = CGBitmapContextCreate(cvMat.data,                 // Pointer to  data
                                                   cols,                       // Width of bitmap
                                                   rows,                       // Height of bitmap
                                                   8,                          // Bits per component
                                                   cvMat.step[0],              // Bytes per row
                                                   colorSpace,                 // Colorspace
                                                   kCGImageAlphaNoneSkipLast |
                                                   kCGBitmapByteOrderDefault); // Bitmap info flags

    CGContextDrawImage(contextRef, CGRectMake(0, 0, cols, rows), image.CGImage);
    CGContextRelease(contextRef);
    return cvMat;
}

- (UIImage *)UIImageFromCVMat:(cv::Mat)cvMat {
    NSData *data = [NSData dataWithBytes:cvMat.data length:cvMat.elemSize()*cvMat.total()];
    CGColorSpaceRef colorSpace;

    if (cvMat.elemSize() == 1) {
        colorSpace = CGColorSpaceCreateDeviceGray();
    } else {
        colorSpace = CGColorSpaceCreateDeviceRGB();
    }

    CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)data);

    // Creating CGImage from cv::Mat
    CGImageRef imageRef = CGImageCreate(cvMat.cols,                                 //width
                                       cvMat.rows,                                 //height
                                       8,                                          //bits per component
                                       8 * cvMat.elemSize(),                       //bits per pixel
                                       cvMat.step[0],                            //bytesPerRow
                                       colorSpace,                                 //colorspace
                                       kCGImageAlphaNone|kCGBitmapByteOrderDefault,// bitmap info
                                       provider,                                   //CGDataProviderRef
                                       NULL,                                       //decode
                                       false,                                      //should interpolate
                                       kCGRenderingIntentDefault                   //intent
                                       );

    // Getting UIImage from CGImage
    UIImage *finalImage = [UIImage imageWithCGImage:imageRef];
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);

    return finalImage;
 }


@end
