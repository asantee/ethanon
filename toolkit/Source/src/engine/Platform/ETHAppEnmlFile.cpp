#include "ETHAppEnmlFile.h"

#include "../ETHTypes.h"

#include "../Util/ETHASUtil.h"

#include <Platform/Platform.h>

#include <boost/lexical_cast.hpp>

using namespace gs2d;

#define UNUSED_ARGUMENT(argument) ((void)(argument))

ETHAppEnmlFile::ETHAppEnmlFile(
	const std::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const std::string& platformName,
	const std::string& externalStorageDirectory) :
	hdDensityValue(2.0f),
	fullHdDensityValue(4.0f),
	ldDensityValue(0.5f),
	xldDensityValue(0.25f),
	width(640),
	height(480),
	windowed(true),
	vsync(true),
	title(("Ethanon Engine")),
	richLighting(true),
	minScreenHeightForHdVersion(720),
	minScreenHeightForFullHdVersion(1080),
	maxScreenHeightBeforeNdVersion(480),
	maxScreenHeightBeforeLdVersion(320),
	appDefaultWindowedMode(false)
{
	appDefaultVideoMode = GetAppDefaultVideoMode(appDefaultWindowedMode, externalStorageDirectory);

	std::string out;
	fileManager->GetAnsiFileString(fileName, out);

	enml::File file(out);

	if (file.GetError() == enml::RV_SUCCESS)
	{
		LoadProperties(ETH_DEFAULT_PROPERTY_APP_ENML_ENTITY, file);

		// also read "window" entity for backwards project compatibility
		LoadProperties(ETH_OLDER_DEFAULT_PROPERTY_APP_ENML_ENTITY, file);

		// now read platform specific properties
		if (platformName != (""))
			LoadProperties(platformName, file);

		densityManager.FillParametersFromFile(*this);
	}
	else
	{
		std::cerr << file.GetErrorString() << std::endl;
	}
}

// get a string value from enml and fill the variable only if it exists
static void GetString(const gs2d::enml::File& file, const std::string& platformName, const std::string& attrib, std::string& param)
{
	const std::string value = file.Get(platformName, attrib);
	if (value != (""))
		param = file.Get(platformName, attrib);
}

// get a boolean value from enml and fill the variable only if it exists
static void GetBoolean(const gs2d::enml::File& file, const std::string& platformName, const std::string& attrib, bool& param)
{
	const std::string value = file.Get(platformName, attrib);
	if (value != (""))
		param = ETHGlobal::IsTrue(file.Get(platformName, attrib));
}

void ETHAppEnmlFile::SetAppDefaultVideoMode(
	const Vector2& size,
	const bool windowed,
	const std::string& externalStorageDirectory)
{
	enml::File file;
	{
		std::stringstream ss; ss << static_cast<unsigned int>(size.x);
		file.Add("default", "width", ss.str());
	}
	{
		std::stringstream ss; ss << static_cast<unsigned int>(size.y);
		file.Add("default", "height", ss.str());
	}

	file.Add("default", "windowed", windowed ? "true" : "false");

	enml::SaveStringToAnsiFile(
		externalStorageDirectory + "videoMode.enml",
		file.GenerateString());
}

Vector2 ETHAppEnmlFile::GetAppDefaultVideoMode(
	bool& windowed,
	const std::string& externalStorageDirectory)
{
	const std::string content = enml::GetStringFromAnsiFile(externalStorageDirectory + "videoMode.enml");
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
	const std::string& platformName,
	const std::string& attrib,
	unsigned int& param)
{
	const std::string value = file.Get(platformName, attrib);

	// if there's no value, we'll keep the default value
	if (value.empty())
	{
		return;
	}

	if (value == ("default"))
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
		UNUSED_ARGUMENT(exception);
		param = 0;
	}
}

void ETHAppEnmlFile::LoadProperties(
	const std::string& platformName,
	const gs2d::enml::File& file)
{
	if (!file.Exists(platformName))
		return;

	GetScreenDimension(static_cast<unsigned int>(appDefaultVideoMode.x), file, platformName, ("width"),  width);
	GetScreenDimension(static_cast<unsigned int>(appDefaultVideoMode.y), file, platformName, ("height"), height);

	if (file.Get(platformName, ("windowed")) == ("default"))
		windowed = appDefaultWindowedMode;
	else
		GetBoolean(file, platformName, ("windowed"), windowed);

	GetBoolean(file, platformName, ("vsync"), vsync);
	GetBoolean(file, platformName, ("richLighting"), richLighting);

	GetString(file, platformName, ("fixedWidth"), fixedWidth);
	GetString(file, platformName, ("fixedHeight"), fixedHeight);

	file.GetFloat(platformName, ("hdDensityValue"), &hdDensityValue);
	file.GetFloat(platformName, ("fullHdDensityValue"), &fullHdDensityValue);
	
	file.GetFloat(platformName, ("ldDensityValue"), &ldDensityValue);
	file.GetFloat(platformName, ("xldDensityValue"), &xldDensityValue);

	file.GetUInt(platformName, ("minScreenHeightForHdVersion"), &minScreenHeightForHdVersion);
	file.GetUInt(platformName, ("minScreenHeightForFullHdVersion"), &minScreenHeightForFullHdVersion);
	
	file.GetUInt(platformName, ("maxScreenHeightBeforeNdVersion"), &maxScreenHeightBeforeNdVersion);
	file.GetUInt(platformName, ("maxScreenHeightBeforeLdVersion"), &maxScreenHeightBeforeLdVersion);

	const std::string newTitle = file.Get(platformName, ("title"));
	if (!newTitle.empty())
		title = newTitle;

	std::vector<std::string> words = Platform::SplitString(file.Get(platformName, ("definedWords")), (","));
	definedWords.insert(definedWords.end(), words.begin(), words.end());
	std::sort(definedWords.begin(), definedWords.end());

	std::vector<std::string>::iterator it = std::unique(definedWords.begin(), definedWords.end());
	definedWords.resize(it - definedWords.begin());
}

const std::vector<std::string>& ETHAppEnmlFile::GetDefinedWords() const
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

std::string ETHAppEnmlFile::GetTitle() const
{
	return title;
}

std::string ETHAppEnmlFile::GetFixedWidth() const
{
	return fixedWidth;
}

std::string ETHAppEnmlFile::GetFixedHeight() const
{
	return fixedHeight;
}
