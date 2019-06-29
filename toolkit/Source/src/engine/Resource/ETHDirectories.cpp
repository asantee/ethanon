#include "ETHDirectories.h"

#define ETH_SHADER_DIRECTORY		GS_L("data/")
#define ETH_BITMAP_FONT_DIRECTORY	GS_L("data/")
#define ETH_SCENES_DIRECTORY		GS_L("scenes/")
#define ETH_ENTITY_DIRECTORY		GS_L("entities/")
#define ETH_NORMAL_DIRECTORY		GS_L("entities/normalmaps/")
#define ETH_HALOS_DIRECTORY			GS_L("entities/")
#define ETH_PARTICLES_DIRECTORY		GS_L("particles/")
#define ETH_EFFECTS_DIRECTORY		GS_L("effects/")
#define ETH_SOUNDFX_DIRECTORY		GS_L("soundfx/")

gs2d::str_type::string ETHDirectories::GetShaderDirectory()
{
	return ETH_SHADER_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetBitmapFontDirectory()
{
	return ETH_BITMAP_FONT_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetSceneDirectory()
{
	return ETH_SCENES_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetEntityDirectory()
{
	return ETH_ENTITY_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetNormalMapDirectory()
{
	return ETH_NORMAL_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetHaloDirectory()
{
	return ETH_HALOS_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetParticlesDirectory()
{
	return ETH_PARTICLES_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetEffectsDirectory()
{
	return ETH_EFFECTS_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetSoundFXDirectory()
{
	return ETH_SOUNDFX_DIRECTORY;
}
