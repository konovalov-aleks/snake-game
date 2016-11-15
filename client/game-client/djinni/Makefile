DJINNI := $(DJINNI_PATH)/src/run
BUILD_DIR := $(shell dirname $(shell pwd -L))

COMMON_FLAGS := \
	--ident-jni-file NativeFooBar \
	--ident-java-field mFooBar \
	--ident-jni-class NativeFooBar \
	--ident-cpp-method FooBar \
	--cpp-use-wide-strings true \
	--cpp-out $(BUILD_DIR)/implementation/generated/ \
	--cpp-optional-header '<boost/optional.hpp>' \
	--cpp-optional-template 'boost::optional' \
	--idl all.djinni \
	--list-in-files ./all.djinni.inlist

ANDROID_FLAGS := \
	--jni-out $(BUILD_DIR)/implementation/bridges/ \
	--java-out $(BUILD_DIR)/../platform/android/Game/app/src/main/java/ru/tensor/generated/ \
	--java-package "ru.tensor.generated"

IOS_FLAGS := \
	--objc-out $(BUILD_DIR)/ios/generated_objc/ \
	--objcpp-out $(BUILD_DIR)/ios/generated_objcpp/ \
	--objc-type-prefix SD

IOS_TARGET_NAME := ios.djinni.out
ANDROID_TARGET_NAME := android.djinni.out

MAKE_DEPFILE := python make_depfile.py all.djinni.inlist all.djinni.d

all: android ios

android: android.djinni.out

ios: ios.djinni.out

$(ANDROID_TARGET_NAME): all.djinni
	$(DJINNI) $(COMMON_FLAGS) $(ANDROID_FLAGS)
	$(MAKE_DEPFILE) $(ANDROID_TARGET_NAME)
	touch $(ANDROID_TARGET_NAME)

$(IOS_TARGET_NAME): all.djinni
	$(DJINNI) $(COMMON_FLAGS) $(IOS_FLAGS)
	$(MAKE_DEPFILE) $(IOS_TARGET_NAME)
	touch $(IOS_TARGET_NAME)

-include all.djinni.d
