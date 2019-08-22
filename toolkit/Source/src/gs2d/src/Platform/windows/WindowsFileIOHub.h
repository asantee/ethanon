#ifndef WINDOWS_FILE_IO_HUB_H_
#define WINDOWS_FILE_IO_HUB_H_

#include "../FileIOHub.h"

namespace Platform {

class WindowsFileIOHub : public FileIOHub
{
public:
	WindowsFileIOHub(Platform::FileManagerPtr fileManager, const std::string& bitmapFontSearchDirectory);
	WindowsFileIOHub(Platform::FileManagerPtr fileManager, const std::string& bitmapFontSearchDirectory, const std::string& resourceDirectory);
	void SetFileManager(Platform::FileManagerPtr fileManager, const std::string& resourceDirectory);
	bool IsResourcePackingSupported();
};

typedef boost::shared_ptr<WindowsFileIOHub> WindowsFileIOHubPtr;

}

#endif
