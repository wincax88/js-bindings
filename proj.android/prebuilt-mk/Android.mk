LOCAL_PATH := $(call my-dir)
#==============================================================
include $(CLEAR_VARS)

LOCAL_MODULE := cocos2d_js_static
LOCAL_MODULE_FILENAME := libjscocos2d

LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES := ../../../../../prebuilt/android/$(TARGET_ARCH_ABI)/libjscocos2d.a

LOCAL_CFLAGS := -DHAVE_INSPECTOR

LOCAL_EXPORT_CFLAGS := 




LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../manual \
$(LOCAL_PATH)/../../auto \
$(LOCAL_PATH)/../../../../audio/include

LOCAL_EXPORT_LDLIBS := -lz

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
# LOCAL_WHOLE_STATIC_LIBRARIES += spidermonkey_static
LOCAL_WHOLE_STATIC_LIBRARIES += uv_static
LOCAL_WHOLE_STATIC_LIBRARIES += v8_static

include $(PREBUILT_STATIC_LIBRARY)
#==============================================================
$(call import-module, cocos/prebuilt-mk)
