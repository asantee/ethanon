#ifndef ETH_SPRITE_DENSITY_MANAGER_H_
#define ETH_SPRITE_DENSITY_MANAGER_H_

#include <Enml/Enml.h>
#include <Video.h>

class ETHAppEnmlFile;

class ETHSpriteDensityManager
{
public:
	static const std::string HD_VERSION_PATH_NAME;
	static const std::string FULL_HD_VERSION_PATH_NAME;
	static const std::string LD_VERSION_PATH_NAME;
	static const std::string XLD_VERSION_PATH_NAME;

	enum DENSITY_LEVEL
	{
		NORMAL = 0,
		HD = 1,
		FULL_HD = 2,
		LD = 3, // low-def
		XLD = 4 // very low-def
	};

	ETHSpriteDensityManager();
	void FillParametersFromFile(const ETHAppEnmlFile& file);

	bool ShouldUseHdResources(const gs2d::VideoPtr& video) const;
	bool ShouldUseFullHdResources(const gs2d::VideoPtr& video) const;

	bool ShouldUseLdResources(const gs2d::VideoPtr& video) const;
	bool ShouldUseXLdResources(const gs2d::VideoPtr& video) const;

	std::string ChooseSpriteVersion(const std::string& fullFilePath,
		const gs2d::VideoPtr& video, ETHSpriteDensityManager::DENSITY_LEVEL& densityLevel);
	void SetSpriteDensity(const gs2d::SpritePtr& sprite, const DENSITY_LEVEL& level) const;
	float GetDensityValue(const DENSITY_LEVEL& level) const;

	float hdDensityValue;
	float fullHdDensityValue;
	float ldDensityValue;
	float xldDensityValue;
	unsigned int minScreenHeightForHdResources, minScreenHeightForFullHdResources;
	unsigned int maxScreenHeightBeforeNdVersion, maxScreenHeightBeforeLdVersion;
};

#endif
