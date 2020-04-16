#ifndef MACOSX_FILE_IO_HUB_H_
#define MACOSX_FILE_IO_HUB_H_

#include "../FileIOHub.h"

namespace Platform {

class MacOSXFileIOHub : public FileIOHub
{
public:
	MacOSXFileIOHub(
		Platform::FileManagerPtr fileManager,
		const std::string& bitmapFontSearchDirectory);

	void SetFileManager(Platform::FileManagerPtr fileManager, const std::string& resourceDirectory);
	bool IsResourcePackingSupported();
	std::string GetGlobalExternalStorageDirectory() const;
};

typedef boost::shared_ptr<MacOSXFileIOHub> MacOSXFileIOHubPtr;

}

#endif
