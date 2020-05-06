MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(MY_LOCAL_PATH)

include $(call all-subdir-makefiles)

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(LOCAL_PATH)/../../GS2D/jni/gs2d/Android.mk

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(LOCAL_PATH)/../../../../vendors/libzip/Android.mk
LOCAL_PATH := $(MY_LOCAL_PATH)
include $(LOCAL_PATH)/../../../../../angelscript/projects/android/jni/Android.mk
LOCAL_PATH := $(MY_LOCAL_PATH)
include $(LOCAL_PATH)/../../../../../box2d/Build/Android.mk

# LOCAL_PATH := $(MY_LOCAL_PATH)
# include $(LOCAL_PATH)/gs2d/Android.mk
