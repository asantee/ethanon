/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#include "ETHSpriteDensityManager.h"
#include "../ETHTypes.h"
#include "../Platform/ETHAppEnmlFile.h"
#include <Platform/Platform.h>

using gs2d::str_type::string;

const gs2d::str_type::string ETHSpriteDensityManager::HD_VERSION_PATH_NAME = GS_L("hd/");
const gs2d::str_type::string ETHSpriteDensityManager::FULL_HD_VERSION_PATH_NAME = GS_L("fullhd/");

ETHSpriteDensityManager::ETHSpriteDensityManager() :
	hdDensityValue(2.0f),
	fullHdDensityValue(4.0f),
	minScreenHeightForHdResources(720),
	minScreenHeightForFullHdResources(1080)
{
}

void ETHSpriteDensityManager::FillParametersFromFile(const ETHAppEnmlFile& file)
{
	hdDensityValue = file.GetHdDensityValue();
	fullHdDensityValue = file.GetFullHdDensityValue();
	minScreenHeightForHdResources = file.GetMinScreenHeightForHdVersion();
	minScreenHeightForFullHdResources = file.GetMinScreenHeightForFullHdVersion();
}

bool ETHSpriteDensityManager::ShouldUseHdResources(const gs2d::VideoPtr& video) const
{
	return (video->GetScreenSize().y >= static_cast<int>(minScreenHeightForHdResources)) && !ShouldUseFullHdResources(video);
}

bool ETHSpriteDensityManager::ShouldUseFullHdResources(const gs2d::VideoPtr& video) const
{
	return (video->GetScreenSize().y >= static_cast<int>(minScreenHeightForFullHdResources));
}

static gs2d::str_type::string AssembleResourceName(const string& fullFilePath, const string& versionPathName)
{
	const string folder(Platform::GetFileDirectory(fullFilePath.c_str()));
	const string file(Platform::GetFileName(fullFilePath));
	return (folder + versionPathName + file);
}

gs2d::str_type::string ETHSpriteDensityManager::ChooseSpriteVersion(const gs2d::str_type::string& fullFilePath,
										   const gs2d::VideoPtr& video, ETHSpriteDensityManager::DENSITY_LEVEL& densityLevel)
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

	densityLevel = ETHSpriteDensityManager::DEFAULT;
	return fullFilePath;
}

void ETHSpriteDensityManager::SetSpriteDensity(const gs2d::SpritePtr& sprite, const DENSITY_LEVEL& level) const
{
	sprite->SetSpriteDensityValue(GetDensityValue(level));
}

float ETHSpriteDensityManager::GetDensityValue(const DENSITY_LEVEL& level) const
{
	switch(level)
	{
	case HD:
		return hdDensityValue;
	case FULL_HD:
		return fullHdDensityValue;
	case DEFAULT:
	default:
		return 1.0f;
	}
}
