/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

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

#ifndef FILE_IO_HUB_H_
#define FILE_IO_HUB_H_

#include <boost/shared_ptr.hpp>
#include "FileManager.h"

namespace Platform {

class FileIOHub
{
public:
	FileIOHub(Platform::FileManagerPtr fileManager,
			  const gs2d::str_type::string& resourceDirectory,
			  const gs2d::str_type::string& programDirectory,
			  const gs2d::str_type::string& externalStorageDirectory,
			  const gs2d::str_type::string& globalExternalStorageDirectory,
			  const gs2d::str_type::string& bitmapFontSearchDirectory);

	virtual gs2d::str_type::string GetResourceDirectory() const;
	virtual gs2d::str_type::string GetStartResourceDirectory() const;
	virtual gs2d::str_type::string GetProgramDirectory() const;
	virtual gs2d::str_type::string GetExternalStorageDirectory() const;
	virtual gs2d::str_type::string GetGlobalExternalStorageDirectory() const;

	virtual void SetResourceDirectory(const gs2d::str_type::string& directory);
	virtual void SetFileManager(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& resourceDirectory) = 0;
	virtual const FileManagerPtr& GetFileManager();
	virtual const FileManagerPtr& GetStartFileManager();
	virtual bool IsResourcePackingSupported() = 0;
	virtual void RestoreStartFileManager();
	virtual gs2d::str_type::string GetBitmapFontSearchDirectory() const;
	virtual gs2d::str_type::string GenerateBitmapFontFilePath(const gs2d::str_type::string& fntFile) const;

	void SeekFontFromProgramPath(const bool enable);

protected:
	gs2d::str_type::string m_bitmapFontSearchDirectory;
	gs2d::str_type::string m_startResourceDirectory;
	gs2d::str_type::string m_resourceDirectory;
	gs2d::str_type::string m_programDirectory;
	gs2d::str_type::string m_externalStorageDirectory;
	gs2d::str_type::string m_globalExternalStorageDirectory;

	Platform::FileManagerPtr m_startFileManager;
	Platform::FileManagerPtr m_fileManager;

	bool m_seekFontFromProgramPath;
};

typedef boost::shared_ptr<FileIOHub> FileIOHubPtr;

}

#endif
