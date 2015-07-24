	
set(zlibHeaders
	crc32.h
	deflate.h
	inffast.h
	inffixed.h
	inflate.h
	inftrees.h
	trees.h
	zconf.h
	zlib.h
	zutil.h
)

set(zlibSources

	adler32.c
	compress.c
	crc32.c
	deflate.c
	infback.c
	inffast.c
	inflate.c
	inftrees.c
	trees.c
	uncompr.c
	zutil.c
)
	
#Cria a bibliotecca estatica zlib para windows
add_library(zlib STATIC ${zlibHeaders} ${zlibSources})