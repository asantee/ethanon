#ifndef PLATFORM_IOS_H_
#define PLATFORM_IOS_H_

#include "../StdFileManager.h"
#include "../NativeCommandAssembler.h"
#include "../NativeCommandForwarder.h"
#include "../FileLogger.h"

namespace Platform {
	gs2d::str_type::string ResourceDirectory();
	gs2d::str_type::string ExternalStorageDirectory();
	gs2d::str_type::string GlobalExternalStorageDirectory();
	bool CreateDirectory(const std::string& path);
} // namespace

#endif
