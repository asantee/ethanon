#ifndef MACOSX_FILE_IO_HUB_H_
#define MACOSX_FILE_IO_HUB_H_

#include "../FileIOHub.h"

namespace Platform {

class MacOSXFileIOHub : public FileIOHub
{
public:
	MacOSXFileIOHub(
		Platform::FileManagerPtr fileManager,
		const gs2d::str_type::string& bitmapFontSearchDirectory);

	void SetFileManager(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& resourceDirectory);
	bool IsResourcePackingSupported();
	gs2d::str_type::string GetGlobalExternalStorageDirectory() const;
};

typedef boost::shared_ptr<MacOSXFileIOHub> MacOSXFileIOHubPtr;

}

#endif
