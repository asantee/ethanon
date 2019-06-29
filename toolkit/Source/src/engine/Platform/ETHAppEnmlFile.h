#ifndef ETH_APP_ENML_FILE_H_
#define ETH_APP_ENML_FILE_H_

#include <Enml/Enml.h>

#include <Platform/FileManager.h>

#include "../Resource/ETHSpriteDensityManager.h"

class ETHAppEnmlFile
{
public:
	ETHAppEnmlFile(
		const gs2d::str_type::string& fileName,
		const Platform::FileManagerPtr& fileManager,
		const gs2d::str_type::string& platformName,
		const gs2d::str_type::string& externalStorageDirectory);

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	float GetHdDensityValue() const;
	float GetFullHdDensityValue() const;
	float GetLdDensityValue() const;
	float GetXldDensityValue() const;
	unsigned int GetMinScreenHeightForHdVersion() const;
	unsigned int GetMinScreenHeightForFullHdVersion() const;
	unsigned int GetMaxScreenHeightBeforeNdVersion() const;
	unsigned int GetMaxScreenHeightBeforeLdVersion() const;

	bool IsWindowed() const;
	bool IsVsyncEnabled() const;
	bool IsRichLightingEnabled() const;
	gs2d::str_type::string GetTitle() const;
	gs2d::str_type::string GetFixedWidth() const;
	gs2d::str_type::string GetFixedHeight() const;

	const ETHSpriteDensityManager& GetDensityManager() const;
	const std::vector<gs2d::str_type::string>& GetDefinedWords() const;

	static void SetAppDefaultVideoMode(
		const gs2d::math::Vector2& size,
		const bool windowed,
		const gs2d::str_type::string& externalStorageDirectory);
	static gs2d::math::Vector2 GetAppDefaultVideoMode(
		bool& windowed,
		const gs2d::str_type::string& externalStorageDirectory);

private:
	void LoadProperties(
		const gs2d::str_type::string& platformName,
		const gs2d::enml::File& file);

	ETHSpriteDensityManager densityManager;

	unsigned int width, height;

	float hdDensityValue;
	float fullHdDensityValue;
	float ldDensityValue;
	float xldDensityValue;
	unsigned int minScreenHeightForHdVersion, minScreenHeightForFullHdVersion;
	unsigned int maxScreenHeightBeforeNdVersion, maxScreenHeightBeforeLdVersion;

	gs2d::math::Vector2 appDefaultVideoMode;
	bool appDefaultWindowedMode;

	bool windowed, vsync;
	bool richLighting;
	gs2d::str_type::string title;
	gs2d::str_type::string fixedWidth, fixedHeight;

	std::vector<gs2d::str_type::string> definedWords;
};

#endif
