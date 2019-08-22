#ifndef PLATFORM_LINUX_H_
#define PLATFORM_LINUX_H_

#include "../NativeCommandAssembler.h"
#include "../NativeCommandForwarder.h"
#include "../FileLogger.h"

namespace Platform {

std::string ResourceDirectory();
std::string GlobalExternalStorageDirectory();
std::string ExternalStorageDirectory();

} // namespace platform

#endif