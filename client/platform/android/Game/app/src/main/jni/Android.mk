LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := snake-game
LOCAL_WHOLE_STATIC_LIBRARIES := game-engine game-client
LOCAL_STATIC_LIBRARIES := sbis-lib300 sbis-rpc300 sbis-bl-core300 sbis-communication300

include $(BUILD_SHARED_LIBRARY)

$(call import-add-path,$(SBISPlatformSDK_374220)/bin/android/any)
$(call import-add-path,$(LOCAL_PATH)/../../../../../../../)
$(call import-add-path,$(LOCAL_PATH)/../../../../../../../../)
$(call import-module,game-client)
$(call import-module,engine)
$(call import-module,sbis)
$(call import-module,boost/1.59.0)
