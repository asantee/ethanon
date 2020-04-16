#ifndef NATIVE_COMMAND_FORWARDER_H_
#define NATIVE_COMMAND_FORWARDER_H_

#include <string>

namespace Platform {

class NativeCommandForwarder
{
	std::string m_commands;
protected:
	NativeCommandForwarder();
	void ForwardCommands(std::string& out);
public:
	void Command(const std::string& commandSt, const bool onTop = false);
};

} // namespace Platform

#endif
