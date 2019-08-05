#include "ETHSpriteDensityManager.h"

#include "../ETHTypes.h"

#include "../Platform/ETHAppEnmlFile.h"

#include <Platform/Platform.h>

using gs2d::str_type::string;

const gs2d::str_type::string ETHSpriteDensityManager::HD_VERSION_PATH_NAME = GS_L("hd/");
const gs2d::str_type::string ETHSpriteDensityManager::FULL_HD_VERSION_PATH_NAME = GS_L("fullhd/");
const gs2d::str_type::string ETHSpriteDensityManager::LD_VERSION_PATH_NAME = GS_L("ld/");
const gs2d::str_type::string ETHSpriteDensityManager::XLD_VERSION_PATH_NAME = GS_L("xld/");

ETHSpriteDensityManager::ETHSpriteDensityManager() :
	hdDensityValue(2.0f),
	fullHdDensityValue(4.0f),
	ldDensityValue(0.5f),
	xldDensityValue(0.25f),
	minScreenHeightForHdResources(720),
	minScreenHeightForFullHdResources(1080),
	maxScreenHeightBeforeNdVersion(480),
	maxScreenHeightBeforeLdVersion(320)
{
}

void ETHSpriteDensityManager::FillParametersFromFile(const ETHAppEnmlFile& file)
{
	hdDensityValue = file.GetHdDensityValue();
	fullHdDensityValue = file.GetFullHdDensityValue();
	ldDensityValue = file.GetLdDensityValue();
	xldDensityValue = file.GetXldDensityValue();
	
	minScreenHeightForHdResources = file.GetMinScreenHeightForHdVersion();
	minScreenHeightForFullHdResources = file.GetMinScreenHeightForFullHdVersion();

	maxScreenHeightBeforeLdVersion = file.GetMaxScreenHeightBeforeLdVersion();
	maxScreenHeightBeforeNdVersion = file.GetMaxScreenHeightBeforeNdVersion();
}

bool ETHSpriteDensityManager::ShouldUseHdResources(const gs2d::VideoPtr& video) const
{
	return (video->GetScreenSizeInPixels().y >= static_cast<int>(minScreenHeightForHdResources)) && !ShouldUseFullHdResources(video);
}

bool ETHSpriteDensityManager::ShouldUseFullHdResources(const gs2d::VideoPtr& video) const
{
	return (video->GetScreenSizeInPixels().y >= static_cast<int>(minScreenHeightForFullHdResources));
}

bool ETHSpriteDensityManager::ShouldUseLdResources(const gs2d::VideoPtr& video) const
{
	return (video->GetScreenSizeInPixels().y <= static_cast<int>(maxScreenHeightBeforeNdVersion)) && !ShouldUseXLdResources(video);
}

bool ETHSpriteDensityManager::ShouldUseXLdResources(const gs2d::VideoPtr& video) const
{
	return (video->GetScreenSizeInPixels().y <= static_cast<int>(maxScreenHeightBeforeLdVersion));
}

static gs2d::str_type::string AssembleResourceName(const string& fullFilePath, const string& versionPathName)
{
	const string folder(Platform::GetFileDirectory(fullFilePath.c_str()));
	const string file(Platform::GetFileName(fullFilePath));
	return (folder + versionPathName + file);
}

gs2d::str_type::string ETHSpriteDensityManager::ChooseSpriteVersion(
	const gs2d::str_type::string& fullFilePath,
	const gs2d::VideoPtr& video,
	ETHSpriteDensityManager::DENSITY_LEVEL& densityLevel)
{
	const Platform::FileManagerPtr fileManager = video->GetFileIOHub()->GetFileManager();
	const bool shouldUseFullHdResources = ShouldUseFullHdResources(video);
	if (shouldUseFullHdResources)
	{
		const string fullHdVersionFileName(AssembleResourceName(fullFilePath, FULL_HD_VERSION_PATH_NAME));
		if (fileManager->FileExists(fullHdVersionFileName))
		{
			densityLevel = ETHSpriteDensityManager::FULL_HD;
			return fullHdVersionFileName;
		}
	}

	if (ShouldUseHdResources(video) || shouldUseFullHdResources)
	{
		const string hdVersionFileName(AssembleResourceName(fullFilePath, HD_VERSION_PATH_NAME));
		if (fileManager->FileExists(hdVersionFileName))
		{
			densityLevel = ETHSpriteDensityManager::HD;
			return hdVersionFileName;
		}
	}
	
	// low definition seeking
	const bool shouldUseXldResources = ShouldUseXLdResources(video);
	if (shouldUseXldResources)
	{
		const string xldVersionFileName(AssembleResourceName(fullFilePath, XLD_VERSION_PATH_NAME));
		if (fileManager->FileExists(xldVersionFileName))
		{
			densityLevel = ETHSpriteDensityManager::XLD;
			return xldVersionFileName;
		}
	}

	if (ShouldUseLdResources(video) || shouldUseXldResources)
	{
		const string ldVersionFileName(AssembleResourceName(fullFilePath, LD_VERSION_PATH_NAME));
		if (fileManager->FileExists(ldVersionFileName))
		{
			densityLevel = ETHSpriteDensityManager::LD;
			return ldVersionFileName;
		}
	}

	densityLevel = ETHSpriteDensityManager::NORMAL;
	return fullFilePath;
}

void ETHSpriteDensityManager::SetSpriteDensity(const gs2d::SpritePtr& sprite, const DENSITY_LEVEL& level) const
{
	sprite->SetPixelDensity(GetDensityValue(level));
}

float ETHSpriteDensityManager::GetDensityValue(const DENSITY_LEVEL& level) const
{
	switch(level)
	{
	case HD:
		return hdDensityValue;
	case FULL_HD:
		return fullHdDensityValue;
	case LD:
		return ldDensityValue;
	case XLD:
		return xldDensityValue;
	case NORMAL:
	default:
		return 1.0f;
	}
}
