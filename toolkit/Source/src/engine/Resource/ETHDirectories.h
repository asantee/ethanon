#ifndef ETH_DIRECTORIES_H_
#define ETH_DIRECTORIES_H_

#include <string>

class ETHDirectories
{
public:
	static std::string GetShaderDirectory();
	static std::string GetBitmapFontDirectory();
	static std::string GetSceneDirectory();
	static std::string GetEntityDirectory();
	static std::string GetNormalMapDirectory();
	static std::string GetHaloDirectory();
	static std::string GetParticlesDirectory();
	static std::string GetEffectsDirectory();
	static std::string GetSoundFXDirectory();
};

#endif
