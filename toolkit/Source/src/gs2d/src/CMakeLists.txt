#Inclui diretorios de busca especificos para o GS2D
include_directories(
	${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/CocosDenshion/CocosDenshion/
	#${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/
	${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/soil
)		

#Codigo verificador de plataforma
if(WIN32)
	set(audioFiles
		Audio/Audiere/AudiereAudio.cpp
		Audio/Audiere/AudiereAudio.h
	)
	
	set(enmlFiles
		Enml/Enml.cpp
		Enml/Enml.h
	)
	
	set(headerFilesFolder
		Application.h
		Audio.h
		BaseApplication.h
		Input.h
		Player.h
		Shader.h
		Sprite.h
		Texture.h
		Types.h
		Video.h
		Window.h
	)
	
	set(inputLoosenFiles
		Input/KeyStateManager.cpp
		Input/KeyStateManager.h
	)
	
	set(inputWinFiles
		Input/Win/WinInput.cpp
		Input/Win/WinInput.h
	)
	
	set(mathFiles
		Math/Color.cpp
		Math/Color.h
		Math/GameMath.h
		Math/MersenneTwister.h
		Math/OrientedBoundingBox.cpp
		Math/OrientedBoundingBox.h
		Math/Randomizer.cpp
		Math/Randomizer.h
	)
	
	set(sourceFiles
		Application.cpp
		Sprite.cpp
		Video.cpp
	)
	
	set(unicodeFiles
		Unicode/utf8/utf8/checked.h
		Unicode/utf8/utf8/core.h
		Unicode/utf8/utf8/unchecked.h
		Unicode/utf8/utf8.h
	)
	
	set(videoLoosenFiles
		Video/BitmapFont.cpp
		Video/BitmapFont.h
		Video/BitmapFontManager.cpp
		Video/BitmapFontManager.h
		Video/cgShaderCode.h
	)
	
	set(videoDirect3D9Files
	
		Video/Direct3D9/D3D9CgShader.cpp
		Video/Direct3D9/D3D9CgShader.h
		Video/Direct3D9/D3D9Sprite.cpp
		Video/Direct3D9/D3D9Sprite.cpp
		Video/Direct3D9/D3D9Texture.cpp
		Video/Direct3D9/D3D9Texture.h
		Video/Direct3D9/D3D9Video.cpp
		Video/Direct3D9/D3D9Video.h
		Video/Direct3D9/D3D9VideoInfo.cpp
		Video/Direct3D9/D3D9VideoInfo.h
	)
	
	set(videoPlayerFiles
		Video/DShow/DShowPlayer.cpp
		Video/DShow/DShowPlayer.h
	)

	#Cria a biblioteca estatica GS2d para windows
	add_library(GS2D STATIC ${audioFiles} ${enmlFiles} 
		${headerFilesFolder} ${inputLoosenFiles} ${inputWinFiles}
		${mathFiles} ${sourceFiles} ${unicodeFiles}
		${videoLoosenFiles} ${videoDirect3D9Files} ${videoPlayerFiles} 
	)
		# Gera o sistema de pastas para o VS
		source_group(Audio\\Audiere FILES ${audioFiles})
		source_group(Enml FILES ${enmlFiles})
		source_group(Header_Files FILES ${headerFilesFolder})
		source_group(Input FILES ${inputLoosenFiles})
		source_group(Input\\Win FILES ${inputWinFiles})
		source_group(Math FILES ${mathFiles})
		source_group(Source_Files FILES ${sourceFiles})
		source_group(Unicode FILES ${unicodeFiles})
		source_group(Video FILES ${videoLoosenFiles})
		source_group(Video\\Direct3D9 FILES ${videoDirect3D9Files})
		source_group(Video\\Player FILES ${videoPlayerFiles})
		
#Codigo verificador de plataforma
elseif(UNIX)

	#Adiciona diretrizes de compilacao
	add_definitions(-DMACOSX -DCD_USE_OWN_FILEUTILS )

	set(enmlFiles
		Enml/Enml.cpp
	)
	
	set(inputLoosenFiles
		Input/KeyStateManager.cpp
	)
		
	set(mathFiles
		Math/Color.cpp
		Math/OrientedBoundingBox.cpp
		Math/Randomizer.cpp
	)
	
	set(sourceFiles
		Application.cpp
		Sprite.cpp
		Video.cpp
	)
	
	set(videoLoosenFiles
		Video/BitmapFont.cpp
		Video/BitmapFontManager.cpp
	)
	
	set(GLFiles
		Video/GL/GLRectRenderer.cpp
		Video/GL/GLSprite.cpp
		Video/GL/GLTexture.cpp
		Video/GL/GLVideo.cpp
		Video/GLSDL/GLSDLVideo.cpp
	)
	
	set(GLCGFiles
		Video/GL/Cg/GLCgShader.cpp
		Video/GL/Cg/GLCgShaderContext.cpp
	)
			
	set(SDLINPUT
		Input/SDL/SDLInput.cpp
		Input/SDL/SDLJoystick.cpp
	)
		
	set(FilePlatform2
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Platform/macosx/MacOSXFileIOHub.mm
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Platform/macosx/Platform.macosx.mm
	)
		
	set(FilePlatformSDL
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Platform/sdl/SDLWindow.cpp
	)

	set(Utilities
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Utilities/RecoverableResourceManager.cpp 
	)

	set(CocosDen
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/CocosDenshion/CDAudioContext.mm
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/CocosDenshion/CDAudioSample.mm					
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/CocosDenshion/CocosDenshion/CDAudioManager.m
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/CocosDenshion/CocosDenshion/CDOpenALSupport.m
		#${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/CocosDenshion/CocosDenshion/CDXMacOSXSupport_original.m
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/CocosDenshion/CocosDenshion/CDXMacOSXSupport.m
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/CocosDenshion/CocosDenshion/CDXMacOSXSupport.readme.txt
		#${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/CocosDenshion/CocosDenshion/CDXPropertyModifierAction.m
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/CocosDenshion/CocosDenshion/CocosDenshion.m
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/CocosDenshion/CocosDenshion/SimpleAudioEngine.m
	)
	
	#Cria a biblioteca estatica GS2D para MacOsX
	add_library(GS2D STATIC 
		${enmlFiles} 
		${inputLoosenFiles} 
		${mathFiles} ${sourceFiles} 
		${videoLoosenFiles}
		${GLFiles} ${GLCGFiles} ${SDLINPUT} 
		${FilePlatform2} ${FilePlatformSDL} ${CocosDen}
		${Utilities}
	)
	
	
		#Encontra e carrega configuracoes e um projeto externo
		find_package(OpenGL REQUIRED)

		#Encontra uma Biblioteca
		find_library(SDL2_LIBRARY SDL2)
		find_library( OPENGL_LIBRARY OpenGl)
		find_library( APPKIT_LIBRARY AppKit)
		find_library( COREDATA_LIBRARY CoreData)
		find_library( COCOA_LIBRARY Cocoa)
		find_library( FOUNDATION_LIBRARY Foundation)
		find_library( CG_LIBRARY Cg)

		#Encontra o caminho para SDL2/SDL.h
		find_path(SDL2_INCLUDE_DIRS SDL2/SDL.h)

		#Inclui  o caminho para SDL2/SDL.h ao caminho de buscas do projeto
		include_directories(${SDL2_INCLUDE_DIRS})

#Fim do codigo verificador de plataforma
endif()
		
	