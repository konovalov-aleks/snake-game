LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

MODULE_SOURCES :=   implementation/client_game_room.cpp \
                    implementation/game.cpp

LOCAL_MODULE  := game-client
BRIDGES_FILES := $(wildcard $(LOCAL_PATH)/implementation/bridges/*.cpp)
LOCAL_SRC_FILES := $(MODULE_SOURCES) $(BRIDGES_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/implementation $(LOCAL_PATH)/implementation/generated $(LOCAL_PATH)/../../engine $(DJINNI_PATH)/support-lib/jni
LOCAL_STATIC_LIBRARIES := sbis-lib300

include $(BUILD_STATIC_LIBRARY)

$(call import-add-path,$(SBISPlatformSDK_374220)/bin/android/any)
$(call import-module,sbis)
$(call import-module,boost/1.59.0)
