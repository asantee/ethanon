#ifndef IOS_FILE_IO_HUB_H_
#define IOS_FILE_IO_HUB_H_

#include "../FileIOHub.h"

namespace Platform {

class IOSFileIOHub : public FileIOHub
{
public:
	IOSFileIOHub(const gs2d::str_type::string& bitmapFontSearchDirectory);

	void SetFileManager(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& resourceDirectory);
	bool IsResourcePackingSupported();
};

typedef boost::shared_ptr<IOSFileIOHub> IOSFileIOHubPtr;

}

#endif
