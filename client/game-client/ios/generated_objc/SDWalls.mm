// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from walls.djinni

#import "SDWalls.h"


@implementation SDWalls

- (nonnull instancetype)initWithLeftX:(int32_t)leftX
                                 topY:(int32_t)topY
                               rightX:(int32_t)rightX
                              bottomY:(int32_t)bottomY
{
    if (self = [super init]) {
        _leftX = leftX;
        _topY = topY;
        _rightX = rightX;
        _bottomY = bottomY;
    }
    return self;
}

+ (nonnull instancetype)wallsWithLeftX:(int32_t)leftX
                                  topY:(int32_t)topY
                                rightX:(int32_t)rightX
                               bottomY:(int32_t)bottomY
{
    return [[self alloc] initWithLeftX:leftX
                                  topY:topY
                                rightX:rightX
                               bottomY:bottomY];
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@ %p leftX:%@ topY:%@ rightX:%@ bottomY:%@>", self.class, (void *)self, @(self.leftX), @(self.topY), @(self.rightX), @(self.bottomY)];
}

@end
