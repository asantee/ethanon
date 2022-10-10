#ifndef MACOS_FILE_IO_HUB_H_
#define MACOS_FILE_IO_HUB_H_

#include "../FileIOHub.h"

namespace Platform {

class MacOSFileIOHub : public FileIOHub
{
public:
	MacOSFileIOHub(
		Platform::FileManagerPtr fileManager,
		const std::string& bitmapFontSearchDirectory);

	void SetFileManager(Platform::FileManagerPtr fileManager, const std::string& resourceDirectory);
	bool IsResourcePackingSupported();
	std::string GetGlobalExternalStorageDirectory() const;
};

typedef boost::shared_ptr<MacOSFileIOHub> MacOSFileIOHubPtr;

}

#endif
