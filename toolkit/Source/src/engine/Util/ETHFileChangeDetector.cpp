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

#include "ETHFileChangeDetector.h"

const unsigned int ETHFileChangeDetector::m_timeStrideMS(2000);

ETHFileChangeDetector::ETHFileChangeDetector(const gs2d::ApplicationPtr& app, const gs2d::str_type::string& filePath, const ENCODING encoding) :
	m_filePath(filePath),
	m_fileChanged(false),
	m_encoding(encoding),
	m_app(app),
	m_lastTime(m_timeStrideMS)
{
	m_fileManager = app->GetFileIOHub()->GetFileManager();
	Update();
	m_fileChanged = false;
}

bool ETHFileChangeDetector::IsValidFile() const
{
	return !m_fileContent.empty();
}

bool ETHFileChangeDetector::CheckForChange()
{
	const bool r = m_fileChanged;
	m_fileChanged = false;
	return r;
}

void ETHFileChangeDetector::Update()
{
	if ((m_app->GetElapsedTime() - m_lastTime) > m_timeStrideMS)
	{
		gs2d::str_type::string newContent;
		switch (m_encoding)
		{
		case ANSI:
			m_fileManager->GetAnsiFileString(m_filePath, newContent);
			break;
		case UTF8_WITH_BOM:
			m_fileManager->GetUTF8FileString(m_filePath, newContent);
			break;
		case UTF16_WITH_BOM:
			m_fileManager->GetUTF16FileString(m_filePath, newContent);
			break;
		}
		if (newContent.size() != m_fileContent.size())
		{
			m_fileChanged = true;
		}
		else if (newContent != m_fileContent)
		{
			m_fileChanged = true;
		}
		m_fileContent = newContent;
		m_lastTime = m_app->GetElapsedTime();
	}
}
