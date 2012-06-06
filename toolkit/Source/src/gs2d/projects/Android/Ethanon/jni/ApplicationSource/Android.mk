LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

VENDORS_PATH = $(LOCAL_PATH)/../../../../../vendors
SOURCE_PATH  = $(LOCAL_PATH)/../../../../../..
GS2D_SOURCE_RELATIVE_PATH = ../../../../../src
ENGINE_PATH = ../../../../../../engine
ADDONS_PATH = ../../../../../../addons
ENGINE_VENDORS_PATH = ../../../../../../vendors

LOCAL_C_INCLUDES = \
	$(VENDORS_PATH)/BoostSDK/ \
	$(VENDORS_PATH)/libzip/ \
	$(SOURCE_PATH)/soil/ \
	$(SOURCE_PATH)/gs2d/src/ \
	$(SOURCE_PATH)/box2d/

LOCAL_MODULE    := Application

LOCAL_STATIC_LIBRARIES := libzip libpng libsoil libgs2d libangelscript libbox2d

LOCAL_CFLAGS    := -Werror -DANDROID=1 -DGLES2=1 -DAS_MAX_PORTABILITY -DAS_NO_THREADS

LOCAL_DEFAULT_CPP_EXTENSION := cpp 

LOCAL_SRC_FILES := \
	../../../../../src/Platform/android/main.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/Platform.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/android/Platform.android.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/Platform/android/ZipFileManager.cpp \
	$(GS2D_SOURCE_RELATIVE_PATH)/../../soil/SOIL.c \
	$(ENGINE_VENDORS_PATH)/tinyxml_ansi/tinyxml.cpp \
	$(ENGINE_VENDORS_PATH)/tinyxml_ansi/tinystr.cpp \
	$(ENGINE_VENDORS_PATH)/tinyxml_ansi/tinyxmlerror.cpp \
	$(ENGINE_VENDORS_PATH)/tinyxml_ansi/tinyxmlparser.cpp \
	$(ADDONS_PATH)/ansi/scriptstdstring.cpp \
	$(ADDONS_PATH)/ansi/scriptbuilder.cpp \
	$(ADDONS_PATH)/ansi/scriptfile.cpp \
	$(ADDONS_PATH)/scriptarray.cpp \
	$(ADDONS_PATH)/scriptmath.cpp \
	$(ADDONS_PATH)/scriptmath2d.cpp \
	$(ADDONS_PATH)/scriptmath3d.cpp \
	$(ADDONS_PATH)/scriptdictionary.cpp \
	$(ENGINE_PATH)/ETHCommon.cpp \
	$(ENGINE_PATH)/ETHParticleManager.cpp \
	$(ENGINE_PATH)/ETHPrimitiveDrawer.cpp \
	$(ENGINE_PATH)/ETHEngine.cpp \
	$(ENGINE_PATH)/Resource/ETHDirectories.cpp \
	$(ENGINE_PATH)/Resource/ETHResourceManager.cpp \
	$(ENGINE_PATH)/Resource/ETHResourceProvider.cpp \
	$(ENGINE_PATH)/Resource/ETHSpriteDensityManager.cpp \
	$(ENGINE_PATH)/Util/ETHSpeedTimer.cpp \
	$(ENGINE_PATH)/Util/ETHASUtil.cpp \
	$(ENGINE_PATH)/Util/ETHDateTime.cpp \
	$(ENGINE_PATH)/Util/ETHInput.cpp \
	$(ENGINE_PATH)/Util/ETHGlobalScaleManager.cpp \
	$(ENGINE_PATH)/Entity/ETHScriptEntity.cpp \
	$(ENGINE_PATH)/Entity/ETHRenderEntity.cpp \
	$(ENGINE_PATH)/Entity/ETHSpriteEntity.cpp \
	$(ENGINE_PATH)/Entity/ETHEntityArray.cpp \
	$(ENGINE_PATH)/Entity/ETHEntity.cpp \
	$(ENGINE_PATH)/Entity/ETHEntityController.cpp \
	$(ENGINE_PATH)/Entity/ETHEntityCache.cpp \
	$(ENGINE_PATH)/Entity/ETHEntityProperties.cpp \
	$(ENGINE_PATH)/Entity/ETHCustomDataManager.cpp \
	$(ENGINE_PATH)/Entity/ETHLight.cpp \
	$(ENGINE_PATH)/Entity/ETHEntityChooser.cpp \
	$(ENGINE_PATH)/Shader/ETHShaders.glsl.cpp \
	$(ENGINE_PATH)/Shader/ETHParallaxManager.cpp \
	$(ENGINE_PATH)/Shader/ETHShaderManager.cpp \
	$(ENGINE_PATH)/Shader/ETHVertexLightDiffuse.cpp \
	$(ENGINE_PATH)/Shader/ETHPixelLightDiffuseSpecular.cpp \
	$(ENGINE_PATH)/Shader/ETHFakeEyePositionManager.cpp \
	$(ENGINE_PATH)/Shader/ETHLightmapGen.cpp \
	$(ENGINE_PATH)/Shader/ETHBackBufferTargetManager.cpp \
	$(ENGINE_PATH)/Shader/ETHDefaultDynamicBackBuffer.cpp \
	$(ENGINE_PATH)/Shader/ETHNoDynamicBackBuffer.cpp \
	$(ENGINE_PATH)/Scene/ETHBucketManager.cpp \
	$(ENGINE_PATH)/Scene/ETHScene.cpp \
	$(ENGINE_PATH)/Scene/ETHTempEntityHandler.cpp \
	$(ENGINE_PATH)/Scene/ETHSceneProperties.cpp \
	$(ENGINE_PATH)/Script/ETHScriptWrapper.cpp \
	$(ENGINE_PATH)/Script/ETHScriptWrapper.Math.cpp \
	$(ENGINE_PATH)/Script/ETHScriptWrapper.Math.generic.cpp \
	$(ENGINE_PATH)/Script/ETHScriptWrapper.generic.cpp \
	$(ENGINE_PATH)/Script/ETHScriptObjRegister.cpp \
	$(ENGINE_PATH)/Script/ETHScriptObjRegister.generic.cpp \
	$(ENGINE_PATH)/Script/ETHBinaryStream.cpp \
	$(ENGINE_PATH)/Script/ETHEntityDestructorManager.cpp \
	$(ENGINE_PATH)/Script/ETHEntityDestructorScript.cpp \
	$(ENGINE_PATH)/Script/ETHScriptWrapper.Audio.cpp \
	$(ENGINE_PATH)/Script/ETHScriptWrapper.Drawing.cpp \
	$(ENGINE_PATH)/Script/ETHScriptWrapper.Scene.cpp \
	$(ENGINE_PATH)/Script/ETHScriptWrapper.System.cpp \
	$(ENGINE_PATH)/Physics/ETHPhysicsSimulator.cpp \
	$(ENGINE_PATH)/Physics/ETHPhysicsController.cpp \
	$(ENGINE_PATH)/Physics/ETHRayCastCallback.cpp \
	$(ENGINE_PATH)/Physics/ETHContactListener.cpp \
	$(ENGINE_PATH)/Physics/ETHCollisionBox.cpp \
	$(ENGINE_PATH)/Physics/ETHCompoundShape.cpp \
	$(ENGINE_PATH)/Physics/ETHPolygon.cpp \
	$(ENGINE_PATH)/Physics/ETHDestructionListener.cpp \
	$(ENGINE_PATH)/Physics/ETHJoint.cpp \
	$(ENGINE_PATH)/Physics/ETHPhysicsEntityController.cpp \
	$(ENGINE_PATH)/Physics/ETHRevoluteJoint.cpp

# REMOVE ApplicationTemplate.cpp AND ENTER YOUR SOURCE FILES HERE:
# LOCAL_SRC_FILES += ApplicationTemplate.cpp

LOCAL_LDLIBS := -ldl -llog -lGLESv2 -lz

include $(BUILD_SHARED_LIBRARY)
# include $(BUILD_STATIC_LIBRARY)
