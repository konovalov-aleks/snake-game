// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from point.djinni

#include "NativePoint.hpp"  // my header
#include "Marshal.hpp"

namespace djinni_generated {

NativePoint::NativePoint() = default;

NativePoint::~NativePoint() = default;

auto NativePoint::fromCpp(JNIEnv* jniEnv, const CppType& c) -> ::djinni::LocalRef<JniType> {
    const auto& data = ::djinni::JniClass<NativePoint>::get();
    auto r = ::djinni::LocalRef<JniType>{jniEnv->NewObject(data.clazz.get(), data.jconstructor,
                                                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c.x)),
                                                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c.y)))};
    ::djinni::jniExceptionCheck(jniEnv);
    return r;
}

auto NativePoint::toCpp(JNIEnv* jniEnv, JniType j) -> CppType {
    ::djinni::JniLocalScope jscope(jniEnv, 3);
    assert(j != nullptr);
    const auto& data = ::djinni::JniClass<NativePoint>::get();
    return {::djinni::I32::toCpp(jniEnv, jniEnv->GetIntField(j, data.field_mX)),
            ::djinni::I32::toCpp(jniEnv, jniEnv->GetIntField(j, data.field_mY))};
}

}  // namespace djinni_generated
