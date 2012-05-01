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

#include "StdAnsiFileManager.h"
#include <fstream>

using namespace gs2d;

namespace Platform {

StdAnsiFileManager::StdAnsiFileManager(const gs2d::str_type::string& resourcePath) :
	m_resourcePath(resourcePath)
{
}

bool StdAnsiFileManager::IsLoaded() const
{
	return true;
}

bool StdAnsiFileManager::GetFileBuffer(const str_type::string &fileName, FileBuffer &out)
{
	const str_type::string fullPath(m_resourcePath + fileName);
	FILE *f = fopen(fullPath.c_str(), GS_L("r"));

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

bool StdAnsiFileManager::GetUTF8BOMFileString(const str_type::string &fileName, str_type::string &out)
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

bool StdAnsiFileManager::GetAnsiFileString(const str_type::string &fileName, str_type::string &out)
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

bool StdAnsiFileManager::GetUTF16FileString(const str_type::string &fileName, str_type::string &out)
{
	FileBuffer utf16Buffer;
	if (GetFileBuffer(fileName, utf16Buffer))
	{
		// TODO/TO-DO: handle big-endian encoding too
		str_type::stringstream ss;
		const unsigned char *adr = utf16Buffer->GetAddress();
		for (int t = 2; t < utf16Buffer->GetBufferSize(); t += 2)
		{
			ss << adr[t];
		}
		out = ss.str();
		return true;
	}
	else
	{
		return false;
	}
	return true;
}

bool StdAnsiFileManager::FileExists(const gs2d::str_type::string& fileName) const
{
	const str_type::string fullPath(m_resourcePath + fileName);
	FILE *f = fopen(fullPath.c_str(), GS_L("rb"));
	if (!f)
	{
		return false;
	}
	else
	{
		fclose(f);
		return true;
	}
}
	
}