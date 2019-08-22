#include "MacOSXFileIOHub.h"
#include "../Platform.h"
#include "../FileLogger.h"
#include "../../Application.h"

#include "Platform.macosx.h"

namespace Platform {

FileIOHubPtr CreateFileIOHub(
	const Platform::FileManagerPtr& fileManager,
	const std::string& fontDirectory,
	const std::string* resourceDirectory)
{
	Platform::FileIOHubPtr r = FileIOHubPtr(new MacOSXFileIOHub(fileManager, fontDirectory));
	if (resourceDirectory != 0)
		r->SetResourceDirectory(*resourceDirectory);
	return r;
}

MacOSXFileIOHub::MacOSXFileIOHub(
	Platform::FileManagerPtr fileManager,
	const std::string& bitmapFontSearchDirectory) :
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

	gs2d::ShowMessage(std::string("Resources: ") + GetResourceDirectory(), gs2d::GSMT_INFO);
	gs2d::ShowMessage(std::string("Program:   ") + GetProgramDirectory(), gs2d::GSMT_INFO);
	gs2d::ShowMessage(std::string("External:  ") + GetExternalStorageDirectory(), gs2d::GSMT_INFO);
	gs2d::ShowMessage(std::string("GExternal: ") + GetGlobalExternalStorageDirectory(), gs2d::GSMT_INFO);
}

std::string MacOSXFileIOHub::GetGlobalExternalStorageDirectory() const
{
	// create directory before informing it to the user
	// it forces your application to create the directory only if the user will need it
	CreateDirectoryNS([NSString stringWithCString:GlobalExternalStorageDirectory().c_str() encoding:NSUTF8StringEncoding]);	
	return FileIOHub::GetGlobalExternalStorageDirectory();
}

void MacOSXFileIOHub::SetFileManager(Platform::FileManagerPtr fileManager, const std::string& resourceDirectory)
{
    // not supported
}

bool MacOSXFileIOHub::IsResourcePackingSupported()
{
	return false;
}

} // namespace Platform

std::string Platform::FileLogger::GetLogDirectory()
{
	return Platform::ExternalStorageDirectory() + "log/";
}
