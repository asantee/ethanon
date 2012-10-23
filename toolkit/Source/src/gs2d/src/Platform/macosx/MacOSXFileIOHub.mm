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

#include "MacOSXFileIOHub.h"
#include "../Platform.h"

namespace Platform {

#warning TO-DO move to platform
static NSString* CreateDirectory(NSString* dir)
{
	NSFileManager* fileManager = [NSFileManager defaultManager];
	[fileManager createDirectoryAtPath:dir withIntermediateDirectories:YES attributes:nil error:nil];
	return dir;
}

static gs2d::str_type::string GetResourceDirectory()
{
	NSString* resourceDir = [[NSBundle mainBundle] bundlePath];
	resourceDir = [resourceDir stringByAppendingString:@"/assets/"];
	CreateDirectory(resourceDir);
	return [resourceDir cStringUsingEncoding:1];
}

static gs2d::str_type::string GetGlobalExternalStorageDirectory()
{
	NSString* globalExternalStorageDir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];

	// apprend proc name plus slash
	globalExternalStorageDir = [globalExternalStorageDir stringByAppendingPathComponent:[[NSProcessInfo processInfo] processName]];
	globalExternalStorageDir = [globalExternalStorageDir stringByAppendingString:@"/"];
	return [globalExternalStorageDir cStringUsingEncoding:1];
}

static gs2d::str_type::string GetExternalStorageDirectory()
{
	NSString* externalStorageDir = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0];

	// apprend proc name plus slash
	externalStorageDir = [externalStorageDir stringByAppendingPathComponent:[[NSProcessInfo processInfo] processName]];
	externalStorageDir = [externalStorageDir stringByAppendingString:@"/"];
	return [externalStorageDir cStringUsingEncoding:1];
}

MacOSXFileIOHub::MacOSXFileIOHub(
	Platform::FileManagerPtr fileManager,
	const gs2d::str_type::string& bitmapFontSearchDirectory) :
	FileIOHub(
		fileManager, Platform::GetResourceDirectory(),
		GetModuleDirectory(),
		Platform::GetExternalStorageDirectory(),
		Platform::GetGlobalExternalStorageDirectory(),
		bitmapFontSearchDirectory)
{
	CreateDirectory([NSString stringWithCString:GetExternalStorageDirectory().c_str() encoding:NSUTF8StringEncoding]);
}

gs2d::str_type::string MacOSXFileIOHub::GetGlobalExternalStorageDirectory() const
{
	CreateDirectory([NSString stringWithCString:GetGlobalExternalStorageDirectory().c_str() encoding:NSUTF8StringEncoding]);	
	return FileIOHub::GetGlobalExternalStorageDirectory();
}

void MacOSXFileIOHub::SetFileManager(Platform::FileManagerPtr fileManager, const gs2d::str_type::string& resourceDirectory)
{
    // not supported
}

bool MacOSXFileIOHub::IsResourcePackingSupported()
{
	return false;
}

} // namespace Platform
