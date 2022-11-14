#include "IOSFileIOHub.h"

#include "../Platform.h"
#include "../StdFileManager.h"

#include "Platform.ios.h"

namespace Platform {

FileIOHubPtr CreateFileIOHub(
	const Platform::FileManagerPtr& fileManager,
	const std::string& fontDirectory,
	const std::string* resourceDirectory)
{
	Platform::FileIOHubPtr r = FileIOHubPtr(new IOSFileIOHub(fontDirectory));
	if (resourceDirectory != 0)
		r->SetResourceDirectory(*resourceDirectory);
	return r;
}

IOSFileIOHub::IOSFileIOHub(const std::string& bitmapFontSearchDirectory) :
	FileIOHub(
		Platform::StdFileManagerPtr(new Platform::StdFileManager()),
		ResourceDirectory(),
		GetModuleDirectory(),
		ExternalStorageDirectory(),
		GlobalExternalStorageDirectory(),
		bitmapFontSearchDirectory)
{
	CreateDirectory(ExternalStorageDirectory());
	CreateDirectory(FileLogger::GetLogDirectory());
}

void IOSFileIOHub::SetFileManager(Platform::FileManagerPtr fileManager, const std::string& resourceDirectory)
{
    // not supported
}

bool IOSFileIOHub::IsResourcePackingSupported()
{
	return false;
}

}
