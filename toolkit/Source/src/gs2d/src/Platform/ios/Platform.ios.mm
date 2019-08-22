#import "Platform.ios.h"

#import "../Platform.h"

#import "../../Video/GLES2/GLES2Video.h"

#import <Foundation/Foundation.h>

std::string gs2d::Application::GetPlatformName()
{
	return "ios";
}

void gs2d::ShowMessage(std::stringstream& stream, const GS_MESSAGE_TYPE type)
{
	if (type == GSMT_ERROR)
	{
		std::cerr << stream.str() << std::endl;
	}
	else
	{
		std::cout << stream.str() << std::endl;
	}
}

namespace Platform {

std::string ResourceDirectory()
{
	@autoreleasepool {
		NSString* resourceDir = [[NSBundle mainBundle] resourcePath];
		resourceDir = [resourceDir stringByAppendingString:@"/assets/"];

		const std::string r = [resourceDir cStringUsingEncoding:1];
		return r;
	}
}

std::string ExternalStorageDirectory()
{
	@autoreleasepool {
		NSString* externalStorageDir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject];

		// apprend proc name plus slash
		externalStorageDir = [externalStorageDir stringByAppendingString:@"/ethdata/"];
		externalStorageDir = [externalStorageDir stringByAppendingPathComponent:[[NSProcessInfo processInfo] processName]];
		externalStorageDir = [externalStorageDir stringByAppendingString:@"/"];

		const std::string r = [externalStorageDir cStringUsingEncoding:1];
		return r;
	}
}

std::string GlobalExternalStorageDirectory()
{
	return ExternalStorageDirectory();
}

std::string GetModuleDirectory()
{
	/*NSString* bundleDir = [[NSBundle mainBundle] bundlePath];
	bundleDir = [bundleDir stringByAppendingString:@"/"];
	return [bundleDir cStringUsingEncoding:1];*/
	return ResourceDirectory();
}

bool CreateDirectoryNS(NSString* dir)
{
	@autoreleasepool {
		NSFileManager* fileManager = [NSFileManager defaultManager];
		const bool r = ([fileManager createDirectoryAtPath:dir withIntermediateDirectories:YES attributes:nil error:nil] == YES);
		return r;
	}
}

bool CreateDirectory(const std::string& path)
{
	return CreateDirectoryNS([NSString stringWithUTF8String:path.c_str()]);
}

std::string Platform::FileLogger::GetLogDirectory()
{
	std::string logPath(ExternalStorageDirectory());
	logPath += "log/";
	return logPath;
}

char GetDirectorySlashA()
{
	return '/';
}

} // namespace
