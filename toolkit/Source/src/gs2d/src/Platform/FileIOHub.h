#ifndef FILE_IO_HUB_H_
#define FILE_IO_HUB_H_

#include <boost/shared_ptr.hpp>

#include "FileManager.h"

namespace Platform {

class FileIOHub
{
public:
	FileIOHub(Platform::FileManagerPtr fileManager,
			  const std::string& resourceDirectory,
			  const std::string& programDirectory,
			  const std::string& externalStorageDirectory,
			  const std::string& globalExternalStorageDirectory,
			  const std::string& bitmapFontSearchDirectory);

	virtual std::string GetResourceDirectory() const;
	virtual std::string GetStartResourceDirectory() const;
	virtual std::string GetProgramDirectory() const;
	virtual std::string GetExternalStorageDirectory() const;
	virtual std::string GetGlobalExternalStorageDirectory() const;

	virtual void SetResourceDirectory(const std::string& directory);
	virtual void SetFileManager(Platform::FileManagerPtr fileManager, const std::string& resourceDirectory) = 0;
	virtual const FileManagerPtr& GetFileManager();
	virtual const FileManagerPtr& GetStartFileManager();
	virtual bool IsResourcePackingSupported() = 0;
	virtual void RestoreStartFileManager();
	virtual std::string GetBitmapFontSearchDirectory() const;
	virtual std::string GenerateBitmapFontFilePath(const std::string& fntFile) const;

	void SeekFontFromProgramPath(const bool enable);

protected:
	std::string m_bitmapFontSearchDirectory;
	std::string m_startResourceDirectory;
	std::string m_resourceDirectory;
	std::string m_programDirectory;
	std::string m_externalStorageDirectory;
	std::string m_globalExternalStorageDirectory;

	Platform::FileManagerPtr m_startFileManager;
	Platform::FileManagerPtr m_fileManager;

	bool m_seekFontFromProgramPath;
};

typedef boost::shared_ptr<FileIOHub> FileIOHubPtr;

FileIOHubPtr CreateFileIOHub(
	const Platform::FileManagerPtr& fileManager,
	const std::string& fontDirectory,
	const std::string* resourceDirectory = 0);

}

#endif
