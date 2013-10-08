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

#include "FileManager.h"

#include <fstream>

#include "../Unicode/utf8/utf8.h"

using namespace gs2d;

namespace Platform {

const unsigned short FileManager::UTF16LE_BOM = 0xFEFF;

bool FileManager::GetUTF8FileString(const str_type::string &fileName, str_type::string &out)
{
	FileBuffer buffer;
	if (GetFileBuffer(fileName, buffer))
	{
		str_type::stringstream ss;
		unsigned char *adr = buffer->GetAddress();
		const std::size_t bufferSize = buffer->GetBufferSize();
		
		if (bufferSize == 0)
			return false;

		// check for UTF8 BOM
		std::size_t firstChar = 0;
		if (utf8::starts_with_bom(&adr[0], &adr[bufferSize - 1]))
		{
			firstChar = 3;
		}

		utf8::replace_invalid(&adr[firstChar], &adr[bufferSize - 1], back_inserter(out));
		return true;
	}
	else
	{
		return false;
	}
}

bool FileManager::GetUTF16FileString(const str_type::string &fileName, str_type::string &out)
{
	FileBuffer buffer;
	if (GetFileBuffer(fileName, buffer))
	{
		const std::size_t bufferSize = buffer->GetBufferSize();

		if (bufferSize == 0)
			return false;
	
		const unsigned char* adr = (buffer->GetAddress());
		utf8::utf16to8(&adr[0], &adr[bufferSize - 1], back_inserter(out));
		return true;
	}
	else
	{
		return false;
	}
}

bool FileManager::GetAnsiFileString(const str_type::string &fileName, str_type::string &out)
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

} // namespace Platform
