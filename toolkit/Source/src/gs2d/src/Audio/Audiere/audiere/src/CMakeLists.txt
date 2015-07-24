		SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
		SET(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
		SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
		SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")
		SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
		SET(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
		SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
		SET(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")
		SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
		SET(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL "${CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
		SET(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
		SET(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")
		

	set(audiereFiles
		audiere.h
		audiere.rc
		basic_source.cpp
		basic_source.h
		cd_win32.cpp
		debug.cpp
		debug.h
		default_file.h
		device.cpp
		device.h
		device_ds.cpp
		device_ds.h
		device_ds_buffer.cpp
		device_ds_buffer.h
		device_ds_stream.cpp
		device_ds_stream.h
		device_mixer.cpp
		device_mixer.h
		device_mm.cpp
		device_mm.h
		device_null.cpp
		device_null.h
		dumb_resample.cpp
		dumb_resample.h
		dxguid.cpp
		file_ansi.cpp
		input.cpp
		input_aiff.cpp
		input_aiff.h
		input_flac.cpp
		input_flac.h
		input_mod.cpp
		input_mod.h
		input_mp3.cpp
		input_mp3.h
		input_ogg.cpp
		input_ogg.h
		input_speex.cpp
		input_speex.h
		input_wav.cpp
		input_wav.h
		internal.h
		loop_point_source.cpp
		mci_device.h
		memory_file.cpp
		memory_file.h
		midi_mci.cpp
		noise.cpp
		resampler.cpp
		resampler.h
		resource.h
		sample_buffer.cpp
		sound.cpp
		sound_effect.cpp
		square_wave.cpp
		threads.h
		threads_win32.cpp
		timer.h
		timer_win32.cpp
		tone.cpp
		types.h
		utility.cpp
		utility.h
		version.cpp
	)
	
	set(audiereMpaudec
		mpaudec/bits.c
		mpaudec/internal.h
		mpaudec/mpaudec.c
		mpaudec/mpaudec.h
		mpaudec/mpaudectab.h
		mpaudec/mpegaudio.h
		${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/Audiere/audiere/third-party/vc6/include/FLAC/export.h
	)
	
	set(audiereSpeexfile
		speexfile/speexfile.cpp
		speexfile/speexfile.h
	)
	
	
	include_directories(
						"${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/Audiere/audiere/third-party/vc6/include/"
						"${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/Audiere/audiere/third-party/vc6/lib/"
						"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/DX9SDK/Include/"
						"${PROJECT_SOURCE_DIR}/Source/src/gs2d/vendors/DX9SDK/Lib/"
	)
	
	if(WIN32)
		add_definitions(-DWIN32 -DNDEBUG -D_WINDOWS -D__USRDLL -DAUDIERE_EXPORTS -DNOMINMAX -DWIN32_LEAN_AND_MEAN -DSTRICT -DFLAC__NO_DLL -D_CRT_SECURE_NO_WARNINGS -DETH_UNICODE)
	endif()
	
	
	
	add_library(audiere SHARED ${audiereFiles} ${audiereMpaudec} ${audiereSpeexfile}) # // Gerar o dll	
	

		target_link_libraries(audiere 
		
			WinMM
			rpcrt4
			
			${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/Audiere/audiere/third-party/vc6/lib/dumb_static.lib 
			${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/Audiere/audiere/third-party/vc6/lib/libFLAC_static.lib 
			${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/Audiere/audiere/third-party/vc6/lib/libFLAC++_static.lib
			${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/Audiere/audiere/third-party/vc6/lib/libspeex.lib
			${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/Audiere/audiere/third-party/vc6/lib/ogg_static.lib
			${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/Audiere/audiere/third-party/vc6/lib/vorbis_static.lib
			${PROJECT_SOURCE_DIR}/Source/src/gs2d/src/Audio/Audiere/audiere/third-party/vc6/lib/vorbisfile_static.lib
			
			)
		

source_group(files FILES ${audiereFiles})
source_group(mpaudec FILES ${audiereMpaudec})
source_group(speexfile FILES ${audiereSpeexfile})


