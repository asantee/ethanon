/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

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
