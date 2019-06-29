#ifndef FILE_LOGGER_H_
#define FILE_LOGGER_H_

#include "Logger.h"

namespace Platform {

class FileLogger : public Logger
{
public:
	FileLogger(const gs2d::str_type::string& fileName);
	bool Log(const gs2d::str_type::string& str, const TYPE& type) const;
	static gs2d::str_type::string GetLogDirectory();
private:
	static gs2d::str_type::string GetWarningLogFileDirectory();
	static gs2d::str_type::string GetErrorLogFileDirectory();
	static void WriteToErrorLog(const gs2d::str_type::string& str);
	static void WriteToWarningLog(const gs2d::str_type::string& str);
	gs2d::str_type::string m_fileName;
};

typedef boost::shared_ptr<FileLogger> FileLoggerPtr;

} // namespace Platform

#endif
