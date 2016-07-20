/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#include "../ETHTypes.h"

#include "../Util/ETHASUtil.h"

#include <Platform/Platform.h>

#include <boost/lexical_cast.hpp>

using namespace gs2d;

ETHAppEnmlFile::ETHAppEnmlFile(
	const str_type::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const gs2d::str_type::string& platformName,
	const gs2d::str_type::string& externalStorageDirectory) :
	hdDensityValue(2.0f),
	fullHdDensityValue(4.0f),
	ldDensityValue(0.5f),
	xldDensityValue(0.25f),
	width(640),
	height(480),
	windowed(true),
	vsync(true),
	title(GS_L("Ethanon Engine")),
	richLighting(true),
	minScreenHeightForHdVersion(720),
	minScreenHeightForFullHdVersion(1080),
	maxScreenHeightBeforeNdVersion(480),
	maxScreenHeightBeforeLdVersion(320),
	appDefaultWindowedMode(false)
{
	appDefaultVideoMode = GetAppDefaultVideoMode(appDefaultWindowedMode, externalStorageDirectory);

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
		GS2D_CERR << file.GetErrorString() << std::endl;
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

void ETHAppEnmlFile::SetAppDefaultVideoMode(
	const Vector2& size,
	const bool windowed,
	const gs2d::str_type::string& externalStorageDirectory)
{
	enml::File file;
	{
		str_type::stringstream ss; ss << static_cast<unsigned int>(size.x);
		file.Add("default", "width", ss.str());
	}
	{
		str_type::stringstream ss; ss << static_cast<unsigned int>(size.y);
		file.Add("default", "height", ss.str());
	}

	file.Add("default", "windowed", windowed ? "true" : "false");

	enml::SaveStringToAnsiFile(
		externalStorageDirectory + "videoMode.enml",
		file.GenerateString());
}

Vector2 ETHAppEnmlFile::GetAppDefaultVideoMode(
	bool& windowed,
	const gs2d::str_type::string& externalStorageDirectory)
{
	const str_type::string content = enml::GetStringFromAnsiFile(externalStorageDirectory + "videoMode.enml");
	if (content.empty())
	{
		return Vector2(0.0f, 0.0f);
	}
	else
	{
		enml::File file(content);

		unsigned int width = 0;
		file.GetUInt("default", "width", &width);

		unsigned int height = 0;
		file.GetUInt("default", "height", &height);

		GetBoolean(file, "default", "windowed", windowed);

		return Vector2(static_cast<float>(width), static_cast<float>(height));
	}
}

static void GetScreenDimension(
	const unsigned int defaultLength,
	const gs2d::enml::File& file,
	const str_type::string& platformName,
	const str_type::string& attrib,
	unsigned int& param)
{
	const str_type::string value = file.Get(platformName, attrib);

	// if there's no value, we'll keep the default value
	if (value.empty())
	{
		return;
	}

	if (value == GS_L("default"))
	{
		param = defaultLength;
		return;
	}

	try
	{
		param = boost::lexical_cast<unsigned int>(value);
	}
	catch (boost::bad_lexical_cast& exception)
	{
		GS2D_UNUSED_ARGUMENT(exception);
		param = 0;
	}
}

void ETHAppEnmlFile::LoadProperties(
	const str_type::string& platformName,
	const gs2d::enml::File& file)
{
	if (!file.Exists(platformName))
		return;

	GetScreenDimension(static_cast<unsigned int>(appDefaultVideoMode.x), file, platformName, GS_L("width"),  width);
	GetScreenDimension(static_cast<unsigned int>(appDefaultVideoMode.y), file, platformName, GS_L("height"), height);

	if (file.Get(platformName, GS_L("windowed")) == GS_L("default"))
		windowed = appDefaultWindowedMode;
	else
		GetBoolean(file, platformName, GS_L("windowed"), windowed);

	GetBoolean(file, platformName, GS_L("vsync"), vsync);
	GetBoolean(file, platformName, GS_L("richLighting"), richLighting);

	GetString(file, platformName, GS_L("fixedWidth"), fixedWidth);
	GetString(file, platformName, GS_L("fixedHeight"), fixedHeight);

	file.GetFloat(platformName, GS_L("hdDensityValue"), &hdDensityValue);
	file.GetFloat(platformName, GS_L("fullHdDensityValue"), &fullHdDensityValue);
	
	file.GetFloat(platformName, GS_L("ldDensityValue"), &ldDensityValue);
	file.GetFloat(platformName, GS_L("xldDensityValue"), &xldDensityValue);

	file.GetUInt(platformName, GS_L("minScreenHeightForHdVersion"), &minScreenHeightForHdVersion);
	file.GetUInt(platformName, GS_L("minScreenHeightForFullHdVersion"), &minScreenHeightForFullHdVersion);
	
	file.GetUInt(platformName, GS_L("maxScreenHeightBeforeNdVersion"), &maxScreenHeightBeforeNdVersion);
	file.GetUInt(platformName, GS_L("maxScreenHeightBeforeLdVersion"), &maxScreenHeightBeforeLdVersion);

	const str_type::string newTitle = file.Get(platformName, GS_L("title"));
	if (!newTitle.empty())
		title = newTitle;

	std::vector<gs2d::str_type::string> words = Platform::SplitString(file.Get(platformName, GS_L("definedWords")), GS_L(","));
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

float ETHAppEnmlFile::GetLdDensityValue() const
{
	return ldDensityValue;
}

float ETHAppEnmlFile::GetXldDensityValue() const
{
	return xldDensityValue;
}

unsigned int ETHAppEnmlFile::GetMinScreenHeightForHdVersion() const
{
	return minScreenHeightForHdVersion;
}

unsigned int ETHAppEnmlFile::GetMinScreenHeightForFullHdVersion() const
{
	return minScreenHeightForFullHdVersion;
}

unsigned int ETHAppEnmlFile::GetMaxScreenHeightBeforeNdVersion() const
{
	return maxScreenHeightBeforeNdVersion;
}

unsigned int ETHAppEnmlFile::GetMaxScreenHeightBeforeLdVersion() const
{
	return maxScreenHeightBeforeLdVersion;
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
