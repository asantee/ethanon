#ifndef PLATFORM_MACOSX_H_
#define PLATFORM_MACOSX_H_

#import <Foundation/Foundation.h>

#include "../NativeCommandAssembler.h"
#include "../NativeCommandForwarder.h"
#include "../FileLogger.h"

namespace Platform {

bool CreateDirectoryNS(NSString* dir);
NSString* AppNameFromBundle();
gs2d::str_type::string ResourceDirectory();
gs2d::str_type::string GlobalExternalStorageDirectory();
gs2d::str_type::string ExternalStorageDirectory();

} // namespace platform

#endif
