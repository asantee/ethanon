#ifndef NATIVE_COMMAND_FORWARDER_H_
#define NATIVE_COMMAND_FORWARDER_H_

#include "../Types.h"

namespace Platform {

class NativeCommandForwarder
{
	gs2d::str_type::string m_commands;
protected:
	NativeCommandForwarder();
	void ForwardCommands(gs2d::str_type::string& out);
public:
	void Command(const gs2d::str_type::string& commandSt, const bool onTop = false);
};

} // namespace Platform

#endif
