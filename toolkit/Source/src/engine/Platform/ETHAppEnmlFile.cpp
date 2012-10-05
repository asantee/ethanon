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

#include "ETHAppEnmlFile.h"

#include <string.h>
#include "../ETHTypes.h"
#include "../Util/ETHASUtil.h"

using namespace gs2d;

ETHAppEnmlFile::ETHAppEnmlFile(const str_type::string& fileName, const Platform::FileManagerPtr& fileManager,
							   const gs2d::str_type::string& platformName) :
	hdDensityValue(2.0f),
	fullHdDensityValue(4.0f),
	width(640),
	height(480),
	windowed(true),
	vsync(true),
	title(GS_L("Ethanon Engine")),
	richLighting(true),
	minScreenHeightForHdVersion(720),
	minScreenHeightForFullHdVersion(1080)
{
	str_type::string out;
	fileManager->GetAnsiFileString(fileName, out);
	enml::File file(out);
	if (file.GetError() == enml::RV_SUCCESS)
	{
		LoadProperties(ETH_DEFAULT_PROPERTY_APP_ENML_ENTITY, file);

		// also read "window" entity for backwards project compatibility
		LoadProperties(ETH_OLDER_DEFAULT_PROPERTY_APP_ENML_ENTITY, file);

		// now read platform specific properties
		if (platformName != GS_L(""))
			LoadProperties(platformName, file);

		densityManager.FillParametersFromFile(*this);
	}
	else
	{
#		ifdef GS2D_STR_TYPE_WCHAR
			std::wcerr
#		else
			std::cerr
#		endif
			<< file.GetErrorString() << std::endl;
	}
}

// get a string value from enml and fill the variable only if it exists
static void GetString(const gs2d::enml::File& file, const str_type::string& platformName, const str_type::string& attrib, str_type::string& param)
{
	const str_type::string value = file.Get(platformName, attrib);
	if (value != GS_L(""))
		param = file.Get(platformName, attrib);
}

// get a boolean value from enml and fill the variable only if it exists
static void GetBoolean(const gs2d::enml::File& file, const str_type::string& platformName, const str_type::string& attrib, bool& param)
{
	const str_type::string value = file.Get(platformName, attrib);
	if (value != GS_L(""))
		param = ETHGlobal::IsTrue(file.Get(platformName, attrib));
}

void ETHAppEnmlFile::LoadProperties(const str_type::string& platformName, const gs2d::enml::File& file)
{
	if (!file.Exists(platformName))
		return;

	file.GetUInt(platformName, GS_L("width"), &width);
	file.GetUInt(platformName, GS_L("height"), &height);

	GetBoolean(file, platformName, GS_L("windowed"), windowed);
	GetBoolean(file, platformName, GS_L("vsync"), vsync);
	GetBoolean(file, platformName, GS_L("richLighting"), richLighting);

	GetString(file, platformName, GS_L("fixedWidth"), fixedWidth);
	GetString(file, platformName, GS_L("fixedHeight"), fixedHeight);

	file.GetFloat(platformName, GS_L("hdDensityValue"), &hdDensityValue);
	file.GetFloat(platformName, GS_L("fullHdDensityValue"), &fullHdDensityValue);
	file.GetUInt(platformName, GS_L("minScreenHeightForHdVersion"), &minScreenHeightForHdVersion);
	file.GetUInt(platformName, GS_L("minScreenHeightForFullHdVersion"), &minScreenHeightForFullHdVersion);

	const str_type::string newTitle = file.Get(platformName, GS_L("title"));
	if (!newTitle.empty())
		title = newTitle;

	std::vector<gs2d::str_type::string> words = ETHGlobal::SplitString(file.Get(platformName, GS_L("definedWords")), GS_L(","));
	definedWords.insert(definedWords.end(), words.begin(), words.end());
	std::sort(definedWords.begin(), definedWords.end());

	std::vector<gs2d::str_type::string>::iterator it = std::unique(definedWords.begin(), definedWords.end());
	definedWords.resize(it - definedWords.begin());
}

const std::vector<gs2d::str_type::string>& ETHAppEnmlFile::GetDefinedWords() const
{
	return definedWords;
}

const ETHSpriteDensityManager& ETHAppEnmlFile::GetDensityManager() const
{
	return densityManager;
}

unsigned int ETHAppEnmlFile::GetWidth() const
{
	return width;
}

unsigned int ETHAppEnmlFile::GetHeight() const
{
	return height;
}

float ETHAppEnmlFile::GetHdDensityValue() const
{
	return hdDensityValue;
}

float ETHAppEnmlFile::GetFullHdDensityValue() const
{
	return fullHdDensityValue;
}

unsigned int ETHAppEnmlFile::GetMinScreenHeightForHdVersion() const
{
	return minScreenHeightForHdVersion;
}

unsigned int ETHAppEnmlFile::GetMinScreenHeightForFullHdVersion() const
{
	return minScreenHeightForFullHdVersion;
}

bool ETHAppEnmlFile::IsWindowed() const
{
	return windowed;
}

bool ETHAppEnmlFile::IsVsyncEnabled() const
{
	return vsync;
}

bool ETHAppEnmlFile::IsRichLightingEnabled() const
{
	return richLighting;
}

str_type::string ETHAppEnmlFile::GetTitle() const
{
	return title;
}

str_type::string ETHAppEnmlFile::GetFixedWidth() const
{
	return fixedWidth;
}

str_type::string ETHAppEnmlFile::GetFixedHeight() const
{
	return fixedHeight;
}
