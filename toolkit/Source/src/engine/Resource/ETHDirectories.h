#ifndef ETH_DIRECTORIES_H_
#define ETH_DIRECTORIES_H_

#include <Types.h>

class ETHDirectories
{
public:
	static gs2d::str_type::string GetShaderDirectory();
	static gs2d::str_type::string GetBitmapFontDirectory();
	static gs2d::str_type::string GetSceneDirectory();
	static gs2d::str_type::string GetEntityDirectory();
	static gs2d::str_type::string GetNormalMapDirectory();
	static gs2d::str_type::string GetHaloDirectory();
	static gs2d::str_type::string GetParticlesDirectory();
	static gs2d::str_type::string GetEffectsDirectory();
	static gs2d::str_type::string GetSoundFXDirectory();
};

#endif
