#ifndef FILE_LISTING_H_
#define FILE_LISTING_H_

#include <Platform/Platform.h>
#include <vector>
#include <iostream>

namespace Platform {

class FileListing
{
public:
	struct FILE_NAME
	{
		inline void GetFullFileName(std::string &sOut)
		{
			sOut = dir + file;
		}
		std::string dir;
		std::string file;
	};

	// Should be implemented in file FileListing.{platform}.cpp
	bool ListDirectoryFiles(const char* directory, const char* extension);

	inline unsigned int GetNumFiles()
	{
		return static_cast<unsigned int>(m_fileName.size());
	}

	inline bool GetFileName(const unsigned int index, FILE_NAME& fileName)
	{
		if (index >= GetNumFiles())
		{
			std::cerr << ("FileListing::GetFileName - index >= m_fileName.size().\n");
			return false;
		}
		fileName = m_fileName[index];
		return true;
	}

private:
	std::vector<FILE_NAME> m_fileName;
};

} // namespace Platform

#endif
