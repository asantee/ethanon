#Codigo verificadro de platafromas
if(WIN32)
	# Set de flags
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd") #ele tava forçando um modo de Debug, nao sei pq.
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")

	#Inclui os diretorios a baixo no caminho de busca do projeto
	include_directories(
				
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/BoostSDK/"	
		"${PROJECT_SOURCE_DIR}/Source/src/box2d/"	
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/"		
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/DX9SDK/Lib/"
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/CgSDK/lib/"
	)

	set(editorHeaderEditor

		CustomDataEditor.h
		EditorBase.h
		EntityEditor.h
		IEditorPlugin.h
		ParticleFXEditor.h
		ParticleScale.h
		ProjectManager.h
		SceneEditor.h
	)
	
	set(editorHeaderHelper
		EditorCommon.h
		gsgui.h
	)
	
	set(editorSourceEditor

		CustomDataEditor.cpp
		EditorBase.cpp
		EntityEditor.cpp
		ParticleFXEditor.cpp
		ParticleScale.cpp
		ProjectManager.cpp
		SceneEditor.cpp
	)
	
	set(editorSourceHelper
		gsgui.cpp
	)
	
	set(editorSourceLostFile
		main.cpp
	)
	
	set(WIN
		 EditorBase.windows.cpp
	)


	# Define diretrizes de processamento
	add_definitions(-DNDEBUG -D_CONSOLE -D_ETHANON_EDITOR -D_CRT_SECURE_NO_WARNINGS -DUNSAFE_UTF8_ALLOWED)

	#Gera um executavel, Ethanon_editor p/ Windows
	add_executable(Ethanon_Editor EXCLUDE_FROM_ALL ${editorHeaderEditor} ${editorHeaderHelper} 
		${editorResource} 
		${editorSourceEditor} 
		${editorSourceHelper} 
		${editorSourceLostFile}
		${WIN}
	)
	

	#Liga o executavel nas bibliotecas
	target_link_libraries(Ethanon_Editor 
			
		WinMM.Lib
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/CgSDK/lib/cg.lib 
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/CgSDK/lib/cgD3D9.lib 					
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/DX9SDK/Lib/d3d9.lib 
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/DX9SDK/Lib/d3dx9.lib 
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/DX9SDK/Lib/strmiids.lib
	)

	#Liga o Executavel aos outros modulos do projeto
	target_link_libraries(Ethanon_Editor 
		angelscript
		audiere
		Box2D
		Ethanon_Engine
		GS2D
		hashlib
		libzip
		Platform
		zlib
	)
	
	# Comando feito para pegar o Audiere.dll e copia-lo para a pasta /bin/Debug
	add_custom_command(TARGET Ethanon_Editor POST_BUILD    # Adds a post-build event to MyTest
	COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
    	"${CMAKE_BINARY_DIR}/lib/Debug/audiere.dll"      # <--this is in-file
        ${CMAKE_BINARY_DIR}/bin/Debug)                # <--this is out-file path

    #Gera sistema de pastas p/ p VS
	source_group(Header_Files\\Editor FILES ${editorHeaderEditor})
	source_group(Header_Files\\Helper FILES ${editorHeaderHelper})
	source_group(Source_Files\\Editor FILES ${editorSourceEditor})
	source_group(Source_Files\\Helper FILES ${editorSourceHelper})
	source_group(Source_Files FILES ${editorSourceLostFile})

#Codigo verificador de plataforma
elseif(UNIX)

	#Inclui os diretorios a baixo no caminho de busca do projeto
	include_directories(
				
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/BoostSDK/"	
		"${PROJECT_SOURCE_DIR}/Source/src/box2d/"	
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/"		
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/DX9SDK/Lib/"
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/CgSDK/lib/"
	)

	set(editorSourceEditor

		CustomDataEditor.cpp
		EditorBase.cpp
		EntityEditor.cpp
		ParticleFXEditor.cpp
		ParticleScale.cpp
		ProjectManager.cpp
		SceneEditor.cpp
	)
	
	set(editorSourceHelper
		gsgui.cpp
	)
	
	set(editorSourceLostFile
		main.cpp
	)
	
	set(FileListing
		${PROJECT_SOURCE_DIR}/Source/src/engine/Platform/macosx/FileListing.mm
	)

	set(macosX
		EditorBase.macosx.mm
	)

	#Procura as bibliotecas
	find_library(SDL2_LIBRARY SDL2)
	find_library( APPKIT_LIBRARY AppKit)
	find_library( COREDATA_LIBRARY CoreData)
	find_library( COCOA_LIBRARY Cocoa)
	find_library( FOUNDATION_LIBRARY Foundation)
	find_library( CG_LIBRARY Cg)
	find_library( OPENGL_LIBRARY OpenGl)
	find_library( AVFoundation_LIBRARY AVFoundation)
	find_library( AudioToolbox_LIBRARY AudioToolbox)
	find_library( OpenAL_LIBRARY OpenAL)

	#Encontra o caminho para SDL2/SDL.h
	find_path(SDL2_INCLUDE_DIRS SDL2/SDL.h)

	#Inclui o caminho para SDL2/SDL.h ao caminho de busca do projeto
	include_directories(${SDL2_INCLUDE_DIRS})

		
	# Define diretrizes de processamento
	add_definitions(-DNDEBUG -D_CONSOLE -D_ETHANON_EDITOR -D_CRT_SECURE_NO_WARNINGS -DUNSAFE_UTF8_ALLOWED)
	
	#GERA O EXECUTAVAL PARA MACOSX
	add_executable(Ethanon_Editor 
		${editorResource} ${editorSourceEditor} 
		${editorSourceHelper} ${editorSourceLostFile}
		${FileListing} ${macosX}
	)

	#ligando o executavel em todas suas dependencias.
	target_link_libraries (Ethanon_Editor 
		${APPKIT_LIBRARY}
		${COREDATA_LIBRARY}
		${COCOA_LIBRARY}
		${FOUNDATION_LIBRARY}
		${OPENGL_LIBRARY}
		${CG_LIBRARY}
		${SDL2_LIBRARY}
		${AVFoundation_LIBRARY}
		${AudioToolbox_LIBRARY}
		${OpenAL_LIBRARY}
		angelscript
		hashlib
		GS2D
		Box2D
		Soil
		Ethanon_Engine
		Platform
	)

endif()