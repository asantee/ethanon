#ifndef LINUX_FILE_IO_HUB_H_
#define LINUX_FILE_IO_HUB_H_

#include "../FileIOHub.h"

namespace Platform {

class LinuxFileIOHub : public FileIOHub
{
public:
	LinuxFileIOHub(
		Platform::FileManagerPtr fileManager,
		const std::string& bitmapFontSearchDirectory);

	void SetFileManager(Platform::FileManagerPtr fileManager, const std::string& resourceDirectory);
	bool IsResourcePackingSupported();
	std::string GetGlobalExternalStorageDirectory() const;
};

typedef boost::shared_ptr<LinuxFileIOHub> LinuxFileIOHubPtr;

}

#endif
