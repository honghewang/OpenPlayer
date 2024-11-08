//
//  OPFilterTestVC.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/15.
//

#import "OPFilterTestVC.h"
#import "OPFilterGPUView.h"
#import <Masonry/Masonry.h>
#include <stdio.h>
#include <opencv2/core.hpp>
#include "OPCVUtils.hpp"
#include "OPFilterFacePointsFilter.hpp"
#include "OPFilterFaceMaskFilter.hpp"
#include "OPFilterTextureFilter.hpp"

@interface OPFilterTestVC ()

@property (nonatomic, strong) OPFilterGPUView *gpuView;
@property (nonatomic, strong) OPFilterGPUView *asyngpuView;

@property (nonatomic, strong) UIImageView *imgView;

@property (nonatomic, assign) int index;
@property (nonatomic, strong) UIButton *btn;
@property (nonatomic, assign) int asyncIndex;
@property (nonatomic, strong) UIButton *asyncBtn;

@end

@implementation OPFilterTestVC

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setupUI];
}

- (void)setupUI {
    self.view.backgroundColor = [UIColor whiteColor];
    
    self.gpuView = [[OPFilterGPUView alloc] initWithAsyn:NO];
    [self.view addSubview:self.gpuView];
    CGFloat screenWidth = UIScreen.mainScreen.bounds.size.width;
    [self.gpuView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.mas_equalTo(0);
        make.top.mas_equalTo(200);
        make.height.mas_equalTo(screenWidth * 9 / 16);
    }];
    
    std::shared_ptr<OPFilterRenderFilterLink> maskLink = std::make_shared<OPFilterRenderFilterLink>();
    std::shared_ptr<OPFilterFacePointsFilter> pointsFilter = std::make_shared<OPFilterFacePointsFilter>();
    std::shared_ptr<OPFilterFaceMaskFilter> maskFilter = std::make_shared<OPFilterFaceMaskFilter>();
    std::shared_ptr<OPFilterTextureFilter> grayFilter = std::make_shared<OPFilterTextureFilter>(OPFilterVertexShaderString,OPFilterGrayFragmentShadeString);
    /// 这里可以配置滤镜
    pointsFilter->addTarget(grayFilter);
    grayFilter->addTarget(maskFilter);
    
//    pointsFilter->addTarget(maskFilter);
    
    maskLink->input = pointsFilter;
    maskLink->output = maskFilter;
    [self.gpuView setFilterLinks:maskLink];
    
    self.asyngpuView = [[OPFilterGPUView alloc] initWithAsyn:YES];
    [self.view addSubview:self.asyngpuView];
    [self.asyngpuView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.mas_equalTo(0);
        make.top.mas_equalTo(self.gpuView.mas_bottom);
        make.height.mas_equalTo(screenWidth * 9 / 16);
    }];
    std::shared_ptr<OPFilterRenderFilterLink> pointLink = std::make_shared<OPFilterRenderFilterLink>();
    std::shared_ptr<OPFilterFacePointsFilter> facePointsFilter = std::make_shared<OPFilterFacePointsFilter>();
    pointLink->input = facePointsFilter;
    pointLink->output = facePointsFilter;
    [self.asyngpuView setFilterLinks:pointLink];
    
    UIButton *btn = [[UIButton alloc] init];
    [self.view addSubview:btn];
    btn.backgroundColor = [UIColor redColor];
    [btn setTitle:@"同步AI开始" forState:UIControlStateNormal];
    [btn addTarget:self action:@selector(startProcess) forControlEvents:UIControlEventTouchUpInside];
    [btn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.mas_equalTo(200);
        make.height.mas_equalTo(60);
        make.centerX.mas_equalTo(0);
        make.bottom.mas_equalTo(self.gpuView.mas_top);
    }];
    self.btn = btn;
    
    UIButton *asyncBtn = [[UIButton alloc] init];
    [self.view addSubview:asyncBtn];
    asyncBtn.backgroundColor = [UIColor redColor];
    [asyncBtn setTitle:@"异步AI开始" forState:UIControlStateNormal];
    [asyncBtn addTarget:self action:@selector(asynStartProcess) forControlEvents:UIControlEventTouchUpInside];
    [asyncBtn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.mas_equalTo(200);
        make.height.mas_equalTo(60);
        make.centerX.mas_equalTo(0);
        make.top.mas_equalTo(self.asyngpuView.mas_bottom);
    }];
    self.asyncBtn = asyncBtn;

}

- (void)startProcess {
    NSString *imgName = self.index % 2 == 0 ? @"face" : @"face2";
    self.index++;
    NSString *path = [[NSBundle mainBundle] pathForResource:imgName ofType:@"jpeg"];
    [self.gpuView loadImgMat:OPCVUtils::imgRGBWithFile(path.UTF8String)];
    [self.btn setTitle:[NSString stringWithFormat:@"同步AI开始:%d", self.index] forState:UIControlStateNormal];
}

- (void)asynStartProcess {
    NSString *imgName = self.asyncIndex % 2 == 0 ? @"face" : @"face2";
    self.asyncIndex++;
    NSString *path = [[NSBundle mainBundle] pathForResource:imgName ofType:@"jpeg"];
    [self.asyngpuView loadImgMat:OPCVUtils::imgRGBWithFile(path.UTF8String)];
    [self.asyncBtn setTitle:[NSString stringWithFormat:@"异步AI开始:%d", self.asyncIndex] forState:UIControlStateNormal];
    
}

@end
