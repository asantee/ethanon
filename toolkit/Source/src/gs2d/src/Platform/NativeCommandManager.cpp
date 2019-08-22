#include "NativeCommandManager.h"

#include "Platform.h"

namespace Platform {

void NativeCommandManager::InsertCommandListener(const NativeCommandListenerPtr& listener)
{
	m_listeners.push_back(listener);
}

void NativeCommandManager::RunCommands(const std::string& commands)
{
	if (commands == "")
		return;

	std::vector<std::string> commandLines = Platform::SplitString(commands, ("\n"));
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
