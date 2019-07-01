#include "NativeCommandManager.h"

#include "Platform.h"

namespace Platform {

void NativeCommandManager::InsertCommandListener(const NativeCommandListenerPtr& listener)
{
	m_listeners.push_back(listener);
}

void NativeCommandManager::RunCommands(const gs2d::str_type::string& commands)
{
	if (commands == "")
		return;

	std::vector<gs2d::str_type::string> commandLines = Platform::SplitString(commands, GS_L("\n"));
	for (std::size_t t = 0; t < commandLines.size(); t++)
	{
		for (std::size_t c = 0; c < m_listeners.size(); c++)
		{
			if (m_listeners[c]->ExecuteCommand(commandLines[t]))
			{
				break;
			}
		}
	}
}

} // namespace Platform
