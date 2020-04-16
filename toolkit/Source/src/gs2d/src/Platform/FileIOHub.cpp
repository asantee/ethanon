#include "FileIOHub.h"

namespace Platform {

FileIOHub::FileIOHub(
	Platform::FileManagerPtr fileManager,
    const std::string& resourceDirectory,
    const std::string& programDirectory,
    const std::string& externalStorageDirectory,
    const std::string& globalExternalStorageDirectory,
    const std::string& bitmapFontSearchDirectory) :
	m_resourceDirectory(resourceDirectory),
	m_programDirectory(programDirectory),
	m_externalStorageDirectory(externalStorageDirectory),
	m_globalExternalStorageDirectory(globalExternalStorageDirectory),
	m_startResourceDirectory(resourceDirectory),
	m_fileManager(fileManager),
	m_startFileManager(fileManager),
	m_bitmapFontSearchDirectory(bitmapFontSearchDirectory),
	m_seekFontFromProgramPath(false)
{
}

std::string FileIOHub::GetResourceDirectory() const
{
	return m_resourceDirectory;
}

std::string FileIOHub::GetStartResourceDirectory() const
{
	return m_startResourceDirectory;
}

std::string FileIOHub::GetProgramDirectory() const
{
	return m_programDirectory;
}

std::string FileIOHub::GetExternalStorageDirectory() const
{
	return m_externalStorageDirectory;
}

std::string FileIOHub::GetGlobalExternalStorageDirectory() const
{
	return m_globalExternalStorageDirectory;
}

void FileIOHub::SetResourceDirectory(const std::string& directory)
{
	m_resourceDirectory = directory;
}

void FileIOHub::SetExternalStorageDirectory(const std::string& directory)
{
	m_externalStorageDirectory = directory;
}

void FileIOHub::SetGlobalExternalStorageDirectory(const std::string& directory)
{
	m_globalExternalStorageDirectory = directory;
}

void FileIOHub::RestoreStartFileManager()
{
	m_fileManager = m_startFileManager;
	m_resourceDirectory = m_startResourceDirectory;
}

const FileManagerPtr& FileIOHub::GetFileManager()
{
	return m_fileManager;
}

std::string FileIOHub::GetBitmapFontSearchDirectory() const
{
	return m_bitmapFontSearchDirectory;
}

const FileManagerPtr& FileIOHub::GetStartFileManager()
{
	return m_startFileManager;
}

std::string FileIOHub::GenerateBitmapFontFilePath(const std::string& fntFile) const
{
	return (m_seekFontFromProgramPath ? GetProgramDirectory() : GetResourceDirectory()) + GetBitmapFontSearchDirectory() + fntFile;
}

void FileIOHub::SeekFontFromProgramPath(const bool enable)
{
	m_seekFontFromProgramPath = enable;
}

}
