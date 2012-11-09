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

#include "WindowsFileIOHub.h"
#include "../Platform.h"
#include <windows.h>

namespace Platform {

FileIOHubPtr CreateFileIOHub(
	const Platform::FileManagerPtr& fileManager,
	const gs2d::str_type::string& fontDirectory,
	const gs2d::str_type::string* resourceDirectory)
{
	if (resourceDirectory == 0)
	{
		return FileIOHubPtr(new WindowsFileIOHub(fileManager, fontDirectory));
	}
	else
	{
		return FileIOHubPtr(new WindowsFileIOHub(fileManager, fontDirectory, *resourceDirectory));
	}
}

WindowsFileIOHub::WindowsFileIOHub(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& bitmapFontSearchDirectory) :
	FileIOHub(fileManager,
			  GetModuleDirectory(), GetModuleDirectory(),
			  GetModuleDirectory(), GetModuleDirectory(), bitmapFontSearchDirectory)
{
}

WindowsFileIOHub::WindowsFileIOHub(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& bitmapFontSearchDirectory,
								   const gs2d::str_type::string& resourceDirectory) :
	FileIOHub(fileManager,
			  resourceDirectory, GetModuleDirectory(),
			  GetModuleDirectory(), GetModuleDirectory(), bitmapFontSearchDirectory)
{
}

void WindowsFileIOHub::SetFileManager(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& resourceDirectory)
{
	m_fileManager = fileManager;
	SetResourceDirectory(resourceDirectory);
}

bool WindowsFileIOHub::IsResourcePackingSupported()
{
	return true;
}

}
