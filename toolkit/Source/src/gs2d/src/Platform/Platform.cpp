/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

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

gs2d::str_type::string RemoveExtension(const gs2d::str_type::char_t* source)
{
	gs2d::str_type::string dest = source;
	const std::size_t len = dest.length();
	for (std::size_t t = 0; t < len; t++)
	{
		if (source[t] == GS_L('.'))
		{
			dest.resize(t);
			break;
		}
	}
	return dest;
}

bool IsExtensionRight(const gs2d::str_type::string& fileName, const gs2d::str_type::string& ext)
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

std::vector<gs2d::str_type::string> SplitString(gs2d::str_type::string str, const gs2d::str_type::string& c)
{
	std::vector<gs2d::str_type::string> v;
	std::size_t pos;
	while ((pos = str.find(c)) != gs2d::str_type::string::npos)
	{
		v.push_back(str.substr(0, pos));
		str = str.substr(pos + c.length(), gs2d::str_type::string::npos);
	}
	v.push_back(str);
	return v;
}

#ifdef GS2D_STR_TYPE_WCHAR
	std::wstring GetFileName(const std::wstring& source)
	{
		std::wstring r = source;
		FixSlashes(r);
		const std::size_t pos = r.rfind(GetDirectorySlash());
		if (pos != std::wstring::npos)
		{
			r = r.substr(pos + 1, std::wstring::npos);
		}
		return r;
	}

	std::wstring GetFileDirectory(const wchar_t *source)
	{
		std::wstring r = source;
		FixSlashes(r);
		const std::size_t pos = r.rfind(GetDirectorySlash());
		if (pos != std::wstring::npos)
		{
			r = r.substr(0, pos + 1);
		}
		return r;
	}

	std::wstring& FixSlashes(std::wstring& path, const wchar_t slash)
	{
		const std::size_t size = path.size();
		for (std::size_t t = 0; t < size; t++)
		{
			if (path[t] == L'/' || path[t] == L'\\')
				path[t] = slash;
		}
		return path;
	}

	std::wstring& FixSlashes(std::wstring& path)
	{
		return FixSlashes(path, GetDirectorySlashW());
	}

	std::wstring& FixSlashesForUnix(std::wstring& path)
	{
		return FixSlashes(path, L'/');
	}

	std::wstring AddLastSlash(const std::wstring& path)
	{
		if (path.empty())
		{
			return L"";
		}
		std::wstring r = path;
		FixSlashes(r);
		const std::size_t lastChar = r.size() - 1;
		if (r.at(lastChar) != GetDirectorySlashW())
		{
			r.resize(lastChar + 2);
			r[lastChar + 1] = GetDirectorySlashW();
		}
		return r;
	}

#endif

} // namespace Platform
