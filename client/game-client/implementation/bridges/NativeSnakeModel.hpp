// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from snake.djinni

#pragma once

#include "djinni_support.hpp"
#include "snake_model.hpp"

namespace djinni_generated {

class NativeSnakeModel final {
public:
    using CppType = ::SnakeModel;
    using JniType = jobject;

    using Boxed = NativeSnakeModel;

    ~NativeSnakeModel();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    NativeSnakeModel();
    friend ::djinni::JniClass<NativeSnakeModel>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("ru/tensor/generated/SnakeModel") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(Ljava/util/ArrayList;Lru/tensor/generated/VectorModel;Lru/tensor/generated/VectorModel;Lru/tensor/generated/ColorModel;)V") };
    const jfieldID field_mPoints { ::djinni::jniGetFieldID(clazz.get(), "mPoints", "Ljava/util/ArrayList;") };
    const jfieldID field_mLeftEye { ::djinni::jniGetFieldID(clazz.get(), "mLeftEye", "Lru/tensor/generated/VectorModel;") };
    const jfieldID field_mRightEye { ::djinni::jniGetFieldID(clazz.get(), "mRightEye", "Lru/tensor/generated/VectorModel;") };
    const jfieldID field_mColor { ::djinni::jniGetFieldID(clazz.get(), "mColor", "Lru/tensor/generated/ColorModel;") };
};

}  // namespace djinni_generated
