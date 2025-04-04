//
//  OPCVEffectBaseVC.h
//  OpenPlayer
//
//  Created by 王宏鹤 on 2025/4/3.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface OPCVEffectBaseVC : UIViewController

// override
//- (cv::Mat)effect:(cv::Mat&)mat;

@property (nonatomic, strong) UIImageView *imageView;

- (NSString *)imgPath;

@end

NS_ASSUME_NONNULL_END
