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
		inline void GetFullFileName(gs2d::str_type::string &sOut)
		{
			sOut = dir + file;
		}
		gs2d::str_type::string dir;
		gs2d::str_type::string file;
	};

	bool ListDirectoryFiles(const gs2d::str_type::char_t* directory, const gs2d::str_type::char_t* extension);

	inline unsigned int GetNumFiles()
	{
		return static_cast<unsigned int>(m_fileName.size());
	}

	inline bool GetFileName(const unsigned int index, FILE_NAME& fileName)
	{
		if (index >= GetNumFiles())
		{
			GS2D_CERR << GS_L("FileListing::GetFileName - index >= m_fileName.size().\n");
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
