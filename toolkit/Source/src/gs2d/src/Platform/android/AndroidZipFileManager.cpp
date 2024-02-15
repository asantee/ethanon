#include "AndroidZipFileManager.h"

namespace Platform {

AndroidZipFileManager::AndroidZipFileManager(const char* filePath, const char* password)
	: ZipFileManager(filePath, password)
{
}

} // namespace Platform
