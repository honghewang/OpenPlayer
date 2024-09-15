//
//  OPCapturePreview.h
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/9/14.
//

#import <UIKit/UIKit.h>
#include "OPFilterProcess.hpp"

NS_ASSUME_NONNULL_BEGIN

@interface OPCapturePreview : UIView

- (instancetype)initWithAsyn:(BOOL)isAsyn;

- (void)loadImgBuffer:(CVImageBufferRef)imgBuffer;

- (void)setFilterLinks:(std::shared_ptr<OPFilterRenderFilterLink>)link;

@end

NS_ASSUME_NONNULL_END
