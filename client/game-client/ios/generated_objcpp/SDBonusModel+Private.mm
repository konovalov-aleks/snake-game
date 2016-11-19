// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from bonus.djinni

#import "SDBonusModel+Private.h"
#import "SDColorModel+Private.h"
#import "SDVectorModel+Private.h"
#include <cassert>

namespace djinni_generated {

auto BonusModel::toCpp(ObjcType obj) -> CppType
{
    assert(obj);
    return {::djinni_generated::VectorModel::toCpp(obj.position),
            ::djinni_generated::ColorModel::toCpp(obj.color)};
}

auto BonusModel::fromCpp(const CppType& cpp) -> ObjcType
{
    return [[SDBonusModel alloc] initWithPosition:(::djinni_generated::VectorModel::fromCpp(cpp.position))
                                            color:(::djinni_generated::ColorModel::fromCpp(cpp.color))];
}

}  // namespace djinni_generated
