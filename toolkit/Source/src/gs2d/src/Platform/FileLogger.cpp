#include "FileLogger.h"
#include "Platform.h"

#include <fstream>
#include <iostream>

#ifdef ANDROID
#include "android/AndroidLog.h"
#endif

namespace Platform {

bool AppendToFile(const gs2d::str_type::string& fileName, const gs2d::str_type::string& str)
{
	gs2d::str_type::ofstream ofs(fileName.c_str(), std::ios::out | std::ios::app);
	if (ofs.is_open())
	{
		ofs << str << std::endl;
		ofs.close();
		return true;
	}
	else
	{
		return false;
	}
}

FileLogger::FileLogger(const gs2d::str_type::string& fileName) : m_fileName(fileName)
{
	gs2d::str_type::ofstream ofs(fileName.c_str());
	if (ofs.is_open())
	{
		ofs << GS_L("[") << fileName << GS_L("]") << std::endl;
		ofs.close();
	}
}

void FileLogger::WriteToErrorLog(const gs2d::str_type::string& str)
{
	static bool errorLogFileCreated = false;
	if (!errorLogFileCreated)
	{
		gs2d::str_type::ofstream ofs(GetErrorLogFileDirectory().c_str());
		if (ofs.is_open())
		{
			ofs << GS_L("[") << GetErrorLogFileDirectory() << GS_L("]") << std::endl;
			ofs.close();
		}
	}
	AppendToFile(GetErrorLogFileDirectory(), str);
	errorLogFileCreated = true;
}

void FileLogger::WriteToWarningLog(const gs2d::str_type::string& str)
{
	static bool warningLogFileCreated = false;
	if (!warningLogFileCreated)
	{
		gs2d::str_type::ofstream ofs(GetWarningLogFileDirectory().c_str());
		if (ofs.is_open())
		{
			ofs << GS_L("[") << GetWarningLogFileDirectory() << GS_L("]") << std::endl;
			ofs.close();
		}
	}
	AppendToFile(GetWarningLogFileDirectory(), str);
	warningLogFileCreated = true;
}

bool FileLogger::Log(const gs2d::str_type::string& str, const TYPE& type) const
{
	#if defined(WIN32) || defined(APPLE_IOS) || defined(MACOSX)
	GS2D_COUT << str << std::endl;
	#endif

	if (type == ERROR)
		WriteToErrorLog(str);
	else if (type == WARNING)
		WriteToWarningLog(str);

	#ifdef ANDROID
		switch (type)
		{
		case ERROR:
		case WARNING:
			LOGE("%s", str.c_str());
			break;
		default:
			LOGI("%s", str.c_str());
		}
	#endif
	return AppendToFile(m_fileName, str);
}

gs2d::str_type::string FileLogger::GetErrorLogFileDirectory()
{
	return GetLogDirectory() + GS_L("_error.log.txt");
}

gs2d::str_type::string FileLogger::GetWarningLogFileDirectory()
{
	return GetLogDirectory() + GS_L("_warning.log.txt");
}

} // namespace Platform
