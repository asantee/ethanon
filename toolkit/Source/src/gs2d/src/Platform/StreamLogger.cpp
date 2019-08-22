#include "StreamLogger.h"

#include <iostream>

namespace Platform {

StreamLogger::StreamLogger(const std::string& fileName) :
	FileLogger(fileName)
{
}

bool StreamLogger::Log(const std::string& str, const TYPE& type) const
{
	switch (type)
	{
	case ERROR:
		std::cerr << "ERROR: " << str << std::endl;
		break;
	case WARNING:
		std::cout << "WARNING: " << str << std::endl;
		break;
	default:
		std::cout << "INFO: " << str << std::endl;
		break;
	};
	return FileLogger::Log(str, type);
}

} // namespace Platform
