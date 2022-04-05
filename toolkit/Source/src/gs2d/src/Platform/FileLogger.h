#ifndef FILE_LOGGER_H_
#define FILE_LOGGER_H_

#include "Logger.h"

namespace Platform {

class FileLogger : public Logger
{
public:
	class ErrorRecorder
	{
	public:
		virtual void RecordError(const std::string& description) = 0;
		virtual void Log(const std::string& description) = 0;
	};
	typedef boost::shared_ptr<ErrorRecorder> ErrorRecorderPtr;

	FileLogger(const std::string& fileName);
	bool Log(const std::string& str, const TYPE& type) const;
	static std::string GetLogDirectory();

	static void SetErrorRecorder(const ErrorRecorderPtr& errorRecorder);

private:
	static std::string GetWarningLogFileDirectory();
	static std::string GetErrorLogFileDirectory();
	static void WriteToErrorLog(const std::string& str);
	static void WriteToWarningLog(const std::string& str);
	std::string m_fileName;

	static ErrorRecorderPtr m_errorRecorder;
};

typedef boost::shared_ptr<FileLogger> FileLoggerPtr;

} // namespace Platform

#endif
