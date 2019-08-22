#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <vector>
#include <string>

namespace Platform {

std::string GetFileName(const std::string& source);
std::string GetFileDirectory(const char* source);
std::string AddLastSlash(const std::string& path);
std::string& FixSlashes(std::string& path);
std::string& FixSlashes(std::string& path, const char slash);
std::string& FixSlashesForUnix(std::string& path);

bool CreateDirectory(const std::string& path);
std::vector<std::string> SplitString(std::string str, const std::string& c);

char GetDirectorySlashA();
std::string GetModuleDirectory();
std::string RemoveExtension(const char* source);
bool IsExtensionRight(const std::string& fileName, const std::string& ext);

short ShortEndianSwap(const short s);

} 
// namespace Platform

#endif
