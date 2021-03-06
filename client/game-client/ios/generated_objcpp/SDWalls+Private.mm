// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from walls.djinni

#import "SDWalls+Private.h"
#import "DJIMarshal+Private.h"
#include <cassert>

namespace djinni_generated {

auto Walls::toCpp(ObjcType obj) -> CppType
{
    assert(obj);
    return {::djinni::I32::toCpp(obj.leftX),
            ::djinni::I32::toCpp(obj.topY),
            ::djinni::I32::toCpp(obj.rightX),
            ::djinni::I32::toCpp(obj.bottomY)};
}

auto Walls::fromCpp(const CppType& cpp) -> ObjcType
{
    return [[SDWalls alloc] initWithLeftX:(::djinni::I32::fromCpp(cpp.left_x))
                                     topY:(::djinni::I32::fromCpp(cpp.top_y))
                                   rightX:(::djinni::I32::fromCpp(cpp.right_x))
                                  bottomY:(::djinni::I32::fromCpp(cpp.bottom_y))];
}

}  // namespace djinni_generated
