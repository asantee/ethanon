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

#include "FileManager.h"

#include <fstream>

using namespace gs2d;

namespace Platform {

bool FileManager::ConvertAnsiFileToUTF16LE(const gs2d::str_type::string& fileName)
{
	str_type::string content;
	if (!GetAnsiFileString(fileName, content))
	{
		return false;
	}

	const unsigned short utf16bom = 0xFEFF;
	str_type::ofstream ofs(fileName.c_str(), std::ios::out | std::ios::binary);
	if (ofs.is_open())
	{
		ofs.write((str_type::char_t*)&utf16bom, 2);

		const std::size_t length = content.length();
		for (std::size_t t = 0; t < length; t++)
		{
			ofs.write((str_type::char_t*)&content[t], 1);
			const char c = '\0';
			ofs.write((str_type::char_t*)&c, 1);
		}
		ofs.close();
		return true;
	}
	else
	{
		return false;
	}
}

} // namespace Platform
