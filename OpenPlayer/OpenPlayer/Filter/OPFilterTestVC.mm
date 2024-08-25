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

@interface OPFilterTestVC ()

@property (nonatomic, strong) OPFilterGPUView *gpuView;
@property (nonatomic, strong) OPFilterGPUView *asyngpuView;

@property (nonatomic, strong) UIImageView *imgView;

@property (nonatomic, assign) int index;

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
    std::shared_ptr<OPFilterFaceMaskFilter> maskFilter = std::make_shared<OPFilterFaceMaskFilter>();
    maskLink->input = maskFilter;
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
    
    UIButton *bnt = [[UIButton alloc] init];
    [self.view addSubview:bnt];
    bnt.backgroundColor = [UIColor redColor];
    [bnt setTitle:@"开始" forState:UIControlStateNormal];
    [bnt addTarget:self action:@selector(startProcess) forControlEvents:UIControlEventTouchUpInside];
    [bnt mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.mas_equalTo(80);
        make.height.mas_equalTo(60);
        make.centerX.mas_equalTo(0);
        make.bottom.mas_equalTo(self.gpuView.mas_top);
    }];
}

- (void)startProcess {
    NSString *imgName = self.index % 2 == 0 ? @"face" : @"face2";
    self.index++;
    NSString *path = [[NSBundle mainBundle] pathForResource:imgName ofType:@"jpeg"];
    [self.gpuView loadImgMat:OPCVUtils::imgWithFile(path.UTF8String)];
    [self.asyngpuView loadImgMat:OPCVUtils::imgWithFile(path.UTF8String)];
}

@end
