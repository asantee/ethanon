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

#include "MacOSXFileIOHub.h"
#include "../Platform.h"
#include "../FileLogger.h"
#include "../../Application.h"

#include "Platform.macosx.h"

namespace Platform {

FileIOHubPtr CreateFileIOHub(
	const Platform::FileManagerPtr& fileManager,
	const gs2d::str_type::string& fontDirectory,
	const gs2d::str_type::string& resourceDirectory)
{
	GS2D_UNUSED_ARGUMENT(resourceDirectory);
	return FileIOHubPtr(new MacOSXFileIOHub(fileManager, fontDirectory));
}

MacOSXFileIOHub::MacOSXFileIOHub(
	Platform::FileManagerPtr fileManager,
	const gs2d::str_type::string& bitmapFontSearchDirectory) :
	FileIOHub(
		fileManager, ResourceDirectory(),
		GetModuleDirectory(),
		ExternalStorageDirectory(),
		GlobalExternalStorageDirectory(),
		bitmapFontSearchDirectory)
{
	CreateDirectoryNS([NSString stringWithCString:ExternalStorageDirectory().c_str() encoding:NSUTF8StringEncoding]);
	CreateDirectoryNS([NSString stringWithCString:ResourceDirectory().c_str() encoding:NSUTF8StringEncoding]);
	CreateDirectoryNS([NSString stringWithCString:Platform::FileLogger::GetLogDirectory().c_str() encoding:NSUTF8StringEncoding]);

	#ifdef DEBUG
	 gs2d::ShowMessage(gs2d::str_type::string("Resources: ") + GetResourceDirectory(), gs2d::GSMT_INFO);
	 gs2d::ShowMessage(gs2d::str_type::string("Program:   ") + GetProgramDirectory(), gs2d::GSMT_INFO);
	 gs2d::ShowMessage(gs2d::str_type::string("External:  ") + GetExternalStorageDirectory(), gs2d::GSMT_INFO);
	 gs2d::ShowMessage(gs2d::str_type::string("GExternal: ") + GetGlobalExternalStorageDirectory(), gs2d::GSMT_INFO);
	#endif
}

gs2d::str_type::string MacOSXFileIOHub::GetGlobalExternalStorageDirectory() const
{
	// create directory before informing it to the user
	// it forces your application to create the directory only if the user will need it
	CreateDirectoryNS([NSString stringWithCString:GlobalExternalStorageDirectory().c_str() encoding:NSUTF8StringEncoding]);	
	return FileIOHub::GetGlobalExternalStorageDirectory();
}

void MacOSXFileIOHub::SetFileManager(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& resourceDirectory)
{
    // not supported
}

bool MacOSXFileIOHub::IsResourcePackingSupported()
{
	return false;
}

} // namespace Platform

gs2d::str_type::string Platform::FileLogger::GetLogDirectory()
{
	return Platform::ExternalStorageDirectory() + "log/";
}
