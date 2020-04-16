#include "NativeCommandForwarder.h"

namespace Platform {

NativeCommandForwarder::NativeCommandForwarder() :
	m_commands((""))
{
}

void NativeCommandForwarder::ForwardCommands(std::string& out)
{
	out = m_commands;
	m_commands = ("");
}

void NativeCommandForwarder::Command(const std::string& commandStr, const bool onTop)
{
	const std::string commandLine = (commandStr + ("\n"));
	if (!onTop)
	{
		m_commands += commandLine;
	}
	else
	{
		m_commands = commandLine + m_commands;
	}
}


} // namespace Platform
