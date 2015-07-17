
#Codigo verificador de plataforma
if(WIN32)					
	set(randPlatformFiles

		src/Platform/FileIOHub.cpp
		src/Platform/FileIOHub.h
		src/Platform/FileLogger.cpp
		src/Platform/FileLogger.h
		src/Platform/Logger.h
		src/Platform/Platform.cpp
		src/Platform/Platform.h
	)
	
	set(implementationsWin32Files
		src/Platform/windows/Platform.windows.cpp
	)
	
	set(implementationsAbstractFiles
		src/Platform/abstract/Platform.abstract.cpp
	)
	
	set(implementationsLoosenFiles

		src/Platform/FileManager.cpp
		src/Platform/FileManager.h
		src/Platform/StdFileManager.cpp
		src/Platform/StdFileManager.h
		src/Platform/windows/WindowsFileIOHub.cpp
		src/Platform/windows/WindowsFileIOHub.h
		src/Platform/ZipFileManager.cpp
		src/Platform/ZipFileManager.h
	)

	set(sharedDataFiles

		src/Platform/SharedData/SharedData.cpp
		src/Platform/SharedData/SharedData.h	
		src/Platform/SharedData/SharedDataManager.cpp
		src/Platform/SharedData/SharedDataManager.h
	)
	
	# gera a biblioteca Platform, estatica, para Windows
	add_library(Platform STATIC ${randPlatformFiles} ${implementationsWin32Files} ${implementationsAbstractFiles} ${implementationsLoosenFiles} 	${sharedDataFiles}
			)

	# Cria formatacao de pastas no VS
	source_group(Implementations FILES ${implementationsLoosenFiles})
	source_group(Implementations\\Abstract FILES ${implementationsAbstractFiles})
	source_group(Implementations\\Win32 FILES ${implementationsWin32Files})		
	source_group(SharedData FILES ${sharedDataFiles})


#Codigo verificador de plataforma
elseif(UNIX)

	# Busca no sistema se existe SDL2
	find_library(SDL2_LIBRARY SDL2)

	# Busca no sistema o caminho para SDL2/SDL.h
	find_path(SDL2_INCLUDE_DIRS SDL2/SDL.h)

	#Adiciona o caminho SDL2/SDL.h ao caminho de busca do projeto
	include_directories(${SDL2_INCLUDE_DIRS})
	

	set(sharedData
		src/Platform/SharedData/SharedData.cpp
		src/Platform/SharedData/SharedDataManager.cpp
	)

	set(macosx
		src/Platform/macosx/Platform.macosx.mm
		src/Platform/macosx/MacOSXFileIOHub.mm
	)

	set(randPlatformFiles
		src/Platform/Platform.cpp
		src/Platform/FileIOHub.cpp
		src/Platform/FileLogger.cpp
		src/Platform/FileManager.cpp
		src/Platform/StdFileManager.cpp
	)

	
	set(sdl
		src/Platform/sdl/SDLWindow.cpp
	)

	# gera a biblioteca Platform, estatica, para MacOsX
	add_library(Platform STATIC ${sharedData} ${macosx} ${randPlatformFiles} ${sdl})

#Fim da verificacao de plataformas
endif()
