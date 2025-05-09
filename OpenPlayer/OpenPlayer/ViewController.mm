//
//  ViewController.m
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/5/7.
//

#import "ViewController.h"
#import "TableViewCell.h"
#import "OPDecoderTestVC.h"
#import "OPPlayerVC.h"
#import "OPOpenCVTestVC.h"
#import "OPFilterGPUImageTest.h"
#import "OPFilterTestVC.h"
#include "OPCVUtils.hpp"
#import "OPCameraViewController.h"
#import "OPPyTorchTestVC.h"
#import "OPOpenCVTableVC.h"

@interface OPVCModel : NSObject

@property (nonatomic, strong) NSString *title;

@property (nonatomic, strong) Class cls;

@end

@implementation OPVCModel

@end


@interface ViewController () <UITableViewDelegate, UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.tableView registerClass:TableViewCell.class forCellReuseIdentifier:NSStringFromClass(TableViewCell.class)];
    
    NSString *path = [[NSBundle mainBundle] pathForResource:@"shape_predictor_68_face_landmarks" ofType:@"dat"];
    NSString *facePath = [[NSBundle mainBundle] pathForResource:@"mmod_human_face_detector" ofType:@"dat"];
    OPCVUtils::getInstance()->loadModel(path.UTF8String, facePath.UTF8String);
}

#pragma mark - UITableViewDelegate, UITableViewDataSource


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [ViewController testList].count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    OPVCModel *model = [ViewController testList][indexPath.item];
    TableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(TableViewCell.class)];
    cell.title = model.title;
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    OPVCModel *model = [ViewController testList][indexPath.item];
    UIViewController *vc = [[model.cls alloc] init];
    vc.title = model.title;
    [self.navigationController pushViewController:vc animated:YES];
}


+ (NSArray *)testList {
    OPVCModel *model1 = [[OPVCModel alloc] init];
    model1.title = @"解码演示";
    model1.cls = OPDecoderTestVC.class;
    
    OPVCModel *model2 = [[OPVCModel alloc] init];
    model2.title = @"播放器";
    model2.cls = OPPlayerVC.class;
    
    
    OPVCModel *model3 = [[OPVCModel alloc] init];
    model3.title = @"GPUImage测试";
    model3.cls = OPFilterGPUImageTest.class;
    
    OPVCModel *model4 = [[OPVCModel alloc] init];
    model4.title = @"OpenCV测试";
    model4.cls = OPOpenCVTableVC.class;
    
    OPVCModel *model5 = [[OPVCModel alloc] init];
    model5.title = @"滤镜测试：AI同步/异步";
    model5.cls = OPFilterTestVC.class;
    
    OPVCModel *model6 = [[OPVCModel alloc] init];
    model6.title = @"相机";
    model6.cls = OPCameraViewController.class;
    
    OPVCModel *model7 = [[OPVCModel alloc] init];
    model7.title = @"PyTorch模型测试，分割模型";
    model7.cls = OPPyTorchTestVC.class;
    
    return @[
        model1,
        model2,
        model3,
        model4,
        model5,
        model6,
        model7
    ];
}


@end
