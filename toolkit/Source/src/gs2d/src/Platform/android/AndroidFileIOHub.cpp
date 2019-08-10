#include "AndroidFileIOHub.h"
#include "../Platform.h"

namespace Platform {

AndroidFileIOHub::AndroidFileIOHub(Platform::FileManagerPtr fileManager, 
								   const gs2d::str_type::string& externalStorageDirectory,
								   const gs2d::str_type::string& globalExternalStorageDirectory,
								   const gs2d::str_type::string& bitmapFontSearchDirectory) :
	FileIOHub(fileManager, "assets/", "",
			  externalStorageDirectory,
			  globalExternalStorageDirectory,
			  bitmapFontSearchDirectory)
{
}

void AndroidFileIOHub::SetFileManager(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& resourceDirectory)
{
    m_fileManager = fileManager;
    SetResourceDirectory(resourceDirectory);
}

bool AndroidFileIOHub::IsResourcePackingSupported()
{
	return false;
}

}
