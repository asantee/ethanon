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

// TODO implement wchar_t support on Android... not yet supported by the NDK
#if !defined(ANDROID) && !defined(APPLE_IOS)
#include <xstring>
#endif

#include <string>

namespace Platform {

gs2d::str_type::string GetFileName(const gs2d::str_type::string &source);

// TODO implement wchar_t support on Android... not yet supported by the NDK
#if !defined(ANDROID) && !defined(APPLE_IOS)
std::wstring AddLastSlash(const std::wstring& path);
std::string AddLastSlash(const std::string& path);
std::wstring& FixSlashes(std::wstring& path);
std::string& FixSlashes(std::string& path);

gs2d::str_type::string GetModulePath();
gs2d::str_type::string GetCurrentDirectoryPath();
std::wstring ConvertUtf8ToUnicode(const char* utf8String); // use it in low-level only. utf8::converter is a high-level wrapper
std::string ConvertUnicodeToUtf8(const wchar_t* unicodeString); // use it in low-level only. utf8::converter is a high-level wrapper
std::wstring ConvertAsciiToUnicode(const char* asciiString); // use it in low-level only. utf8::converter is a high-level wrapper
std::string ConvertUnicodeToAscii(const wchar_t* unicodeString); // use it in low-level only. utf8::converter is a high-level wrapper
std::wstring GetFilePath(const wchar_t *source);

#else
gs2d::str_type::string GetFilePath(const char *source);
gs2d::str_type::string AddLastSlash(const gs2d::str_type::string& path);
gs2d::str_type::string& FixSlashes(gs2d::str_type::string& path);
gs2d::str_type::string GetModulePath();
gs2d::str_type::string GetCurrentDirectoryPath();
#endif // #ifndef ANDROID
} 
// namespace Platform

#endif
