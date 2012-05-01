#include "stdafx.h"
#include <string>
#include <vector>
#include "WildcardSearcher.h"
#include "Messages.h"
#include "AsciiVerifier.h"
#include "Utf8Verifier.h"
#include "Utf16Verifier.h"

#include "windows.h"

void SearchFilesInDirectory(
	FileFormat formatType, const std::wstring& directory, const std::wstring& filemask, const std::wstring& excludeDirectories
);

void VerifyFilesFormatByWildcard(FileFormat formatType, const std::wstring& wildcard, const std::wstring& excludeDirectories)
{
	if (formatType == unknown)
	{
		PrintWrongFileFormatMessage();
		return;
	}

	std::wstring directory;
	std::wstring filemask(wildcard);

	std::wstring::size_type pos = wildcard.rfind(L"\\");
	if (pos != std::wstring::npos)
	{
		directory = wildcard.substr(0, pos+1);
		filemask = wildcard.substr(pos+1);
	}

	SearchFilesInDirectory(formatType, directory, filemask, L".;..;" + excludeDirectories);
}

void VerifyFileContent(FileFormat formatType, const std::wstring& filename, LARGE_INTEGER filesize); // TODO: remove formatType from here
bool IsFileMatchesMask(const std::wstring& name, const std::wstring& mask);
bool IsDirectoryAllowed(const std::wstring& name, const std::wstring& excludes);

void SearchFilesInDirectory(
	FileFormat formatType, const std::wstring& directory, const std::wstring& filemask, const std::wstring& excludeDirectories
)
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	HANDLE hFind = FindFirstFile((directory + L"*").c_str(), &ffd);

	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::wstring subdirectory = ffd.cFileName;
				if ( IsDirectoryAllowed(subdirectory, excludeDirectories) )
				{
					SearchFilesInDirectory(formatType, directory + subdirectory + L"\\", filemask, excludeDirectories);
				}
			}
			else if (IsFileMatchesMask(ffd.cFileName, filemask))
			{
				filesize.LowPart = ffd.nFileSizeLow;
				filesize.HighPart = ffd.nFileSizeHigh;
				VerifyFileContent(formatType, directory + ffd.cFileName, filesize); // TODO: replace it from here
			}
		}
		while (FindNextFile(hFind, &ffd) != 0);
	} 

	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES) 
	{
		PrintErrorWhileFilesFindingMessage();
	}

	FindClose(hFind);
}

bool IsDirectoryAllowed(const std::wstring& name, const std::wstring& excludes)
{
	// it works for masks like: "dir_name", "dir1_name;dir2_name", "dir1_name;dir2_name;dir3_name"
	std::vector<std::wstring> directoryNames;

	std::wstring::size_type leftPos = 0; 
	std::wstring::size_type rightPos = std::wstring::npos; 
	do
	{
		rightPos = excludes.find(L";", leftPos); 
		directoryNames.push_back(excludes.substr(leftPos, rightPos - leftPos));
		leftPos = rightPos + 1;
	}
	while (rightPos != std::wstring::npos);

	for (int i = 0; i < (int)directoryNames.size(); ++i)
	{
		if (name == directoryNames[i])
			return false;
	}
	return true;
}

bool IsFileMatchesMask(const std::wstring& name, const std::wstring& mask)
{
	// it works for masks like: "*.as", "*.cpp;*.h", "*.txt;*.doc;*.rtf"
	// WARNING: it doesn't support any masks you want. but it could be extended.
	// //TODO: implement universal mask matching

	std::vector<std::wstring> extensions;

	std::wstring::size_type leftPos = 0; 
	std::wstring::size_type rightPos = std::wstring::npos; 
	do
	{
		rightPos = mask.find(L";", leftPos); 
		extensions.push_back(mask.substr(leftPos + 1, rightPos - leftPos - 1));
		leftPos = rightPos + 1;
	}
	while (rightPos != std::wstring::npos);

	for (int i = 0; i < (int)extensions.size(); ++i)
	{
		std::wstring::size_type extPos = name.rfind( extensions[i] );
		if ( extPos != std::wstring::npos && extPos + extensions[i].length() == name.length())
			return true;
	}
	return false;
}

void VerifyFileContent(FileFormat formatType, const std::wstring& filename, LARGE_INTEGER filesize)
{
	const __int64 maxFileSizeToVerify = 1024*1024*1024;
	if (filesize.QuadPart > maxFileSizeToVerify)
	{
		PrintErrorTooLargeFileMessage();
		return;
	}

	class FileContent
	{
	private:
		unsigned char * byteContent;
		unsigned int byteSize;
	public:
		FileContent(const std::wstring& filename, int filesize)
		{
			byteContent = 0;
			byteSize = filesize;
			ReadFromFile(filename);
		}
		~FileContent()
		{
			if (byteContent)
			{
				delete [] byteContent;
				byteContent = 0;
			}
			byteSize = 0;
		}

		int Size()
		{
			return byteSize;
		}

		const unsigned char * Get()
		{
			return byteContent;
		}
	protected:
		void ReadFromFile(const std::wstring& filename)
		{
			if (byteSize == 0)
				return;
			HANDLE hFile = CreateFile(
				filename.c_str(),
				GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
			);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				byteContent = new unsigned char[byteSize+1];
				unsigned long byteActualRead;
				ReadFile(hFile, byteContent, byteSize, &byteActualRead, NULL);
			}
			CloseHandle(hFile);
		}
	};

	int limitedFilesize = (int)filesize.QuadPart;
	FileContent fileContent(filename, limitedFilesize);
	if (fileContent.Size() == 0)
		return;

	bool verifyResult = false;
	std::vector<int> errorPositions;
	// // TODO: use function pointer instead of switch-case
	switch (formatType)
	{
	case ASCII: 
		verifyResult = ascii::Verify(fileContent.Get(), fileContent.Size(), errorPositions);
		break;
	case UTF8: 
		verifyResult = utf8::Verify(fileContent.Get(), fileContent.Size(), errorPositions);
		break;
	case UTF16: 
		verifyResult = utf16::Verify(fileContent.Get(), fileContent.Size(), errorPositions);
		break;
	}

	if (!verifyResult)
	{
		std::wstring fileFormatString = L"unknown";
		switch (formatType)
		{
		case ASCII: 
			fileFormatString = L"ASCII";
			break;
		case UTF8: 
			fileFormatString = L"UTF-8";
			break;
		case UTF16: 
			fileFormatString = L"UTF-16";
			break;
		}
		PrintErrorFileFormatValidationFailedMessage(filename, fileFormatString, errorPositions);
	}
}
