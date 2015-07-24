#Codigo verificador de plataforma
if(WIN32)	

	set(hashlibHeaders
		hashlibpp.h
		hl_exception.h
		hl_hashwrapper.h
		hl_md5.h
		hl_md5wrapper.h
		hl_sha1.h
		hl_sha1wrapper.h
		hl_sha2ext.h
		hl_sha2mac.h
		hl_sha256.h
		hl_sha256wrapper.h
		hl_sha384wrapper.h
		hl_sha512wrapper.h
		hl_types.h
		hl_wrapperfactory.h
	)

	set(hashlibSources
		hl_md5.cpp
		hl_md5wrapper.cpp
		hl_sha1.cpp
		hl_sha1wrapper.cpp
		hl_sha256.cpp
		hl_sha256wrapper.cpp
		hl_sha2ext.cpp
		hl_sha384wrapper.cpp
		hl_sha512wrapper.cpp
		hl_wrapperfactory.cpp
	)

	#Croa a biblioteca eastatica hashlib para windowns
	add_library(hashlib STATIC ${hashlibHeaders} ${hashlibSources})	

#Codigo verificador de plataforma
elseif(UNIX)

	set(hashlibSources
		hl_md5.cpp
		hl_md5wrapper.cpp
		hl_sha1.cpp
		hl_sha1wrapper.cpp
		hl_sha256.cpp
		hl_sha256wrapper.cpp
		hl_sha2ext.cpp
		hl_sha384wrapper.cpp
		hl_sha512wrapper.cpp
		hl_wrapperfactory.cpp
	)
	#Gera a biblioteca eastatica hashlib para MacOsX
	add_library(hashlib STATIC ${hashlibSources})

#Fim da verificacao de plataforma
endif()
	