
#Arquvivos necessario em SoIL
set( soilfiles

	image_DXT.c
	image_helper.c
	SOIL.c
	stb_image_aug.c
)
#Cria a biblioteca estatica SOIL que eh necessario no MacOsX
add_library(Soil STATIC ${soilfiles})
