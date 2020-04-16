#include "StdFileManager.h"

#include <fstream>
#include <vector>

namespace Platform {

static FILE* LoadFile(const std::string& fileName)
{
	FILE* file = 0;
#if (__STDC_VERSION__ >= 201112L) || ( _MSC_VER >= 1500 )
	fopen_s(&file, fileName.c_str(), "rb");
#else
	file = fopen(fileName.c_str(), "rb");
#endif
	return file;
}

bool StdFileManager::IsLoaded() const
{
	return true;
}

bool StdFileManager::GetFileBuffer(const std::string &fileName, FileBuffer &out)
{
	FILE* file = LoadFile(fileName);
	if (!file)
	{
		return false;
	}
	
	fseek(file, 0, SEEK_END);
	const std::size_t len = ftell(file);
	fseek(file, 0, SEEK_SET);

	out = FileBuffer(new _FileBuffer<unsigned char>(len));
	fread(out->GetAddress(), len, 1, file);
	fclose(file);
	return true;
}

bool StdFileManager::FileExists(const std::string& fileName) const
{
	FILE* file = LoadFile(fileName);
	if (!file)
	{
		return false;
	}
	else
	{
		fclose(file);
		return true;
	}
}

bool StdFileManager::IsPacked() const
{
	return false;
}

}
