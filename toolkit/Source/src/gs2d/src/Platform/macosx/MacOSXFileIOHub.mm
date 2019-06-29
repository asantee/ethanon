#include "MacOSXFileIOHub.h"
#include "../Platform.h"
#include "../FileLogger.h"
#include "../../Application.h"

#include "Platform.macosx.h"

namespace Platform {

FileIOHubPtr CreateFileIOHub(
	const Platform::FileManagerPtr& fileManager,
	const gs2d::str_type::string& fontDirectory,
	const gs2d::str_type::string* resourceDirectory)
{
	Platform::FileIOHubPtr r = FileIOHubPtr(new MacOSXFileIOHub(fileManager, fontDirectory));
	if (resourceDirectory != 0)
		r->SetResourceDirectory(*resourceDirectory);
	return r;
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

	gs2d::ShowMessage(gs2d::str_type::string("Resources: ") + GetResourceDirectory(), gs2d::GSMT_INFO);
	gs2d::ShowMessage(gs2d::str_type::string("Program:   ") + GetProgramDirectory(), gs2d::GSMT_INFO);
	gs2d::ShowMessage(gs2d::str_type::string("External:  ") + GetExternalStorageDirectory(), gs2d::GSMT_INFO);
	gs2d::ShowMessage(gs2d::str_type::string("GExternal: ") + GetGlobalExternalStorageDirectory(), gs2d::GSMT_INFO);
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
