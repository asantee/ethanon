#Codigo verificador de plataforma
if(WIN32)

	#Inclui os diretorios a baixo no caminho de busca do projeto
	include_directories(
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/BoostSDK/"
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/Audiere/audiere/third-party/vc6/include/"
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/zlib_msvc/"
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/libzip_msvc/"
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/"
		"${PROJECT_SOURCE_DIR}/Source/src/box2d/"
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/CgSDK/include/"
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/DX9SDK/Include"
		"${PROJECT_SOURCE_DIR}/Source/src/vendors/hashlib2plus/src"			
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/Audiere/audiere/third-party/vc6/lib/"
	)

	#Adiciona diretrizes de processamento
	add_definitions(-D_ETHANON_EDITOR -DWIN32 -D_DEBUG -D_CONSOLE -D_CRT_SECURE_NO_WARNINGS -DD3D_DEBUG_INFO -DUNSAFE_UTF8_ALLOWED)

	#Seta Flags
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd") #ele tava forçando um modo de Debug, nao sei pq.
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")

	# Cria uma pasta lib para onde o CMake ira conduzir as libs geradas
	SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

	# Cria uma pasta bin para onde o CMake ira conduzir os executaveis geradas
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

	#Indica onde esta o proximo CMakeLists.txt a ser usado, na ordem descrita
	add_subdirectory(src)										#Platform
	add_subdirectory(src/engine)								#Engine
	add_subdirectory(src/editor)								#Editor
	add_subdirectory(src/machine)  								#Ethanon Machine
	add_subdirectory(src/gs2d/src)								#GS2D
	add_subdirectory(src/angelscript)							#AngelScript
	add_subdirectory(src/box2d/Box2D)							#Box2D
	add_subdirectory(src/gs2d/vendors/zlib_msvc)				#zlib
	add_subdirectory(src/gs2d/vendors/libzip_msvc)				#libzip
	add_subdirectory(src/vendors/hashlib2plus/src)				#hashlib
	add_subdirectory(src/gs2d/src/Audio/Audiere/audiere/src)	#Audiere

	#Copia uma serie de arquivos que sao necessarioas para o local onde sao necessarios, O local para onde serao copiadoes esta na ultima linha
	file(COPY 
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/CgSDK/bin/cg.dll
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/CgSDK/bin/cgD3D8.dll
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/CgSDK/bin/cgD3D9.dll
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/CgSDK/bin/cgD3D10.dll
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/CgSDK/bin/cgGL.dll
		${PROJECT_SOURCE_DIR}/Ethanon\ Toolkit/data
		${PROJECT_SOURCE_DIR}/Ethanon\ Toolkit/particles
		DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug
	)

#Codigo verificador de platafroma
elseif(UNIX)
	
	#Inclui os diretorios a baixo no caminho de busca do projeto
	include_directories(
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/BoostSDK/"
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/"
		"${PROJECT_SOURCE_DIR}/Source/src/box2d/"
		"${PROJECT_SOURCE_DIR}/Source/src/vendors/hashlib2plus/src"		
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors"	
		"${PROJECT_SOURCE_DIR}/Source/src/soil/"
		"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/DX9SDK/Include"
	)

	#Define diretrizes de compilacao
	add_definitions(-D_ETHANON_EDITOR -D_CONSOLE -D_CRT_SECURE_NO_WARNINGS -DCD_USE_OWN_FILEUTILS -DMACOSX -DOPENGL)

	# Seta o local onde devem ser colocadas pelo CMake as bibliotecas geradas
	set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	# Seta o local onde devem ser colocadas pelo CMake os executaveis
	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

	#Set de flags necessarias ao Ethanon
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -std=gnu99")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=gnu99")
	SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

	add_subdirectory(src)								#Platform
	add_subdirectory(src/editor)
	add_subdirectory(src/machine)
	add_subdirectory(src/engine)
	add_subdirectory(src/gs2d/src)	
	add_subdirectory(src/box2d/Box2D)
	add_subdirectory(src/angelscript)					#AngelScript
	add_subdirectory(src/soil)
	add_subdirectory(src/vendors/hashlib2plus/src)		#hashlib

	

	#Copiar alguns arquivos necessarios para o local onde serao necessarias
	file(COPY 
		${PROJECT_SOURCE_DIR}/Ethanon\ Toolkit/data
		${PROJECT_SOURCE_DIR}/Ethanon\ Toolkit/particles
		DESTINATION ${CMAKE_BINARY_DIR}/bin/assets
	)


	#Encontra bibliotecas
	find_library( APPKIT_LIBRARY AppKit)
	find_library( COREDATA_LIBRARY CoreData)
	find_library( COCOA_LIBRARY Cocoa)
	find_library( FOUNDATION_LIBRARY Foundation)


#Fim do codigo verificador de platafroma
endif()	

#codigo morto eu acho
set(
	MODULES

	angelscript
	audiere
	Box2D
	Ethanon_Editor
	Ethanon_Engine
	Ethanon_Machine
	GS2D
	hashlib
	libzip
	Platform
	zlib
)