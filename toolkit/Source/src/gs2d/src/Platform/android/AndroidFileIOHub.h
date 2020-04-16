#ifndef ANDROID_FILE_IO_HUB_H_
#define ANDROID_FILE_IO_HUB_H_

#include "../FileIOHub.h"

namespace Platform {

class AndroidFileIOHub : public FileIOHub
{
public:
	AndroidFileIOHub(
		Platform::FileManagerPtr fileManager, 
		 const std::string& externalStorageDirectory,
		 const std::string& globalExternalStorageDirectory,
		 const std::string& bitmapFontSearchDirectory);

	void SetFileManager(Platform::FileManagerPtr fileManager, const std::string& resourceDirectory);
	bool IsResourcePackingSupported();
};

typedef boost::shared_ptr<AndroidFileIOHub> AndroidFileIOHubPtr;

}

#endif
