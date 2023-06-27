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

std::vector<std::string> SplitString(std::string str, const std::string& c);

char GetDirectorySlashA();
std::string GetModuleDirectory();
std::string GetModuleName();
std::string RemoveExtension(const std::string& source);
std::string RemoveExtension(const char* source);
bool IsExtensionRight(const std::string& fileName, const std::string& ext);
bool IsDirectory(const std::string& path);

short ShortEndianSwap(const short s);

std::string GetMD5HashFromString(const std::string& str);
std::string GetSHA1HashFromString(const std::string& str);
uint32_t GetFastHashFromString(const std::string& str);
uint32_t GetFastHashFromFloat(const float v);

} 
// namespace Platform

#endif
