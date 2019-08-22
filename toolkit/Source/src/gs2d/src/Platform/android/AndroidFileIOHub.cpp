#include "AndroidFileIOHub.h"
#include "../Platform.h"

namespace Platform {

AndroidFileIOHub::AndroidFileIOHub(Platform::FileManagerPtr fileManager, 
								   const std::string& externalStorageDirectory,
								   const std::string& globalExternalStorageDirectory,
								   const std::string& bitmapFontSearchDirectory) :
	FileIOHub(fileManager, "assets/", "",
			  externalStorageDirectory,
			  globalExternalStorageDirectory,
			  bitmapFontSearchDirectory)
{
}

void AndroidFileIOHub::SetFileManager(Platform::FileManagerPtr fileManager, const std::string& resourceDirectory)
{
    m_fileManager = fileManager;
    SetResourceDirectory(resourceDirectory);
}

bool AndroidFileIOHub::IsResourcePackingSupported()
{
	return false;
}

}
