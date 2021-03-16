#include "FileLogger.h"
#include "Platform.h"

#include <fstream>
#include <iostream>

#ifdef ANDROID
#include "android/AndroidLog.h"
#endif

namespace Platform {

bool AppendToFile(const std::string& fileName, const std::string& str)
{
	std::ofstream ofs(fileName.c_str(), std::ios::out | std::ios::app);
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

FileLogger::ErrorRecorderPtr FileLogger::m_errorRecorder;

FileLogger::FileLogger(const std::string& fileName) : m_fileName(fileName)
{
	std::ofstream ofs(fileName.c_str());
	if (ofs.is_open())
	{
		ofs << ("[") << fileName << ("]") << std::endl;
		ofs.close();
	}
}

void FileLogger::WriteToErrorLog(const std::string& str)
{
	static bool errorLogFileCreated = false;
	if (!errorLogFileCreated)
	{
		std::ofstream ofs(GetErrorLogFileDirectory().c_str());
		if (ofs.is_open())
		{
			ofs << ("[") << GetErrorLogFileDirectory() << ("]") << std::endl;
			ofs.close();
		}
	}
	AppendToFile(GetErrorLogFileDirectory(), str);
	errorLogFileCreated = true;
}

void FileLogger::WriteToWarningLog(const std::string& str)
{
	static bool warningLogFileCreated = false;
	if (!warningLogFileCreated)
	{
		std::ofstream ofs(GetWarningLogFileDirectory().c_str());
		if (ofs.is_open())
		{
			ofs << ("[") << GetWarningLogFileDirectory() << ("]") << std::endl;
			ofs.close();
		}
	}
	AppendToFile(GetWarningLogFileDirectory(), str);
	warningLogFileCreated = true;
}

bool FileLogger::Log(const std::string& str, const TYPE& type) const
{
	#if defined(_WIN32) || defined(APPLE_IOS) || defined(MACOSX)
		std::cout << str << std::endl;
	#endif

	if (type == ERROR)
	{
		WriteToErrorLog(str);

		if (m_errorRecorder)
		{
			m_errorRecorder->RecordError(str);
		}
	}
	else if (type == WARNING)
	{
		WriteToWarningLog(str);
	}

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

void FileLogger::SetErrorRecorder(const ErrorRecorderPtr &errorRecorder)
{
	m_errorRecorder = errorRecorder;
}

std::string FileLogger::GetErrorLogFileDirectory()
{
	return GetLogDirectory() + ("_error.log.txt");
}

std::string FileLogger::GetWarningLogFileDirectory()
{
	return GetLogDirectory() + ("_warning.log.txt");
}

} // namespace Platform
