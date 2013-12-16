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

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "../Types.h"

#include <vector>

namespace Platform {

std::string GetFileName(const std::string& source);
std::string GetFileDirectory(const char* source);
std::string AddLastSlash(const std::string& path);
std::string& FixSlashes(std::string& path);
std::string& FixSlashes(std::string& path, const char slash);
std::string& FixSlashesForUnix(std::string& path);

bool CreateDirectory(const std::string& path);
std::vector<gs2d::str_type::string> SplitString(gs2d::str_type::string str, const gs2d::str_type::string& c);

char GetDirectorySlashA();
gs2d::str_type::string GetModuleDirectory();
gs2d::str_type::string RemoveExtension(const gs2d::str_type::char_t* source);
bool IsExtensionRight(const gs2d::str_type::string& fileName, const gs2d::str_type::string& ext);

short ShortEndianSwap(const short s);

} 
// namespace Platform

#endif
