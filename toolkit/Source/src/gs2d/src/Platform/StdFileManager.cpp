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

#include "StdFileManager.h"

#include <fstream>
#include <vector>

#include "../Unicode/utf8/utf8.h"

using namespace gs2d;

namespace Platform {

static FILE* LoadFile(const str_type::string& fileName)
{
	FILE* file = 0;
	#ifdef GS2D_STR_TYPE_WCHAR
	 #if _MSC_VER >= 1500
	  _wfopen_s(&file, fileName.c_str(), L"rb");
	 #else
	  file = wfopen(fileName.c_str(), L"rb");
	 #endif
	#else
	 file = fopen(fileName.c_str(), "rb");
	#endif
	return file;
}

bool StdFileManager::IsLoaded() const
{
	return true;
}

bool StdFileManager::GetFileBuffer(const str_type::string &fileName, FileBuffer &out)
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

bool StdFileManager::GetUTF8BOMFileString(const str_type::string &fileName, str_type::string &out)
{
#ifdef GS2D_STR_TYPE_ANSI
	static bool wideChar = false;
#else
	static bool wideChar = true;
#endif

	FileBuffer buffer;
	if (GetFileBuffer(fileName, buffer))
	{
		str_type::stringstream ss;
		unsigned char *adr = buffer->GetAddress();
		const std::size_t bufferSize = buffer->GetBufferSize();

		for (unsigned long t = 3; t < bufferSize; t++)
		{
			const bool isLast = (t == bufferSize - 1);
			const unsigned char* ch = static_cast<unsigned char*>(&adr[t]);

			// if the character is valid UTF or we're already using 1 byte per character
			// simply append it to the final string
			if (utf8::is_valid(ch, ch + 1) || !wideChar)
			{
				ss << static_cast<char>(ch[0]);
			}
			// otherwise, is it is not valid and we're using 2+ bytes per char
			// convert it into UTF-16 and append
			else if (!isLast && wideChar)
			{
				if (utf8::is_valid(ch, ch + 2))
				{
					std::vector<unsigned short> utf16line;
					utf8::utf8to16(ch, ch + 2, std::back_inserter(utf16line));
					ss << static_cast<str_type::char_t>(utf16line[0]);
					t++;
				}
			}
		}
		out = ss.str();
		return true;
	}
	else
	{
		return false;
	}
}

bool StdFileManager::GetAnsiFileString(const str_type::string &fileName, str_type::string &out)
{
	FileBuffer buffer;
	if (GetFileBuffer(fileName, buffer))
	{
		str_type::stringstream ss;
		unsigned char *adr = buffer->GetAddress();
		for (unsigned long t = 0; t < buffer->GetBufferSize(); t++)
		{
			const char c = static_cast<char>(adr[t]);
			if (c != 0x0D) // remove all carriage return
				ss << c;
		}
		out = ss.str();
		return true;
	}
	else
	{
		return false;
	}
}

bool StdFileManager::GetUTF16FileString(const str_type::string &fileName, str_type::string &out)
{
	FILE* file = LoadFile(fileName);
	if (!file)
	{
		return false;
	}

	long length = 0;

	const std::size_t BOMsize = 2;
	const std::size_t charSize = 2;

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (length <= 0)
	{
		return false;
	}

	unsigned short usOrder = 0;
	fread(&usOrder, BOMsize, 1, file);

	out.clear();
	unsigned short currChar = 0;
	while (fread(&currChar, charSize, 1, file) == 1)
	{
		str_type::stringstream ss;
		const str_type::char_t ch = static_cast<str_type::char_t>(currChar);
		ss << ch;
		out = out.append(ss.str());
	}
	fclose(file);
	return true;
}

bool StdFileManager::FileExists(const gs2d::str_type::string& fileName) const
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
