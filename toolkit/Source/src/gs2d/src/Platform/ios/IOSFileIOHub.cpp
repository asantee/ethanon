#include "IOSFileIOHub.h"

#include "../Platform.h"
#include "../StdFileManager.h"

#include "Platform.ios.h"

namespace Platform {

IOSFileIOHub::IOSFileIOHub(const gs2d::str_type::string& bitmapFontSearchDirectory) :
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

void IOSFileIOHub::SetFileManager(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& resourceDirectory)
{
    // not supported
}

bool IOSFileIOHub::IsResourcePackingSupported()
{
	return false;
}

}
