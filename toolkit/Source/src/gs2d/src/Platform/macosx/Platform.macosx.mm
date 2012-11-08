/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#include "Platform.macosx.h"
#include "../Platform.h"
#include "../../Video/GLES2/GLES2Video.h"

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
	return "macosx";
}

namespace Platform {

gs2d::str_type::string GetModuleDirectory()
{
	NSString* bundleDir = [[NSBundle mainBundle] bundlePath];
	bundleDir = [bundleDir stringByAppendingString:@"/"];
	return [bundleDir cStringUsingEncoding:1];
}

bool CreateDirectoryNS(NSString* dir)
{
	NSFileManager* fileManager = [NSFileManager defaultManager];
	return ([fileManager createDirectoryAtPath:dir withIntermediateDirectories:YES attributes:nil error:nil] == YES);
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
