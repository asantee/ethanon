#ifndef IOS_FILE_IO_HUB_H_
#define IOS_FILE_IO_HUB_H_

#include "../FileIOHub.h"

namespace Platform {

class IOSFileIOHub : public FileIOHub
{
public:
	IOSFileIOHub(const std::string& bitmapFontSearchDirectory);

	void SetFileManager(Platform::FileManagerPtr fileManager, const std::string& resourceDirectory);
	bool IsResourcePackingSupported();
};

typedef boost::shared_ptr<IOSFileIOHub> IOSFileIOHubPtr;

}

#endif
