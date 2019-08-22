#include "LinuxFileIOHub.h"
#include "../Platform.h"
#include "../FileLogger.h"
#include "../../Application.h"

#include "Platform.linux.h"

namespace Platform {

FileIOHubPtr CreateFileIOHub(
	const Platform::FileManagerPtr& fileManager,
	const std::string& fontDirectory,
	const std::string* resourceDirectory)
{
	Platform::FileIOHubPtr r = FileIOHubPtr(new LinuxFileIOHub(fileManager, fontDirectory));
	if (resourceDirectory != 0)
		r->SetResourceDirectory(*resourceDirectory);
	return r;
}

LinuxFileIOHub::LinuxFileIOHub(
	Platform::FileManagerPtr fileManager,
	const std::string& bitmapFontSearchDirectory) :
	FileIOHub(
		fileManager, ResourceDirectory(),
		GetModuleDirectory(),
		ExternalStorageDirectory(),
		GlobalExternalStorageDirectory(),
		bitmapFontSearchDirectory)
{
	gs2d::ShowMessage(std::string("Resources: ") + GetResourceDirectory(), gs2d::GSMT_INFO);
	gs2d::ShowMessage(std::string("Program:   ") + GetProgramDirectory(), gs2d::GSMT_INFO);
	gs2d::ShowMessage(std::string("External:  ") + GetExternalStorageDirectory(), gs2d::GSMT_INFO);
	gs2d::ShowMessage(std::string("GExternal: ") + GetGlobalExternalStorageDirectory(), gs2d::GSMT_INFO);
}

std::string LinuxFileIOHub::GetGlobalExternalStorageDirectory() const
{
	return "";
}

void LinuxFileIOHub::SetFileManager(Platform::FileManagerPtr fileManager, const std::string& resourceDirectory)
{
    // not supported
}

bool LinuxFileIOHub::IsResourcePackingSupported()
{
	return false;
}

} // namespace Platform

std::string Platform::FileLogger::GetLogDirectory()
{
	return Platform::ExternalStorageDirectory() + "log/";
}
