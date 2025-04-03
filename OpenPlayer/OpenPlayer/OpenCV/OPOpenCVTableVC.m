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
    
    return @[model1, model2];
}

@end
