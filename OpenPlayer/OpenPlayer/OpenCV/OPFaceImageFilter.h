//
//  OPFaceImageFilter.h
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/20.
//

#import <GPUImage/GPUImage.h>
NS_ASSUME_NONNULL_BEGIN

@interface OPFaceImageFilter : GPUImageTwoPassFilter

@property (nonatomic, strong) NSArray<NSNumber *> *facePoints;

@end

NS_ASSUME_NONNULL_END
