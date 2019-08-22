#include "ETHFileChangeDetector.h"

const unsigned int ETHFileChangeDetector::m_timeStrideMS(2000);

ETHFileChangeDetector::ETHFileChangeDetector(const gs2d::ApplicationPtr& app, const std::string& filePath, const ENCODING encoding) :
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
		std::string newContent;
		switch (m_encoding)
		{
		case ANSI:
			m_fileManager->GetAnsiFileString(m_filePath, newContent);
			break;
		case UTF8_WITH_BOM:
		case UTF16_WITH_BOM:
			m_fileManager->GetUTFFileString(m_filePath, newContent);
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
