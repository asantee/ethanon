#ifndef PLATFORM_NATIVE_COMMAND_LISTENR_H_
#define PLATFORM_NATIVE_COMMAND_LISTENR_H_

#include <string>

#include <boost/shared_ptr.hpp>

namespace Platform {

class NativeCommandListener
{
public:
	virtual bool ExecuteCommand(const std::string& command) = 0;
};

typedef boost::shared_ptr<NativeCommandListener> NativeCommandListenerPtr;

} // namespace Platform

#endif
