#Codigo verificador de Plataforma
if(WIN32)

	set(engineCoreDrawing	
		Drawing/ETHDrawable.cpp
		Drawing/ETHDrawable.h
		Drawing/ETHDrawableManager.cpp
		Drawing/ETHDrawableManager.h
		Drawing/ETHParticleDrawer.cpp
		Drawing/ETHParticleDrawer.h
	)
	
	
	set(engineCoreEntity
		Entity/ETHCustomDataManager.cpp
		Entity/ETHCustomDataManager.h
		Entity/ETHEntity.cpp
		Entity/ETHEntity.h
		Entity/ETHEntityArray.cpp
		Entity/ETHEntityArray.h
		Entity/ETHEntityCache.cpp
		Entity/ETHEntityCache.h
		Entity/ETHEntityChooser.cpp
		Entity/ETHEntityChooser.h
		Entity/ETHEntityController.cpp
		Entity/ETHEntityController.h
		Entity/ETHEntityProperties.cpp
		Entity/ETHEntityProperties.h
		Entity/ETHLight.cpp
		Entity/ETHLight.h
		Entity/ETHRenderEntity.cpp
		Entity/ETHRenderEntity.h
		Entity/ETHScriptEntity.cpp
		Entity/ETHScriptEntity.h
		Entity/ETHSpriteEntity.cpp
		Entity/ETHSpriteEntity.h
	)
	
	set(engineCoreParticles
		Particles/ETHParticleManager.cpp
		Particles/ETHParticleManager.h
		Particles/ETHParticleSystem.cpp
		Particles/ETHParticleSystem.h
	)
	
	set(engineCorePhysics	
		Physics/ETHCollisionBox.cpp
		Physics/ETHCollisionBox.h
		Physics/ETHCompoundShape.cpp
		Physics/ETHCompoundShape.h
		Physics/ETHContactListener.cpp
		Physics/ETHContactListener.h
		Physics/ETHDestructionListener.cpp
		Physics/ETHDestructionListener.h
		Physics/ETHJoint.cpp
		Physics/ETHJoint.h
		Physics/ETHPhysicsController.cpp
		Physics/ETHPhysicsController.h
		Physics/ETHPhysicsEntityController.cpp
		Physics/ETHPhysicsEntityController.h
		Physics/ETHPhysicsSimulator.cpp
		Physics/ETHPhysicsSimulator.h
		Physics/ETHPolygon.cpp
		Physics/ETHPolygon.h
		Physics/ETHRayCastCallback.cpp
		Physics/ETHRayCastCallback.h
		Physics/ETHRevoluteJoint.cpp
		Physics/ETHRevoluteJoint.h
	)
	
	set(engineCoreRenderer
		Renderer/ETHEntityHaloRenderer.cpp
		Renderer/ETHEntityHaloRenderer.h
		Renderer/ETHEntityParticleRenderer.cpp
		Renderer/ETHEntityParticleRenderer.h
		Renderer/ETHEntityPieceRenderer.cpp
		Renderer/ETHEntityPieceRenderer.h
		Renderer/ETHEntityRenderingManager.cpp
		Renderer/ETHEntityRenderingManager.h
		Renderer/ETHEntitySpriteRenderer.cpp
		Renderer/ETHEntitySpriteRenderer.h
	)
	
	set(engineCoreResouce
		Resource/ETHDirectories.cpp
		Resource/ETHDirectories.h
		Resource/ETHResourceManager.cpp
		Resource/ETHResourceManager.h
		Resource/ETHResourceProvider.cpp
		Resource/ETHResourceProvider.h
		Resource/ETHSpriteDensityManager.cpp
		Resource/ETHSpriteDensityManager.h
	)
	
	set(engineCoreScene
		Scene/ETHActiveEntityHandler.cpp
		Scene/ETHActiveEntityHandler.h
		Scene/ETHBucketManager.cpp
		Scene/ETHBucketManager.h
		Scene/ETHScene.cpp
		Scene/ETHScene.h
		Scene/ETHSceneProperties.cpp
		Scene/ETHSceneProperties.h
	)
	
	set(engineCoreScriptLoosenFiles
		Script/ETHBinaryStream.cpp
		Script/ETHBinaryStream.h
		Script/ETHScriptObjRegister.cpp
		Script/ETHScriptObjRegister.h
		Script/ETHScriptWrapper.Audio.cpp
		Script/ETHScriptWrapper.cpp
		Script/ETHScriptWrapper.Drawing.cpp
		Script/ETHScriptWrapper.h
		Script/ETHScriptWrapper.Math.cpp
		Script/ETHScriptWrapper.Scene.cpp
		Script/ETHScriptWrapper.SharedData.cpp
		Script/ETHScriptWrapper.System.cpp
	)
	
	set(engineCoreScriptGeneric
		Script/ETHScriptObjRegister.generic.cpp
		Script/ETHScriptWrapper.generic.cpp
		Script/ETHScriptWrapper.Math.generic.cpp
	)
	
	set(engineCoreScriptAddons
		${PROJECT_SOURCE_DIR}/Source/src/addons/aswrappedcall.h
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptarray.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptarray.h
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptbuilder.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptbuilder.h
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptdictionary.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptdictionary.h
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptfile.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptfile.h
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptmath.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptmath.h
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptmath2d.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptmath2d.h
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptmath3d.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptmath3d.h
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptstdstring.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptstdstring.h
	)
	
	set(engineCoreShader
		Shader/ETHBackBufferTargetManager.cpp
		Shader/ETHBackBufferTargetManager.h
		Shader/ETHDefaultDynamicBackBuffer.cpp
		Shader/ETHDefaultDynamicBackBuffer.h
		Shader/ETHDynamicBackBuffer.h
		Shader/ETHFakeEyePositionManager.cpp
		Shader/ETHFakeEyePositionManager.h
		Shader/ETHLightingProfile.h
		Shader/ETHLightmapGen.cpp
		Shader/ETHLightmapGen.h
		Shader/ETHNoDynamicBackBuffer.cpp
		Shader/ETHNoDynamicBackBuffer.h
		Shader/ETHParallaxManager.cpp
		Shader/ETHParallaxManager.h
		Shader/ETHPixelLightDiffuseSpecular.cpp
		Shader/ETHPixelLightDiffuseSpecular.h
		Shader/ETHShaderManager.cpp
		Shader/ETHShaderManager.h
		Shader/ETHShaders.cg.cpp
		Shader/ETHShaders.h
		Shader/ETHVertexLightDiffuse.cpp
		Shader/ETHVertexLightDiffuse.h
	)
	
	set(engineCoreUtil
		Util/ETHASUtil.cpp
		Util/ETHASUtil.h
		Util/ETHDateTime.cpp
		Util/ETHDateTime.h
		Util/ETHFileChangeDetector.cpp
		Util/ETHFileChangeDetector.h
		Util/ETHFrameTimer.cpp
		Util/ETHFrameTimer.h
		Util/ETHGlobalScaleManager.cpp
		Util/ETHGlobalScaleManager.h
		Util/ETHInput.cpp
		Util/ETHInput.h
		Util/ETHSpeedTimer.cpp
		Util/ETHSpeedTimer.h
	)
	
	set(engineCoreLoosenFiles
		ETHEngine.cpp
		ETHEngine.h
		ETHTypes.h
	)
	
	set(enginePlatform
		Platform/ETHAppEnmlFile.cpp
		Platform/ETHAppEnmlFile.h
		Platform/ETHPlatform.abstract.cpp
		Platform/ETHPlatform.h
		Platform/windows/ETHPlatform.windows.cpp
		Platform/FileListing.abstract.cpp
		Platform/FileListing.h	
		Platform/windows/FileListing.windows.cpp
	)

	set(enginePlatformMaxOSX
		Platform/ETHAppEnmlFile.cpp
		Platform/ETHAppEnmlFile.h
		Platform/ETHPlatform.abstract.cpp
		Platform/ETHPlatform.h
		Platform/FileListing.abstract.cpp
		Platform/FileListing.h	
	)
	
	set(engineVendors
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinystr.cpp
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinystr.h
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinyxml.cpp
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinyxml.h
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinyxmlerror.cpp
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinyxmlparser.cpp
	)
	
	#Gera biblioteca estatica Ethanon_Engine para Windows
	add_library(Ethanon_Engine STATIC ${engineCoreDrawing} ${engineCoreEntity}
		${engineCoreParticles} ${engineCorePhysics} ${engineCoreRenderer} 
		${engineCoreResouce} ${engineCoreScene} ${engineCoreScriptLoosenFiles} 
		${engineCoreScriptGeneric} ${engineCoreScriptAddons}
		${engineCoreShader} ${engineCoreUtil} ${engineCoreLoosenFiles} ${enginePlatform}
		${engineVendors}
	)

	#Gera o sistema de pastas no VS
	source_group(Core\\Drawing FILES ${engineCoreDrawing})
	source_group(Core\\Entity FILES ${engineCoreEntity})
	source_group(Core\\Particles FILES ${engineCoreParticles})
	source_group(Core\\Physics FILES ${engineCorePhysics})
	source_group(Core\\Renderer FILES ${engineCoreRenderer})
	source_group(Core\\Resouce FILES ${engineCoreResouce})
	source_group(Core\\Scene FILES ${engineCoreScene})
	source_group(Core\\Script FILES ${engineCoreScriptLoosenFiles})
	source_group(Core\\Script\\addons FILES ${engineCoreScriptAddons})
	source_group(Core\\Script\\Generic FILES ${engineCoreScriptGeneric})
	source_group(Core\\Shader FILES ${engineCoreShader})
	source_group(Core\\Util FILES ${engineCoreUtil})
	source_group(Core FILES ${engineCoreLoosenFiles})
	source_group(Platform FILES ${enginePlatform})
	source_group(Vendors\\tinyxml FILES ${engineVendors})
	
