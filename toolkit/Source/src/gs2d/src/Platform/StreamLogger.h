#ifndef STREAM_LOGGER_H_
#define STREAM_LOGGER_H_

#include "FileLogger.h"

namespace Platform {

class StreamLogger : public FileLogger
{
public:
	StreamLogger(const std::string& fileName);
	bool Log(const std::string& str, const TYPE& type) const;
};

typedef boost::shared_ptr<FileLogger> FileLoggerPtr;

} // namespace Platform

#endif
