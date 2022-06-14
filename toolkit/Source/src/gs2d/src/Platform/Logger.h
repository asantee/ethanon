#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>

#include <boost/shared_ptr.hpp>

namespace Platform {

class Logger
{
public:
	enum TYPE
	{
		LT_ERROR = 0,
		LT_WARNING = 1,
		LT_INFO = 2
	};
	virtual bool Log(const std::string& str, const TYPE& type) const = 0;
};

typedef boost::shared_ptr<Logger> LoggerPtr;

} // namespace Platform

#endif
