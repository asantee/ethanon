#ifndef FILE_IO_HUB_H_
#define FILE_IO_HUB_H_

#include <boost/shared_ptr.hpp>

#include "FileManager.h"

namespace Platform {

class FileIOHub
{
public:
	FileIOHub(Platform::FileManagerPtr fileManager,
			  const gs2d::str_type::string& resourceDirectory,
			  const gs2d::str_type::string& programDirectory,
			  const gs2d::str_type::string& externalStorageDirectory,
			  const gs2d::str_type::string& globalExternalStorageDirectory,
			  const gs2d::str_type::string& bitmapFontSearchDirectory);

	virtual gs2d::str_type::string GetResourceDirectory() const;
	virtual gs2d::str_type::string GetStartResourceDirectory() const;
	virtual gs2d::str_type::string GetProgramDirectory() const;
	virtual gs2d::str_type::string GetExternalStorageDirectory() const;
	virtual gs2d::str_type::string GetGlobalExternalStorageDirectory() const;

	virtual void SetResourceDirectory(const gs2d::str_type::string& directory);
	virtual void SetFileManager(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& resourceDirectory) = 0;
	virtual const FileManagerPtr& GetFileManager();
	virtual const FileManagerPtr& GetStartFileManager();
	virtual bool IsResourcePackingSupported() = 0;
	virtual void RestoreStartFileManager();
	virtual gs2d::str_type::string GetBitmapFontSearchDirectory() const;
	virtual gs2d::str_type::string GenerateBitmapFontFilePath(const gs2d::str_type::string& fntFile) const;

	void SeekFontFromProgramPath(const bool enable);

protected:
	gs2d::str_type::string m_bitmapFontSearchDirectory;
	gs2d::str_type::string m_startResourceDirectory;
	gs2d::str_type::string m_resourceDirectory;
	gs2d::str_type::string m_programDirectory;
	gs2d::str_type::string m_externalStorageDirectory;
	gs2d::str_type::string m_globalExternalStorageDirectory;

	Platform::FileManagerPtr m_startFileManager;
	Platform::FileManagerPtr m_fileManager;

	bool m_seekFontFromProgramPath;
};

typedef boost::shared_ptr<FileIOHub> FileIOHubPtr;

FileIOHubPtr CreateFileIOHub(
	const Platform::FileManagerPtr& fileManager,
	const gs2d::str_type::string& fontDirectory,
	const gs2d::str_type::string* resourceDirectory = 0);

}

#endif
