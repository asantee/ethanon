#include "Platform.h"
#include <iostream>

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

std::string RemoveExtension(const char* source)
{
	std::string dest = source;
	const int max = static_cast<int>(dest.length()) - 1;
	for (int t = max - 1; t >= 0; t--)
	{
		if (source[t] == Platform::GetDirectorySlashA())
			return source;

		if (source[t] == ('.'))
		{
			dest.resize(t);
			break;
		}
	}
	return dest;
}

bool IsExtensionRight(const std::string& fileName, const std::string& ext)
{
	const std::size_t pos = fileName.rfind(ext);
	if (fileName.size() - pos == ext.size())
	{
		return true;
	}
	else
	{
		return false;
	}
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

} // namespace Platform
