#import <AppKit/AppKit.h>

#include <iostream>

#include "Platform.macosx.h"

#include "../Platform.h"

#include "../../Application.h"

void gs2d::ShowMessage(str_type::stringstream& stream, const GS_MESSAGE_TYPE type)
{
	if (type == GSMT_ERROR)
	{
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert addButtonWithTitle:@"OK"];
		[alert setMessageText:[NSString stringWithCString:stream.str().c_str() encoding:NSUTF8StringEncoding]];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert runModal];

		std::cerr << "Error: " << stream.str() << std::endl;
	}
	else
	{
		std::cout << ((type == GSMT_WARNING) ? "Warning: " : "") << stream.str() << std::endl;
	}
}

gs2d::str_type::string gs2d::Application::GetPlatformName()
{
	return "macos";
}

namespace Platform {

NSString* AppNameFromBundle()
{
	NSString* bundleDir = [[NSBundle mainBundle] bundlePath];
	NSString *appName = [[bundleDir lastPathComponent] stringByDeletingPathExtension];

	return appName;
}

gs2d::str_type::string ResourceDirectory()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	NSString* resourceDir = [[NSBundle mainBundle] resourcePath];
	resourceDir = [resourceDir stringByAppendingString:@"/assets/"];

	const gs2d::str_type::string r = [resourceDir cStringUsingEncoding:1];
    [pool release];
	return r;
}

gs2d::str_type::string GlobalExternalStorageDirectory()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	NSString* globalExternalStorageDir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];

	// apprend proc name plus slash
	globalExternalStorageDir = [globalExternalStorageDir stringByAppendingPathComponent:AppNameFromBundle()];
	globalExternalStorageDir = [globalExternalStorageDir stringByAppendingString:@"/"];

	const gs2d::str_type::string r = [globalExternalStorageDir cStringUsingEncoding:1];
    [pool release];
	return r;
}

gs2d::str_type::string ExternalStorageDirectory()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString* externalStorageDir = [NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES) objectAtIndex:0];

	// apprend proc name plus slash
	externalStorageDir = [externalStorageDir stringByAppendingPathComponent:AppNameFromBundle()];
	externalStorageDir = [externalStorageDir stringByAppendingString:@"/"];

	const gs2d::str_type::string r = [externalStorageDir cStringUsingEncoding:1];
    [pool release];
	return r;
}

gs2d::str_type::string GetModuleDirectory()
{
	/*NSString* bundleDir = [[NSBundle mainBundle] bundlePath];
	bundleDir = [bundleDir stringByAppendingString:@"/"];
	return [bundleDir cStringUsingEncoding:1];*/
	return ResourceDirectory();
}

bool CreateDirectoryNS(NSString* dir)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSFileManager* fileManager = [NSFileManager defaultManager];
	const bool r = ([fileManager createDirectoryAtPath:dir withIntermediateDirectories:YES attributes:nil error:nil] == YES);
    [pool release];
	return r;
}

bool CreateDirectory(const std::string& path)
{
	return CreateDirectoryNS([NSString stringWithUTF8String:path.c_str()]);
}

char GetDirectorySlashA()
{
	return '/';
}

} // namespace
