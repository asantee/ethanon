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
