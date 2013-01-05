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

#include "FileIOHub.h"

namespace Platform {

FileIOHub::FileIOHub(Platform::FileManagerPtr fileManager,
					 const gs2d::str_type::string& resourceDirectory,
					 const gs2d::str_type::string& programDirectory,
					 const gs2d::str_type::string& externalStorageDirectory,
					 const gs2d::str_type::string& globalExternalStorageDirectory,
					 const gs2d::str_type::string& bitmapFontSearchDirectory) :
	m_resourceDirectory(resourceDirectory),
	m_programDirectory(programDirectory),
	m_externalStorageDirectory(externalStorageDirectory),
	m_globalExternalStorageDirectory(globalExternalStorageDirectory),
	m_startResourceDirectory(resourceDirectory),
	m_fileManager(fileManager),
	m_startFileManager(fileManager),
	m_bitmapFontSearchDirectory(bitmapFontSearchDirectory),
	m_seekFontFromProgramPath(false)
{
}

gs2d::str_type::string FileIOHub::GetResourceDirectory() const
{
	return m_resourceDirectory;
}

gs2d::str_type::string FileIOHub::GetStartResourceDirectory() const
{
	return m_startResourceDirectory;
}

gs2d::str_type::string FileIOHub::GetProgramDirectory() const
{
	return m_programDirectory;
}

gs2d::str_type::string FileIOHub::GetExternalStorageDirectory() const
{
	return m_externalStorageDirectory;
}

gs2d::str_type::string FileIOHub::GetGlobalExternalStorageDirectory() const
{
	return m_globalExternalStorageDirectory;
}

void FileIOHub::SetResourceDirectory(const gs2d::str_type::string& directory)
{
	m_resourceDirectory = directory;
}

void FileIOHub::RestoreStartFileManager()
{
	m_fileManager = m_startFileManager;
	m_resourceDirectory = m_startResourceDirectory;
}

const FileManagerPtr& FileIOHub::GetFileManager()
{
	return m_fileManager;
}

gs2d::str_type::string FileIOHub::GetBitmapFontSearchDirectory() const
{
	return m_bitmapFontSearchDirectory;
}

const FileManagerPtr& FileIOHub::GetStartFileManager()
{
	return m_startFileManager;
}

gs2d::str_type::string FileIOHub::GenerateBitmapFontFilePath(const gs2d::str_type::string& fntFile) const
{
	return (m_seekFontFromProgramPath ? GetProgramDirectory() : GetResourceDirectory()) + GetBitmapFontSearchDirectory() + fntFile;
}

void FileIOHub::SeekFontFromProgramPath(const bool enable)
{
	m_seekFontFromProgramPath = enable;
}

}
