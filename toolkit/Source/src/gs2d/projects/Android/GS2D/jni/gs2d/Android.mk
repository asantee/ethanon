LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

GS2D_SOURCE_RELATIVE_PATH = ../../../../../src
GS2D_SOURCE_PATH = $(LOCAL_PATH)$(GS2D_SOURCE_RELATIVE_PATH)
# VENDORS_PATH = $(LOCAL_PATH)../../../../../../vendors
VENDORS_PATH = $(LOCAL_PATH)/../../../../../vendors
SOURCE_PATH = $(LOCAL_PATH)/../../../../../..

LOCAL_C_INCLUDES = \
	$(VENDORS_PATH)/BoostSDK/ \
	$(VENDORS_PATH)/libzip/ \
	$(SOURCE_PATH)/soil/

LOCAL_MODULE    := gs2d

LOCAL_STATIC_LIBRARIES := libzip libsoil

LOCAL_CFLAGS    := -Werror -DANDROID=1 -DGS2D_WARN_SLOW_DRAWING=1 -DGLES2=1

LOCAL_DEFAULT_CPP_EXTENSION := cpp 

LOCAL_SRC_FILES := \
	$(GS2D_SOURCE_RELATIVE_PATH)/gs2d.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/BitmapFont.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/GLES2/gs2dGLES2.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/GLES2/gs2dGLES2Shader.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/GLES2/gs2dGLES2Sprite.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/GLES2/gs2dGLES2Texture.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/GLES2/GLES2RectRenderer.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/GLES2/GLES2UniformParameter.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Input/Android/gs2dAndroidInput.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Input/gs2dMobileInput.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Audio/Android/gs2dAndroidAudio.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/Platform.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/Logger.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/FileLogger.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/FileIOHub.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/android/Platform.android.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/android/AndroidFileIOHub.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/ZipFileManager.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/NativeCommandForwarder.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/NativeCommandAssembler.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Enml/Enml.cpp

LOCAL_LDLIBS := -ldl -lz -llog -lGLESv2

# include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)