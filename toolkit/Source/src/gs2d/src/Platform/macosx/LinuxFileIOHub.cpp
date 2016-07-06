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

#include "LinuxFileIOHub.h"
#include "../Platform.h"
#include "../FileLogger.h"
#include "../../Application.h"

#include "Platform.linux.h"

namespace Platform {

FileIOHubPtr CreateFileIOHub(
	const Platform::FileManagerPtr& fileManager,
	const gs2d::str_type::string& fontDirectory,
	const gs2d::str_type::string* resourceDirectory)
{
	Platform::FileIOHubPtr r = FileIOHubPtr(new LinuxFileIOHub(fileManager, fontDirectory));
	if (resourceDirectory != 0)
		r->SetResourceDirectory(*resourceDirectory);
	return r;
}

LinuxFileIOHub::LinuxFileIOHub(
	Platform::FileManagerPtr fileManager,
	const gs2d::str_type::string& bitmapFontSearchDirectory) :
	FileIOHub(
		fileManager, ResourceDirectory(),
		GetModuleDirectory(),
		ExternalStorageDirectory(),
		GlobalExternalStorageDirectory(),
		bitmapFontSearchDirectory)
{
	gs2d::ShowMessage(gs2d::str_type::string("Resources: ") + GetResourceDirectory(), gs2d::GSMT_INFO);
	gs2d::ShowMessage(gs2d::str_type::string("Program:   ") + GetProgramDirectory(), gs2d::GSMT_INFO);
	gs2d::ShowMessage(gs2d::str_type::string("External:  ") + GetExternalStorageDirectory(), gs2d::GSMT_INFO);
	gs2d::ShowMessage(gs2d::str_type::string("GExternal: ") + GetGlobalExternalStorageDirectory(), gs2d::GSMT_INFO);
}

gs2d::str_type::string LinuxFileIOHub::GetGlobalExternalStorageDirectory() const
{
	return "";
}

void LinuxFileIOHub::SetFileManager(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& resourceDirectory)
{
    // not supported
}

bool LinuxFileIOHub::IsResourcePackingSupported()
{
	return false;
}

} // namespace Platform

gs2d::str_type::string Platform::FileLogger::GetLogDirectory()
{
	return Platform::ExternalStorageDirectory() + "log/";
}
