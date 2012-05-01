/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

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

using namespace gs2d;

namespace Platform {

bool StdFileManager::IsLoaded() const
{
	return true;
}

bool StdFileManager::GetFileBuffer(const str_type::string &fileName, FileBuffer &out)
{
	#if _MSC_VER >= 1500
		FILE *f = 0;
		_wfopen_s(&f, fileName.c_str(), GS_L("rb"));
	#else
		FILE *f = _wfopen(fileName.c_str(), GS_L("rb"));
	#endif

	if (!f)
	{
		return false;
	}
	
	fseek(f, 0, SEEK_END);
	const std::size_t len = ftell(f);
	fseek(f, 0, SEEK_SET);

	out = FileBuffer(new _FileBuffer<unsigned char>(len));
	fread(out->GetAddress(), len, 1, f);
	fclose(f);
	return true;
}

bool StdFileManager::GetUTF8BOMFileString(const str_type::string &fileName, str_type::string &out)
{
	FileBuffer buffer;
	if (GetFileBuffer(fileName, buffer))
	{
		// TODO optimize it
		str_type::stringstream ss;
		unsigned char *adr = buffer->GetAddress();
		for (int t=3; t<buffer->GetBufferSize(); t++)
		{
			ss << static_cast<char>(adr[t]);
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
		// TODO optimize it
		str_type::stringstream ss;
		unsigned char *adr = buffer->GetAddress();
		for (int t=0; t<buffer->GetBufferSize(); t++)
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
	FILE* file = 0;
	errno_t err = _wfopen_s(&file, fileName.c_str(), L"rb");
	if (err || !file)
	{
		return false;
	}

	long length = 0;

	// const std::size_t wcharSize = sizeof(wchar_t);
	const std::size_t BOMsize = 2;

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (length <= 0)
	{
		return false;
	}

	out.resize(length);
	unsigned short usOrder;
	fread(&usOrder, BOMsize, 1, file);
	if (fread(&out[0], length - BOMsize, 1, file) != 1)
	{
		fclose(file);
		return false;
	}
	fclose(file);
	return true;
}

bool StdFileManager::FileExists(const gs2d::str_type::string& fileName) const
{
	FILE* file = 0;
	errno_t err = _wfopen_s(&file, fileName.c_str(), L"rb");
	if (err || !file)
	{
		return false;
	}
	else
	{
		fclose(file);
		return true;
	}
}

}