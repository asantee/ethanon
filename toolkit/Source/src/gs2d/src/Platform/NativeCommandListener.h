#ifndef PLATFORM_NATIVE_COMMAND_LISTENR_H_
#define PLATFORM_NATIVE_COMMAND_LISTENR_H_

#include "../Types.h"

namespace Platform {

class NativeCommandListener
{
public:
	virtual bool ExecuteCommand(const gs2d::str_type::string& command) = 0;
};

typedef boost::shared_ptr<NativeCommandListener> NativeCommandListenerPtr;

} // namespace Platform

#endif
