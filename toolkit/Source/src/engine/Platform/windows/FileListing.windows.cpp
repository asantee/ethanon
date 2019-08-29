#include "../FileListing.h"

#include <windows.h>

namespace Platform {

bool FileListing::ListDirectoryFiles(const char* directory, const char* extension)
{
	m_fileName.clear();
	std::string dirExt;
	dirExt = directory;
	dirExt += "*.";
	dirExt += extension;

	WIN32_FIND_DATAA findFileData;
	HANDLE hFind = FindFirstFileA(dirExt.c_str(), &findFileData);
	
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	FILE_NAME filename;
	filename.dir = directory;
	filename.file = findFileData.cFileName;
	m_fileName.push_back(filename);

	while (FindNextFileA(hFind, &findFileData) != 0)
	{
		filename.dir = directory;
		filename.file = findFileData.cFileName;
		m_fileName.push_back(filename);
	}
	FindClose(hFind);
	return true;
}

} // namespace Platform
