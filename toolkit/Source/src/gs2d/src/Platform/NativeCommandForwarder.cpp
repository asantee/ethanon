#include "NativeCommandForwarder.h"

namespace Platform {

using namespace gs2d;

NativeCommandForwarder::NativeCommandForwarder() :
	m_commands(GS_L(""))
{
}

void NativeCommandForwarder::ForwardCommands(str_type::string& out)
{
	out = m_commands;
	m_commands = GS_L("");
}

void NativeCommandForwarder::Command(const str_type::string& commandStr, const bool onTop)
{
	const str_type::string commandLine = (commandStr + GS_L("\n"));
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
