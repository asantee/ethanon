#include "ETHDirectories.h"

#define ETH_SHADER_DIRECTORY		"data/"
#define ETH_BITMAP_FONT_DIRECTORY	"data/"
#define ETH_SCENES_DIRECTORY		"scenes/"
#define ETH_ENTITY_DIRECTORY		"entities/"
#define ETH_NORMAL_DIRECTORY		"entities/normalmaps/"
#define ETH_HALOS_DIRECTORY			"entities/"
#define ETH_PARTICLES_DIRECTORY		"particles/"
#define ETH_EFFECTS_DIRECTORY		"effects/"
#define ETH_SOUNDFX_DIRECTORY		"soundfx/"

std::string ETHDirectories::GetShaderDirectory()
{
	return ETH_SHADER_DIRECTORY;
}

std::string ETHDirectories::GetBitmapFontDirectory()
{
	return ETH_BITMAP_FONT_DIRECTORY;
}

std::string ETHDirectories::GetSceneDirectory()
{
	return ETH_SCENES_DIRECTORY;
}

std::string ETHDirectories::GetEntityDirectory()
{
	return ETH_ENTITY_DIRECTORY;
}

std::string ETHDirectories::GetNormalMapDirectory()
{
	return ETH_NORMAL_DIRECTORY;
}

std::string ETHDirectories::GetHaloDirectory()
{
	return ETH_HALOS_DIRECTORY;
}

std::string ETHDirectories::GetParticlesDirectory()
{
	return ETH_PARTICLES_DIRECTORY;
}

std::string ETHDirectories::GetEffectsDirectory()
{
	return ETH_EFFECTS_DIRECTORY;
}

std::string ETHDirectories::GetSoundFXDirectory()
{
	return ETH_SOUNDFX_DIRECTORY;
}
