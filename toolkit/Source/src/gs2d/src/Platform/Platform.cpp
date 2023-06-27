#include "Platform.h"

#include <boost/shared_ptr.hpp>

#include <iostream>

#include <hashlibpp.h>

namespace Platform {

std::string GetFileName(const std::string& source)
{
	std::string r = source;
	FixSlashes(r);
	const std::size_t pos = r.rfind(GetDirectorySlashA());
	if (pos != std::string::npos)
	{
		r = r.substr(pos + 1, std::string::npos);
	}
	return r;
}

std::string GetFileDirectory(const char *source)
{
	std::string r = source;
	FixSlashes(r);
	const std::size_t pos = r.rfind(GetDirectorySlashA());
	if (pos != std::string::npos)
	{
		r = r.substr(0, pos + 1);
	}
	return r;
}

std::string& FixSlashes(std::string& path, const char slash)
{
	const std::size_t size = path.size();
	for (std::size_t t = 0; t < size; t++)
	{
		if (path[t] == '/' || path[t] == '\\')
			path[t] = slash;
	}
	return path;
}

std::string& FixSlashes(std::string& path)
{
	return FixSlashes(path, GetDirectorySlashA());
}

std::string& FixSlashesForUnix(std::string& path)
{
	return FixSlashes(path, '/');
}

std::string AddLastSlash(const std::string& path)
{
	if (path.empty())
	{
		return "";
	}
	std::string r = path;
	FixSlashes(r);
	const std::size_t lastChar = r.size() - 1;
	if (r.at(lastChar) != GetDirectorySlashA())
	{
		r.resize(lastChar + 2);
		r[lastChar + 1] = GetDirectorySlashA();
	}
	return r;
}

std::string RemoveExtension(const std::string& source)
{
	std::string result = source;
	const int max = static_cast<int>(result.length()) - 1;
	for (int t = max - 1; t >= 0; t--)
	{
		if (source[t] == Platform::GetDirectorySlashA())
		{
			return source;
		}

		if (source[t] == ('.'))
		{
			result.resize(t);
			break;
		}
	}
	return result;
}

std::string RemoveExtension(const char* source)
{
	return RemoveExtension(std::string(source));
}

bool IsExtensionRight(const std::string& fileName, const std::string& ext)
{
	const std::size_t pos = fileName.rfind(ext);
	return (fileName.size() - pos == ext.size());
}

short ShortEndianSwap(const short s)
{
	unsigned char b1, b2;
	b1 = s & 255;
	b2 = (s >> 8) & 255;
	return (b1 << 8) + b2;
}

std::vector<std::string> SplitString(std::string str, const std::string& c)
{
	std::vector<std::string> v;
	std::size_t pos;
	while ((pos = str.find(c)) != std::string::npos)
	{
		v.push_back(str.substr(0, pos));
		str = str.substr(pos + c.length(), std::string::npos);
	}
	v.push_back(str);
	return v;
}

std::string GetMD5HashFromString(const std::string& str)
{
	hashwrapper *wrapper = new md5wrapper();
	std::string r;
	try
	{
		r = wrapper->getHashFromString(str);
	}
	catch (hlException &e)
	{
		std::cerr << "hashlib++ Error: ("  << e.error_number() << "): " << e.error_message() << std::endl;
	}

	delete wrapper;
	return r;
}

std::string GetSHA1HashFromString(const std::string& str)
{
	hashwrapper *wrapper = new sha1wrapper();
	std::string r;
	try
	{
		r = wrapper->getHashFromString(str);
	}
	catch (hlException &e)
	{
		std::cerr << "hashlib++ Error: ("  << e.error_number() << "): " << e.error_message() << std::endl;
	}

	delete wrapper;
	return r;
}

class CRCTable
{
	uint32_t m_crcTable[256];

public:
	CRCTable()
	{
		for (uint32_t i = 0; i < 256; ++i)
		{
			uint32_t crcValue = i;
			for (int j = 0; j < 8; ++j)
			{
				crcValue = (crcValue & 1) ? (crcValue >> 1) ^ 0xEDB88320 : crcValue >> 1;
			}
			m_crcTable[i] = crcValue;
		}
	}
	
	const uint32_t* Get() const
	{
		return m_crcTable;
	}
};

CRCTable g_crcTable;

uint32_t GetFastHashFromBuffer(const uint8_t* data, const size_t size)
{
	uint32_t crc = 0xFFFFFFFF;

	// Compute CRC using lookup table
	for (size_t i = 0; i < size; ++i)
	{
		crc = (crc >> 8) ^ g_crcTable.Get()[(crc ^ data[i]) & 0xFF];
	}

	return crc ^ 0xFFFFFFFF;
}

uint32_t GetFastHashFromString(const std::string& str)
{
	const uint8_t* data = reinterpret_cast<const uint8_t*>(str.data());
	const size_t size = str.length();
	return GetFastHashFromBuffer(data, size);
}

uint32_t GetFastHashFromFloat(const float v)
{
	const uint8_t* data = reinterpret_cast<const uint8_t*>(&v);
	const size_t size = sizeof(float);
	return GetFastHashFromBuffer(data, size);
}

} // namespace Platform
