#ifndef ETH_APP_ENML_FILE_H_
#define ETH_APP_ENML_FILE_H_

#include <Enml/Enml.h>

#include <Platform/FileManager.h>

#include "../Resource/ETHSpriteDensityManager.h"

class ETHAppEnmlFile
{
public:
	ETHAppEnmlFile(
		const std::string& fileName,
		const Platform::FileManagerPtr& fileManager,
		const std::string& platformName,
		const std::string& externalStorageDirectory);

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
	std::string GetTitle() const;
	std::string GetFixedWidth() const;
	std::string GetFixedHeight() const;

	const ETHSpriteDensityManager& GetDensityManager() const;
	const std::vector<std::string>& GetDefinedWords() const;

	static void SetAppDefaultVideoMode(
		const gs2d::math::Vector2& size,
		const bool windowed,
		const std::string& externalStorageDirectory);
	static gs2d::math::Vector2 GetAppDefaultVideoMode(
		bool& windowed,
		const std::string& externalStorageDirectory);

private:
	void LoadProperties(
		const std::string& platformName,
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
	std::string title;
	std::string fixedWidth, fixedHeight;

	std::vector<std::string> definedWords;
};

#endif
