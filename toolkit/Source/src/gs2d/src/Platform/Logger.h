#ifndef LOGGER_H_
#define LOGGER_H_

#include "../Types.h"

namespace Platform {

class Logger
{
public:
	enum TYPE
	{
		ERROR = 0,
		WARNING = 1,
		INFO = 2
	};
	virtual bool Log(const gs2d::str_type::string& str, const TYPE& type) const = 0;
};

typedef boost::shared_ptr<Logger> LoggerPtr;

} // namespace Platform

#endif
