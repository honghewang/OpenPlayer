//
//  OPTorchModule.h
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/11/18.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface OPTorchModule : NSObject

- (nullable instancetype)initWithFileAtPath:(NSString*)filePath;
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (NSMutableData *)segmentImage:(void *)imageBuffer withWidth:(int)width withHeight:(int)height;
- (UIImage *)segmentImage:(UIImage *)img;
@end

NS_ASSUME_NONNULL_END
