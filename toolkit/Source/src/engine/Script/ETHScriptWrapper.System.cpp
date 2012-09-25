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

#include "ETHScriptWrapper.h"
#include "../Entity/ETHRenderEntity.h"
#include <Platform/ZipFileManager.h>
#include <Platform/StdFileManager.h>

Platform::FileManagerPtr ETHScriptWrapper::m_primaryFileManager;
str_type::string         ETHScriptWrapper::m_primaryResourcePath;

void ETHScriptWrapper::ShowMessage(str_type::string sMsg, const ETH_MESSAGE type, const bool abort)
{
	str_type::stringstream ss;
	Platform::Logger::TYPE logType;
	switch (type)
	{
	case ETH_ERROR:
		ss << GS_L("ERROR - ");
		logType = Platform::Logger::ERROR;
		if (abort)
		{
			Abort();
		}
		break;
	case ETH_WARNING:
		ss << GS_L("Warning - ");
		logType = Platform::Logger::WARNING;
		break;
	default:
		logType = Platform::Logger::INFO;
	};
	ss << sMsg;
	m_provider->Log(ss.str(), logType);
}

void ETHScriptWrapper::ForwardCommand(const str_type::string& cmd)
{
	m_provider->GetVideo()->ForwardCommand(cmd);
}

void ETHScriptWrapper::PrintFloat(const float f)
{
	ETH_STREAM_DECL(ss) << f;
	m_provider->Log(ss.str(), Platform::FileLogger::INFO);
}

void ETHScriptWrapper::PrintInt(const int n)
{
	ETH_STREAM_DECL(ss) << n;
	m_provider->Log(ss.str(), Platform::FileLogger::INFO);
}

void ETHScriptWrapper::PrintUInt(const unsigned int n)
{
	ETH_STREAM_DECL(ss) << n;
	m_provider->Log(ss.str(), Platform::FileLogger::INFO);
}

void ETHScriptWrapper::Print(const str_type::string &str)
{
	ETH_STREAM_DECL(ss) << str;
	m_provider->Log(ss.str(), Platform::FileLogger::INFO);
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
	return static_cast<float>(static_cast<double>(m_lastFrameElapsedTime) / 1000.0 * static_cast<double>(speed));
}

void ETHScriptWrapper::SetLastFrameElapsedTime(const unsigned long lastFrameElapsedTime)
{
	m_lastFrameElapsedTime = lastFrameElapsedTime;
}

unsigned long ETHScriptWrapper::GetLastFrameElapsedTime()
{
	return m_lastFrameElapsedTime;
}

void ETHScriptWrapper::Exit()
{
	m_provider->GetVideo()->Quit();
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
	return m_backBuffer->GetBufferSize();
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

ETH_VIDEO_MODE ETHScriptWrapper::GetVideoMode(const unsigned int nMode)
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

str_type::string ETHScriptWrapper::GetStringFromFileInPackage(const str_type::string& fileName)
{
	str_type::string out;
	m_provider->GetVideo()->GetFileManager()->GetAnsiFileString(fileName, out);
	return out;
}

bool ETHScriptWrapper::FileInPackageExists(const str_type::string& fileName)
{
	return m_provider->GetVideo()->GetFileManager()->FileExists(fileName);
}

bool ETHScriptWrapper::FileExists(const str_type::string& fileName)
{
	str_type::ifstream ifs(fileName.c_str());
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

str_type::string ETHScriptWrapper::GetArgv(const int n)
{
	if (n >= m_argc || n < 0)
	{
		return GS_L("");
	}
	return m_argv[n];
}

str_type::string ETHScriptWrapper::GetProgramPath()
{
	return m_provider->GetResourcePath();
}

str_type::string ETHScriptWrapper::GetAbsolutePath(const str_type::string &fileName)
{
	return GetProgramPath() + fileName;
}

str_type::string ETHScriptWrapper::GetExternalStoragePath()
{
	return m_provider->GetVideo()->GetExternalStoragePath();
}

str_type::string ETHScriptWrapper::GetGlobalExternalStoragePath()
{
	return m_provider->GetVideo()->GetGlobalExternalStoragePath();
}

void ETHScriptWrapper::EnableQuitKeys(const bool enable)
{
	m_provider->GetVideo()->EnableQuitShortcuts(enable);
}

void ETHScriptWrapper::SetWindowProperties(const str_type::string &winTitle, const unsigned int width, const unsigned int height,
										   const bool windowed, const bool sync, const GS_PIXEL_FORMAT gsPF)
{
	GS2D_UNUSED_ARGUMENT(sync);
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
		ETHAppEnmlFile file(m_provider->GetResourcePath() + ETH_APP_PROPERTIES_FILE, video->GetFileManager(), video->GetPlatformName());
		CreateDynamicBackBuffer(file);
	}
}

str_type::string ETHScriptWrapper::GetPlatformName()
{
	return m_provider->GetVideo()->GetPlatformName();
}

bool ETHScriptWrapper::EnablePackLoading(const str_type::string& packFileName, const str_type::string& password)
{
	if (IsResourcePackingSupported())
	{
		VideoPtr video = m_provider->GetVideo();
		const str_type::string packagePath = m_provider->GetResourcePath() + packFileName;
		Platform::FileManagerPtr fileManager(new Platform::ZipFileManager(packagePath.c_str(), password.c_str()));
		const bool isLoaded = fileManager->IsLoaded();
		if (isLoaded)
		{
			// if m_primaryFileManager isn't set yet, then it means the Application
			// object still holds the primary one, so we save it in case we need it later
			if (!m_primaryFileManager)
			{
				m_primaryFileManager  = video->GetFileManager();
				m_primaryResourcePath = m_provider->GetResourcePath();
			}
			video->SetFileManager(fileManager);
			m_provider->SetResourcePath(GS_L(""));
		}
		return isLoaded;
	}
	else
	{
		return false;
	}
}

void ETHScriptWrapper::DisablePackLoading()
{
	// if m_primaryFileManager is valid, it means the primary file manager has been changed,
	// so we go back to it
	if (m_primaryFileManager)
	{
		m_provider->GetVideo()->SetFileManager(m_primaryFileManager);
		m_provider->SetResourcePath(m_primaryResourcePath);
	}
}

bool ETHScriptWrapper::IsResourcePackingSupported()
{
	const str_type::string& platformName = m_provider->GetVideo()->GetPlatformName();
	if (platformName == GS_L("windows") || platformName == GS_L("mac"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ETHScriptWrapper::IsPackLoadingEnabled()
{
	return (m_provider->GetVideo()->GetFileManager()->IsPacked() && m_primaryFileManager);
}
