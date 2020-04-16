#include "IOSFileIOHub.h"

#include "../Platform.h"
#include "../StdFileManager.h"

#include "Platform.ios.h"

namespace Platform {

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
