LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -DANDROID=1 -DGLES2

LOCAL_MODULE    := soil
LOCAL_SRC_FILES := \
  image_helper.c \
  stb_image_aug.c  \
  image_DXT.c \
  SOIL.c \

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
