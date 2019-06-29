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
