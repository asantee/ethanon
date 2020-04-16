#ifndef PLATFORM_IOS_H_
#define PLATFORM_IOS_H_

#include "../StdFileManager.h"
#include "../NativeCommandAssembler.h"
#include "../NativeCommandForwarder.h"
#include "../FileLogger.h"

namespace Platform {
	std::string ResourceDirectory();
	std::string ExternalStorageDirectory();
	std::string GlobalExternalStorageDirectory();
	bool CreateDirectory(const std::string& path);
} // namespace

#endif
