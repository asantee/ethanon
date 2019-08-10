#ifndef ANDROID_FILE_IO_HUB_H_
#define ANDROID_FILE_IO_HUB_H_

#include "../FileIOHub.h"

namespace Platform {

class AndroidFileIOHub : public FileIOHub
{
public:
	AndroidFileIOHub(Platform::FileManagerPtr fileManager, 
					 const gs2d::str_type::string& externalStorageDirectory,
					 const gs2d::str_type::string& globalExternalStorageDirectory,
					 const gs2d::str_type::string& bitmapFontSearchDirectory);

	void SetFileManager(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& resourceDirectory);
	bool IsResourcePackingSupported();
};

typedef boost::shared_ptr<AndroidFileIOHub> AndroidFileIOHubPtr;

}

#endif
