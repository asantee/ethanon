#include "ETHScriptWrapper.h"

#include "../Entity/ETHRenderEntity.h"

#include "../Platform/ETHAppEnmlFile.h"

#include <Platform/StdFileManager.h>

#if defined(_MSC_VER) || defined(ANDROID)
 #include <Platform/ZipFileManager.h>
#endif

#define UNUSED_ARGUMENT(argument) ((void)(argument))

void ETHScriptWrapper::TestCrash()
{
	int* crashMePlease = nullptr;
	*crashMePlease = 42;
}

void ETHScriptWrapper::ShowMessage(std::string sMsg, const ETH_MESSAGE type, const bool abort)
{
	std::stringstream ss;
	Platform::Logger::TYPE logType;
	switch (type)
	{
	case ETH_ERROR:
		ss << ("ERROR - ");
		logType = Platform::Logger::LT_ERROR;
		if (abort)
		{
			Abort();
		}
		break;
	case ETH_WARNING:
		ss << ("Warning - ");
		logType = Platform::Logger::LT_WARNING;
		break;
	default:
		logType = Platform::Logger::LT_INFO;
	};
	ss << sMsg;
	m_provider->Log(ss.str(), logType);
}

void ETHScriptWrapper::ForwardCommand(const std::string& cmd)
{
	m_provider->GetVideo()->ForwardCommand(cmd);
}

void ETHScriptWrapper::PrintFloat(const float f)
{
	ETH_STREAM_DECL(ss) << f;
	m_provider->Log(ss.str(), Platform::FileLogger::LT_INFO);
}

void ETHScriptWrapper::PrintInt(const int n)
{
	ETH_STREAM_DECL(ss) << n;
	m_provider->Log(ss.str(), Platform::FileLogger::LT_INFO);
}

void ETHScriptWrapper::PrintUInt(const unsigned int n)
{
	ETH_STREAM_DECL(ss) << n;
	m_provider->Log(ss.str(), Platform::FileLogger::LT_INFO);
}

void ETHScriptWrapper::Print(const std::string &str)
{
	ETH_STREAM_DECL(ss) << str;
	m_provider->Log(ss.str(), Platform::FileLogger::LT_INFO);
}

float ETHScriptWrapper::GetTimeF()
{
	return m_provider->GetVideo()->GetElapsedTimeF();
}

unsigned long ETHScriptWrapper::GetTime()
{
	return m_provider->GetVideo()->GetElapsedTime();
}

float ETHScriptWrapper::UnitsPerSecond(const float speed)
{
	return (m_lastFrameElapsedTime / 1000.0f) * speed;
}

void ETHScriptWrapper::SetLastFrameElapsedTime(const float lastFrameElapsedTime)
{
	m_lastFrameElapsedTime = lastFrameElapsedTime;
}

unsigned long ETHScriptWrapper::GetLastFrameElapsedTime()
{
	return static_cast<unsigned long>(m_lastFrameElapsedTime);
}

float ETHScriptWrapper::GetLastFrameElapsedTimeF()
{
	return m_lastFrameElapsedTime;
}

void ETHScriptWrapper::Exit()
{
	m_provider->GetVideo()->Quit();
}

std::string ETHScriptWrapper::GetCurrentCallstack()
{
	if (!m_pScriptContext)
	{
		return "";
	}

	std::stringstream ss;
	for (std::size_t n = 0; n < m_pScriptContext->GetCallstackSize(); n++)
	{
		asIScriptFunction* stackedFunction = m_pScriptContext->GetFunction(static_cast<asUINT>(n));
		if (stackedFunction != NULL)
		{
			const std::string section = (stackedFunction->GetScriptSectionName());
			ss << ("    ") << stackedFunction->GetDeclaration()
			   << (" (") << section << (", ") << m_pScriptContext->GetLineNumber(static_cast<asUINT>(n)) << (")") << std::endl;
		}
	}
	return ss.str();
}

unsigned int ETHScriptWrapper::GetScreenWidth()
{
	return static_cast<unsigned int>(GetScreenSize().x);
}

unsigned int ETHScriptWrapper::GetScreenHeight()
{
	return static_cast<unsigned int>(GetScreenSize().y);
}

Vector2 ETHScriptWrapper::GetScreenSize()
{
	return m_provider->GetVideo()->GetScreenSizeF();
}

Vector2 ETHScriptWrapper::GetScreenSizeInPixels()
{
	return m_provider->GetVideo()->GetScreenSizeInPixels();
}

float ETHScriptWrapper::GetFPSRate()
{
	return m_provider->GetVideo()->GetFPSRate();
}

