#ifndef ANDROID_ZIP_FILE_MANAGER_H_
#define ANDROID_ZIP_FILE_MANAGER_H_

#include "../ZipFileManager.h"

namespace Platform {

class AndroidZipFileManager : public ZipFileManager
{
public:
    AndroidZipFileManager(const char* filePath, const char* password = 0);
};

typedef boost::shared_ptr<AndroidZipFileManager> AndroidZipFileManagerPtr;

} // namespace Platform

#endif
