#ifndef ZIP_FILE_MANAGER_H_
#define ZIP_FILE_MANAGER_H_

#include "FileManager.h"

#include <unordered_map>

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

private:
	void BuildEntryIndex();
	int FindEntry(const std::string& fixedPath) const;

	// Entry name -> index, filled once when the archive is opened. The bundled libzip looks names up with a
	// linear strcmp over every entry (12k+ in the APK), and zip_fopen also reads the entry's local header and
	// inflates its first block, so FileExists did disk I/O and GetFileBuffer scanned the directory twice.
	std::unordered_map<std::string, int> m_entryIndices;
};

typedef boost::shared_ptr<ZipFileManager> ZipFileManagerPtr;

} // namespace Platform

#endif
