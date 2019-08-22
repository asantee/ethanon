#ifndef IOS_NATIVE_COMMAND_LISTENER_H_
#define IOS_NATIVE_COMMAND_LISTENER_H_

#include "../NativeCommandListener.h"

namespace Platform {

class IOSNativeCommmandListener : public NativeCommandListener
{
public:
	bool ExecuteCommand(const std::string& commandLine);
};

typedef boost::shared_ptr<IOSNativeCommmandListener> IOSNativeCommmandListenerPtr;

} // namespace Platform

#endif
