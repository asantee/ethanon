#include "WindowsFileIOHub.h"
#include "../Platform.h"
#include <windows.h>

namespace Platform {

FileIOHubPtr CreateFileIOHub(
	const Platform::FileManagerPtr& fileManager,
	const std::string& fontDirectory,
	const std::string* resourceDirectory)
{
	if (resourceDirectory == 0)
	{
		return FileIOHubPtr(new WindowsFileIOHub(fileManager, fontDirectory));
	}
	else
	{
		return FileIOHubPtr(new WindowsFileIOHub(fileManager, fontDirectory, *resourceDirectory));
	}
}

WindowsFileIOHub::WindowsFileIOHub(Platform::FileManagerPtr fileManager, const std::string& bitmapFontSearchDirectory) :
	FileIOHub(fileManager,
			  GetModuleDirectory(), GetModuleDirectory(),
			  GetModuleDirectory(), GetModuleDirectory(), bitmapFontSearchDirectory)
{
}

WindowsFileIOHub::WindowsFileIOHub(Platform::FileManagerPtr fileManager, const std::string& bitmapFontSearchDirectory,
								   const std::string& resourceDirectory) :
	FileIOHub(fileManager,
			  resourceDirectory, GetModuleDirectory(),
			  GetModuleDirectory(), GetModuleDirectory(), bitmapFontSearchDirectory)
{
}

void WindowsFileIOHub::SetFileManager(Platform::FileManagerPtr fileManager, const std::string& resourceDirectory)
{
	m_fileManager = fileManager;
	SetResourceDirectory(resourceDirectory);
}

bool WindowsFileIOHub::IsResourcePackingSupported()
{
	return true;
}

}
