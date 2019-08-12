LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

GS2D_SOURCE_RELATIVE_PATH = ../../../../../src
GS2D_SOURCE_PATH = $(LOCAL_PATH)$(GS2D_SOURCE_RELATIVE_PATH)
# VENDORS_PATH = $(LOCAL_PATH)../../../../../../vendors
VENDORS_PATH = $(LOCAL_PATH)/../../../../../vendors
SOURCE_PATH = $(LOCAL_PATH)/../../../../../..

LOCAL_C_INCLUDES = \
	$(VENDORS_PATH)/BoostSDK/ \
	$(VENDORS_PATH)/libzip/

LOCAL_MODULE    := gs2d

LOCAL_STATIC_LIBRARIES := libzip

LOCAL_CFLAGS    := -Werror -DANDROID=1 -DGS2D_WARN_SLOW_DRAWING=1 -DGLES2=1

LOCAL_DEFAULT_CPP_EXTENSION := cpp 

LOCAL_SRC_FILES := \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/PolygonRenderer.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Shader.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Sprite.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/SpriteRects.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Application.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Math/GameMath.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Math/Vector2.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Math/Vector2i.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Math/Vector3.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Math/Vector4.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Math/Matrix4x4.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Math/Rect2D.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Math/Rect2Di.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Math/Randomizer.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Math/Color.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Math/OrientedBoundingBox.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/BitmapFont.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/BitmapFontManager.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/GLES2/GLES2Video.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/GLES2/GLES2Shader.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/GLES2/GLES2Texture.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/GLES2/GLES2PolygonRenderer.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Video/GLES2/android/AndroidGLES2Video.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Input/Android/AndroidInput.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Input/MobileInput.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Input/KeyStateManager.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Audio/Android/AndroidAudio.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/Platform.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/Logger.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/FileLogger.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/FileIOHub.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/android/Platform.android.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/android/AndroidFileIOHub.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/FileManager.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/ZipFileManager.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/NativeCommandForwarder.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/NativeCommandAssembler.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/SharedData/SharedDataManager.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/SharedData/SharedData.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Enml/Enml.cpp

# include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)