void ETHScriptWrapper::HideCursor(const bool hide)
{
	m_provider->GetVideo()->HideCursor(hide);
}

bool ETHScriptWrapper::Windowed()
{
	return m_provider->GetVideo()->IsWindowed();
}

ETHScriptWrapper::ETH_VIDEO_MODE ETHScriptWrapper::GetVideoMode(const unsigned int nMode)
{
	const Video::VIDEO_MODE gsMode = m_provider->GetVideo()->GetVideoMode(nMode);
	ETH_VIDEO_MODE mode;
	if (gsMode.height > 0)
	{
		mode.height = gsMode.height;
		mode.width  = gsMode.width;
		mode.pf = gsMode.pf;
	}
	return mode;
}

unsigned int ETHScriptWrapper::GetVideoModeCount()
{
	return m_provider->GetVideo()->GetVideoModeCount();
}

Vector2 ETHScriptWrapper::GetSystemScreenSize()
{
	const Vector2i v2 = m_provider->GetVideo()->GetClientScreenSize();
	return Vector2((float)v2.x, (float)v2.y);
}

int ETHScriptWrapper::GetArgc()
{
	return m_argc;
}

std::string ETHScriptWrapper::GetStringFromFileInPackage(const std::string& fileName)
{
	std::string out;
	m_provider->GetFileManager()->GetAnsiFileString(fileName, out);
	return out;
}

bool ETHScriptWrapper::FileInPackageExists(const std::string& fileName)
{
	return m_provider->GetFileManager()->FileExists(fileName);
}

void ETHScriptWrapper::SetAppDefaultVideoMode(const Vector2& size, const bool windowed)
{
	ETHAppEnmlFile::SetAppDefaultVideoMode(size, windowed, GetProvider()->GetFileIOHub()->GetExternalStorageDirectory());
}

Vector2 ETHScriptWrapper::GetAppDefaultVideoMode(bool& windowed)
{
	return ETHAppEnmlFile::GetAppDefaultVideoMode(windowed, GetProvider()->GetFileIOHub()->GetExternalStorageDirectory());
}

bool ETHScriptWrapper::FileExists(const std::string& fileName)
{
	std::ifstream ifs(fileName.c_str());
	if (ifs.is_open())
	{
		ifs.close();
		return true;
	}
	else
	{
		return false;
	}
}

std::string ETHScriptWrapper::GetArgv(const int n)
{
	if (n >= m_argc || n < 0)
	{
		return ("");
	}
	return m_argv[n];
}

std::string ETHScriptWrapper::GetResourceDirectory()
{
	return m_provider->GetFileIOHub()->GetResourceDirectory();
}

std::string ETHScriptWrapper::GetAbsolutePath(const std::string &fileName)
{
	return GetResourceDirectory() + fileName;
}

std::string ETHScriptWrapper::GetExternalStorageDirectory()
{
	return m_provider->GetFileIOHub()->GetExternalStorageDirectory();
}

std::string ETHScriptWrapper::GetGlobalExternalStorageDirectory()
{
	return m_provider->GetFileIOHub()->GetGlobalExternalStorageDirectory();
}

void ETHScriptWrapper::ResetVideoMode(
	const std::string& winTitle,
	const unsigned int width,
	const unsigned int height,
	const bool windowed,
	const bool sync,
	const Texture::PIXEL_FORMAT gsPF)
{
	UNUSED_ARGUMENT(sync);
	const VideoPtr& video = m_provider->GetVideo();
	if (video)
	{
		const bool toggle = (video->IsWindowed() != windowed);
		video->ResetVideoMode(width, height, gsPF, toggle);
		video->SetWindowTitle(winTitle);
		if (windowed)
		{
			Vector2i v2Backbuffer(static_cast<int>(width), static_cast<int>(height));
			Vector2i v2Screen = video->GetClientScreenSize();
			video->SetWindowPosition(v2Screen/2-v2Backbuffer/2);
		}
		ETHAppEnmlFile file(
			m_provider->GetFileIOHub()->GetResourceDirectory() + ETH_APP_PROPERTIES_FILE,
			m_provider->GetFileManager(),
			video->GetPlatformName(),
			m_provider->GetFileIOHub()->GetExternalStorageDirectory(),
			GetSharedData("ethanon.system.isLowRamDevice") == "true");

		UpdateFixedHeight();

		CreateDynamicBackBuffer(file);
	}
}

std::string ETHScriptWrapper::GetPlatformName()
{
	return m_provider->GetVideo()->GetPlatformName();
}
