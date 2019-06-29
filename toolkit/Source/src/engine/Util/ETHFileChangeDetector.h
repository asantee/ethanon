#ifndef ETH_FILE_CHANGE_DETECTOR_H_
#define ETH_FILE_CHANGE_DETECTOR_H_

#include <Platform/FileManager.h>
#include <Application.h>

class ETHFileChangeDetector
{
	gs2d::ApplicationPtr m_app;
	Platform::FileManagerPtr m_fileManager;
	gs2d::str_type::string m_filePath;
	gs2d::str_type::string m_fileContent;
	bool m_fileChanged;
	static const unsigned int m_timeStrideMS;
	unsigned long m_lastTime;

public:

	enum ENCODING
	{
		ANSI = 0,
		UTF8_WITH_BOM = 1,
		UTF16_WITH_BOM = 2
	};

	ETHFileChangeDetector(const gs2d::ApplicationPtr& app, const gs2d::str_type::string& filePath, const ENCODING encoding);
	bool IsValidFile() const;
	bool CheckForChange();
	void Update();

private:
	ENCODING m_encoding;
};

typedef boost::shared_ptr<ETHFileChangeDetector> ETHFileChangeDetectorPtr;

#endif
