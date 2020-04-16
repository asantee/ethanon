#ifndef PLATFORM_MACOSX_H_
#define PLATFORM_MACOSX_H_

#import <Foundation/Foundation.h>

#include "../NativeCommandAssembler.h"
#include "../NativeCommandForwarder.h"
#include "../FileLogger.h"

namespace Platform {

bool CreateDirectoryNS(NSString* dir);
NSString* AppNameFromBundle();
std::string ResourceDirectory();
std::string GlobalExternalStorageDirectory();
std::string ExternalStorageDirectory();

} // namespace platform

#endif