#Codigo verificador de platafroma	
elseif(UNIX)
	
	set(engineCoreDrawing
		Drawing/ETHDrawable.cpp
		Drawing/ETHDrawableManager.cpp
		Drawing/ETHParticleDrawer.cpp
	)
	
	set(engineCoreEntity	
		Entity/ETHCustomDataManager.cpp
		Entity/ETHEntity.cpp
		Entity/ETHEntityArray.cpp
		Entity/ETHEntityCache.cpp
		Entity/ETHEntityChooser.cpp
		Entity/ETHEntityController.cpp
		Entity/ETHEntityProperties.cpp
		Entity/ETHLight.cpp
		Entity/ETHRenderEntity.cpp
		Entity/ETHScriptEntity.cpp
		Entity/ETHSpriteEntity.cpp
	)
	
	
	set(engineCoreParticles
		Particles/ETHParticleManager.cpp
		Particles/ETHParticleSystem.cpp		
	)
	
	set(engineCorePhysics
		Physics/ETHCollisionBox.cpp
		Physics/ETHCompoundShape.cpp
		Physics/ETHContactListener.cpp
		Physics/ETHDestructionListener.cpp
		Physics/ETHJoint.cpp
		Physics/ETHPhysicsController.cpp
		Physics/ETHPhysicsEntityController.cpp
		Physics/ETHPhysicsSimulator.cpp
		Physics/ETHPolygon.cpp
		Physics/ETHRayCastCallback.cpp
		Physics/ETHRevoluteJoint.cpp
	)
	
	set(engineCoreRenderer
		Renderer/ETHEntityHaloRenderer.cpp
		Renderer/ETHEntityParticleRenderer.cpp
		Renderer/ETHEntityPieceRenderer.cpp
		Renderer/ETHEntityRenderingManager.cpp
		Renderer/ETHEntitySpriteRenderer.cpp
	)
	
	set(engineCoreResouce
		Resource/ETHDirectories.cpp
		Resource/ETHResourceManager.cpp
		Resource/ETHResourceProvider.cpp
		Resource/ETHSpriteDensityManager.cpp
	)
	
	set(engineCoreScene
		Scene/ETHActiveEntityHandler.cpp
		Scene/ETHBucketManager.cpp
		Scene/ETHScene.cpp
		Scene/ETHSceneProperties.cpp
	)
	
	set(engineCoreScriptLoosenFiles
		Script/ETHBinaryStream.cpp
		Script/ETHScriptObjRegister.cpp
		Script/ETHScriptWrapper.Audio.cpp
		Script/ETHScriptWrapper.cpp
		Script/ETHScriptWrapper.Drawing.cpp
		Script/ETHScriptWrapper.Math.cpp
		Script/ETHScriptWrapper.Scene.cpp
		Script/ETHScriptWrapper.SharedData.cpp
		Script/ETHScriptWrapper.System.cpp
	)
	
	set(engineCoreScriptGeneric
		Script/ETHScriptObjRegister.generic.cpp
		Script/ETHScriptWrapper.generic.cpp
		Script/ETHScriptWrapper.Math.generic.cpp
	)
	
	set(engineCoreScriptAddons
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptarray.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptbuilder.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptdictionary.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptfile.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptmath.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptmath2d.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptmath3d.cpp
		${PROJECT_SOURCE_DIR}/Source/src/addons/scriptstdstring.cpp
	)
	
	set(engineCoreShader
		Shader/ETHBackBufferTargetManager.cpp
		Shader/ETHDefaultDynamicBackBuffer.cpp
		Shader/ETHFakeEyePositionManager.cpp
		Shader/ETHLightmapGen.cpp
		Shader/ETHNoDynamicBackBuffer.cpp
		Shader/ETHParallaxManager.cpp
		Shader/ETHPixelLightDiffuseSpecular.cpp
		Shader/ETHShaderManager.cpp
		Shader/ETHShaders.cg.cpp
		Shader/ETHVertexLightDiffuse.cpp
	)
	
	set(engineCoreUtil
		Util/ETHASUtil.cpp
		Util/ETHDateTime.cpp
		Util/ETHFileChangeDetector.cpp
		Util/ETHFrameTimer.cpp
		Util/ETHGlobalScaleManager.cpp
		Util/ETHInput.cpp
		Util/ETHSpeedTimer.cpp
	)
	
	set(engineCoreLoosenFiles
		ETHEngine.cpp
	)
	
	
	set(enginePlatform
		Platform/ETHAppEnmlFile.cpp
		Platform/ETHPlatform.abstract.cpp
		Platform/windows/ETHPlatform.windows.cpp
		Platform/FileListing.abstract.cpp
		Platform/windows/FileListing.windows.cpp
	)

	set(enginePlatformMaxOSX
		Platform/ETHAppEnmlFile.cpp
		Platform/ETHPlatform.abstract.cpp
		Platform/FileListing.abstract.cpp
	)
	
	set(engineVendors
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinystr.cpp
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinyxml.cpp
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinyxmlerror.cpp
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinyxmlparser.cpp
	)
		
	set(FilesForMac	
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinyxmlparser.cpp
	)

	set(Tiny
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinystr.cpp
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinyxml.cpp
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinyxmlerror.cpp
		${PROJECT_SOURCE_DIR}/Source/src/vendors/tinyxml_ansi/tinyxmlparser.cpp
	)
		
	#Gera Biblioteca estatica Ethanon_engine para MacOsX
	add_library(Ethanon_Engine STATIC ${engineCoreDrawing} ${engineCoreEntity}
		${engineCoreParticles} ${engineCorePhysics} ${engineCoreRenderer} 
		${engineCoreResouce} ${engineCoreScene} ${engineCoreScriptLoosenFiles} 
		${engineCoreScriptGeneric} ${engineCoreScriptAddons}
		${engineCoreShader} ${engineCoreUtil} ${engineCoreLoosenFiles} ${enginePlatformMaxOSX}
		${engineVendors} ${FilesForMac} ${Tiny}
	)

	#Encontra uma Biblioteca
	find_library( APPKIT_LIBRARY AppKit)
	find_library( COREDATA_LIBRARY CoreData)
	find_library( COCOA_LIBRARY Cocoa)
	find_library( FOUNDATION_LIBRARY Foundation)
		
#Fim do Codigo Seletor	
endif()