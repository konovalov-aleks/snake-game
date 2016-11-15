// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from field.djinni

#pragma once

#include "djinni_support.hpp"
#include "field.hpp"

namespace djinni_generated {

class NativeField final {
public:
    using CppType = ::Field;
    using JniType = jobject;

    using Boxed = NativeField;

    ~NativeField();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    NativeField();
    friend ::djinni::JniClass<NativeField>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("ru/tensor/generated/Field") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(Lru/tensor/generated/Walls;Ljava/util/ArrayList;Lru/tensor/generated/SnakeModel;)V") };
    const jfieldID field_mWalls { ::djinni::jniGetFieldID(clazz.get(), "mWalls", "Lru/tensor/generated/Walls;") };
    const jfieldID field_mSnakes { ::djinni::jniGetFieldID(clazz.get(), "mSnakes", "Ljava/util/ArrayList;") };
    const jfieldID field_mMySnake { ::djinni::jniGetFieldID(clazz.get(), "mMySnake", "Lru/tensor/generated/SnakeModel;") };
};

}  // namespace djinni_generated
