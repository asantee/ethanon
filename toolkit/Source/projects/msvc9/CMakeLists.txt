
set(msvc9Files
	afxres.h
	resource.h
	file_version.rc
	resource.rc
)

#Cria a Biblioteca estatica Ethanon_machine	
add_library(Ethanon_Machine STATIC ${msvc9Files})

#Cria sistema de pastas no VS	
source_group(Resource_Files FILES ${msvc9Files})

	
	