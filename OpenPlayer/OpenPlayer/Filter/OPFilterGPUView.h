//
//  OPFilterGPUView.h
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/17.
//

#import <UIKit/UIKit.h>
#include "OPFilterProcess.hpp"

NS_ASSUME_NONNULL_BEGIN

@interface OPFilterGPUView : UIView

- (instancetype)initWithAsyn:(BOOL)isAsyn;

- (void)loadImgMat:(cv::Mat)mat;

- (void)setFilterLinks:(std::shared_ptr<OPFilterRenderFilterLink>)link;

@end

NS_ASSUME_NONNULL_END
