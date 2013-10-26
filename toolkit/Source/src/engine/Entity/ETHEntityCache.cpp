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

#include "ETHEntityCache.h"

#include "ETHEntityProperties.h"

const ETHEntityProperties* ETHEntityCache::Get(const str_type::string& fileName, const str_type::string& filePath, const Platform::FileManagerPtr& fileManager)
{
	std::map<str_type::string, ETHEntityProperties>::iterator iter = m_props.find(fileName);
	if (iter != m_props.end())
	{
		return &(iter->second);
	}
	else
	{
		const str_type::string fullFilePath = filePath + fileName;
		
		if (fileManager->FileExists(fullFilePath))
		{
			ETHEntityProperties props(fullFilePath, fileManager);
			if (props.IsSuccessfullyLoaded())
			{
				m_props[fileName] = props;
				return &(m_props[fileName]);
			}
		}
	}
	return 0;
}
