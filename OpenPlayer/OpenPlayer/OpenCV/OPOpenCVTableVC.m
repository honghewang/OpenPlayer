//
//  OPOpenCVTableVC.m
//  OpenPlayer
//
//  Created by 王宏鹤 on 2025/4/2.
//

#import "OPOpenCVTableVC.h"
#import "TableViewCell.h"
#import "OPOpenCVTestVC.h"
#import <Masonry/Masonry.h>
#import "OPCVEffectBaseVC.h"
#import "OPCVEffectROIVC.h"
#import "OPCVEffectCannyVC.h"
#import "OPCVEffectChannelVC.h"
#import "OPCVEffectBlendVC.h"
#import "OPCVEffectThresholdVC.h"
#import "OPCVEffectFilterVC.h"
#import "OPCVEffectErodeVC.h"
#import "OPCVEffectDilateVC.h"
#import "OPCVEffectOpenVC.h"
#import "OPCVEffectCloseVC.h"
#import "OPCVEffectGradientVC.h"
#import "OPCVEffectTophatVC.h"
#import "OPCVEffectBlackhatVC.h"
#import "OPCVEffectSobelVC.h"
#import "OPCVEffectHoughLinesVC.h"
#import "OPCVEffectHighpassVC.h"
#import "OPCVEffectLowpassVC.h"
#import "OPCVEffectEqualizeHistVC.h"
#import "OPCVEffectCLAHEVC.h"
#import "OPCVEffectFeatureMatchVC.h"

@interface OPOpenCVModel : NSObject

@property (nonatomic, strong) NSString *title;

@property (nonatomic, strong) Class cls;

@end

@implementation OPOpenCVModel

@end

@interface OPOpenCVTableVC () <UITableViewDelegate, UITableViewDataSource>

@property (nonatomic, strong) UITableView *tableView;

@end

@implementation OPOpenCVTableVC

- (void)viewDidLoad {
    [super viewDidLoad];
    self.tableView = [[UITableView alloc] initWithFrame:self.view.bounds];
    self.tableView.delegate = self;
    self.tableView.dataSource = self;
    [self.view addSubview:self.tableView];
    [self.tableView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.mas_equalTo(UIEdgeInsetsZero);
    }];
    [self.tableView registerClass:TableViewCell.class forCellReuseIdentifier:NSStringFromClass(TableViewCell.class)];
    
}

#pragma mark - UITableViewDelegate, UITableViewDataSource


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [OPOpenCVTableVC testList].count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    OPOpenCVModel *model = [OPOpenCVTableVC testList][indexPath.item];
    TableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(TableViewCell.class)];
    cell.title = model.title;
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    OPOpenCVModel *model = [OPOpenCVTableVC testList][indexPath.item];
    UIViewController *vc = [[model.cls alloc] init];
    vc.title = model.title;
    [self.navigationController pushViewController:vc animated:YES];
}


+ (NSArray *)testList {
    OPOpenCVModel *model1 = [[OPOpenCVModel alloc] init];
    model1.title = @"OpenCV+GPUImage";
    model1.cls = OPOpenCVTestVC.class;
    
    OPOpenCVModel *model2 = [[OPOpenCVModel alloc] init];
    model2.title = @"OpenCV roi 中间200x200效果";
    model2.cls = OPCVEffectROIVC.class;

    OPOpenCVModel *model3 = [[OPOpenCVModel alloc] init];
    model3.title = @"单通道 （显示成(R,R,R)）";
    model3.cls = OPCVEffectChannelVC.class;
    
    OPOpenCVModel *model4 = [[OPOpenCVModel alloc] init];
    model4.title = @"融合效果";
    model4.cls = OPCVEffectBlendVC.class;
    
    OPOpenCVModel *model5 = [[OPOpenCVModel alloc] init];
    model5.title = @"二值效果";
    model5.cls = OPCVEffectThresholdVC.class;
    
    OPOpenCVModel *model6 = [[OPOpenCVModel alloc] init];
    model6.title = @"滤波";
    model6.cls = OPCVEffectFilterVC.class;
    
    OPOpenCVModel *model7 = [[OPOpenCVModel alloc] init];
    model7.title = @"腐蚀";
    model7.cls = OPCVEffectErodeVC.class;
    
    OPOpenCVModel *model8 = [[OPOpenCVModel alloc] init];
    model8.title = @"膨胀";
    model8.cls = OPCVEffectDilateVC.class;
    
    OPOpenCVModel *model9 = [[OPOpenCVModel alloc] init];
    model9.title = @"开运算（先腐蚀后膨胀）";
    model9.cls = OPCVEffectOpenVC.class;
    
    OPOpenCVModel *model10 = [[OPOpenCVModel alloc] init];
    model10.title = @"闭运算（先膨胀后腐蚀）";
    model10.cls = OPCVEffectCloseVC.class;

    OPOpenCVModel *model11 = [[OPOpenCVModel alloc] init];
    model11.title = @"梯度(膨胀-腐蚀)";
    model11.cls = OPCVEffectGradientVC.class;
    
    OPOpenCVModel *model12 = [[OPOpenCVModel alloc] init];
    model12.title = @"礼帽(原图-开运算)";
    model12.cls = OPCVEffectTophatVC.class;
    
    OPOpenCVModel *model13 = [[OPOpenCVModel alloc] init];
    model13.title = @"黑帽(闭运算-原图)";
    model13.cls = OPCVEffectBlackhatVC.class;
    
    
    OPOpenCVModel *model14 = [[OPOpenCVModel alloc] init];
    model14.title = @"Sobel梯度";
    model14.cls = OPCVEffectSobelVC.class;
    
    OPOpenCVModel *model15 = [[OPOpenCVModel alloc] init];
    model15.title = @"Canny效果";
    model15.cls = OPCVEffectCannyVC.class;
    
    OPOpenCVModel *model16 = [[OPOpenCVModel alloc] init];
    model16.title = @"HoughLines直线检测";
    model16.cls = OPCVEffectHoughLinesVC.class;
    
    OPOpenCVModel *model17 = [[OPOpenCVModel alloc] init];
    model17.title = @"DCT高通";
    model17.cls = OPCVEffectHighpassVC.class;
    
    OPOpenCVModel *model18 = [[OPOpenCVModel alloc] init];
    model18.title = @"DCT低通";
    model18.cls = OPCVEffectLowpassVC.class;
    
    OPOpenCVModel *model19 = [[OPOpenCVModel alloc] init];
    model19.title = @"直方图均衡";
    model19.cls = OPCVEffectEqualizeHistVC.class;
    
    
    OPOpenCVModel *model20 = [[OPOpenCVModel alloc] init];
    model20.title = @"自适应直方图均衡";
    model20.cls = OPCVEffectCLAHEVC.class;
    
    OPOpenCVModel *model21 = [[OPOpenCVModel alloc] init];
    model21.title = @"特征匹配";
    model21.cls = OPCVEffectFeatureMatchVC.class;
    
    
    return @[model1, model2, model3, model4, model5, model6, model7, model8, model9, model10, model11, model12, model13, model14, model15, model16, model17, model18, model19, model20, model21];
}

@end
