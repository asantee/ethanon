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

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "../gs2dtypes.h"

#ifdef GS2D_STR_TYPE_WCHAR
	#include <xstring>
#endif

#include <string>

namespace Platform {

std::string GetFileName(const std::string& source);
std::string GetFileDirectory(const char* source);
std::string AddLastSlash(const std::string& path);
std::string& FixSlashes(std::string& path);

char GetDirectorySlash();
gs2d::str_type::string GetProgramDirectory();

#ifdef GS2D_STR_TYPE_WCHAR
	std::wstring GetFileName(const std::wstring& source);
	std::wstring GetFileDirectory(const wchar_t* source);
	std::wstring AddLastSlash(const std::wstring& path);
	std::wstring& FixSlashes(std::wstring& path);

	std::wstring ConvertUtf8ToUnicode(const char* utf8String); // use it in low-level only. utf8::converter is a high-level wrapper
	std::string ConvertUnicodeToUtf8(const wchar_t* unicodeString); // use it in low-level only. utf8::converter is a high-level wrapper
	std::wstring ConvertAsciiToUnicode(const char* asciiString); // use it in low-level only. utf8::converter is a high-level wrapper
	std::string ConvertUnicodeToAscii(const wchar_t* unicodeString); // use it in low-level only. utf8::converter is a high-level wrapper

	wchar_t GetDirectorySlashW();
#endif

} 
// namespace Platform

#endif
