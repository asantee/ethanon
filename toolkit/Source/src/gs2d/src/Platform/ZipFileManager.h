#ifndef ZIP_FILE_MANAGER_H_
#define ZIP_FILE_MANAGER_H_

#include "FileManager.h"

struct zip;

namespace Platform {

class ZipFileManager : public FileManager
{
public:
	ZipFileManager(const char* filePath, const char* password = 0);
	~ZipFileManager();

	bool IsLoaded() const;
	bool GetFileBuffer(const std::string& fileName, FileBuffer& out);
	bool FileExists(const std::string& fileName) const;
	bool IsPacked() const;
	zip* GetZip();

protected:
	zip* m_archive;
};

typedef boost::shared_ptr<ZipFileManager> ZipFileManagerPtr;

} // namespace Platform

#endif
