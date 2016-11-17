// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from snake.djinni

#import "SDVectorModel.h"
#import <Foundation/Foundation.h>

@interface SDSnakeModel : NSObject
- (nonnull instancetype)initWithPoints:(nonnull NSArray<SDVectorModel *> *)points
                               leftEye:(nonnull SDVectorModel *)leftEye
                              rightEye:(nonnull SDVectorModel *)rightEye;
+ (nonnull instancetype)snakeModelWithPoints:(nonnull NSArray<SDVectorModel *> *)points
                                     leftEye:(nonnull SDVectorModel *)leftEye
                                    rightEye:(nonnull SDVectorModel *)rightEye;

@property (nonatomic, readonly, nonnull) NSArray<SDVectorModel *> * points;

@property (nonatomic, readonly, nonnull) SDVectorModel * leftEye;

@property (nonatomic, readonly, nonnull) SDVectorModel * rightEye;

@end