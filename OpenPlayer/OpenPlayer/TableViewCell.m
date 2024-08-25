//
//  TableViewCell.m
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/5/7.
//

#import "TableViewCell.h"
#import <Masonry/Masonry.h>

@interface TableViewCell ()

@property (nonatomic, strong) UILabel *label;

@end

@implementation TableViewCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        [self setupUI];
    }
    return self;
}

#pragma mark - setupUI

- (void)setupUI {
    self.label = [[UILabel alloc] init];
    self.label.textAlignment = NSTextAlignmentLeft;
    [self addSubview:self.label];
    
    [self.label mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.mas_equalTo(16);
        make.right.mas_equalTo(-16);
        make.center.mas_equalTo(CGPointZero);
    }];
}


#pragma mark - title

- (void)setTitle:(NSString *)title {
    self.label.text = title;
}

- (NSString *)title {
    return self.label.text;
}

@end
