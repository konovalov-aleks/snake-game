LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

MODULE_SOURCES :=   implementation/vector2d.cpp \
                    implementation/game_room.cpp \
                    implementation/bonus.cpp \
                    implementation/snake.cpp \
                    implementation/game_state.cpp

LOCAL_MODULE    := game-engine
LOCAL_SRC_FILES := $(MODULE_SOURCES)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/implementation
LOCAL_STATIC_LIBRARIES := sbis-lib300

include $(BUILD_STATIC_LIBRARY)

$(call import-add-path,$(SBISPlatformSDK_374220)/bin/android/any)
$(call import-module,sbis)
$(call import-module,boost/1.59.0)
