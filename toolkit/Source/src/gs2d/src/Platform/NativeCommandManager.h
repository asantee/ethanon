#ifndef PLATFORM_NATIVE_COMMAND_MANAGER_H_
#define PLATFORM_NATIVE_COMMAND_MANAGER_H_

#include "NativeCommandListener.h"

#include <vector>

namespace Platform {

class NativeCommandManager
{
	std::vector<NativeCommandListenerPtr> m_listeners;

public:

	void InsertCommandListener(const NativeCommandListenerPtr& listener);
	void RunCommands(const gs2d::str_type::string& commands);
};

} // namespace Platform

#endif
