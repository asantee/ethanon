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

#import "Platform.ios.h"
#import "../Platform.h"
#import "../../Video/GLES2/gs2dGLES2.h"

gs2d::str_type::string gs2d::GLES2Video::GetPlatformName() const
{
	return "ios";
}

// it will be implemented here for the boost timer is presenting strange behaviour
float gs2d::GLES2Video::GetElapsedTimeF(const TIME_UNITY unity) const
{
	return static_cast<float>(GetElapsedTime(unity));
}

unsigned long gs2d::GLES2Video::GetElapsedTime(const TIME_UNITY unity) const
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	const double time = [[NSDate date] timeIntervalSince1970] - Platform::ios::StartTime::m_startTime;

	double elapsedTimeMS = time * 1000.0;
	switch (unity)
	{
		case TU_HOURS:
			elapsedTimeMS /= 1000.0;
			elapsedTimeMS /= 60.0;
			elapsedTimeMS /= 60.0;
			break;
		case TU_MINUTES:
			elapsedTimeMS /= 1000.0;
			elapsedTimeMS /= 60.0;
			break;
		case TU_SECONDS:
			elapsedTimeMS /= 1000.0;
			break;
		case TU_MILLISECONDS:
		default:
			break;
	};
	[pool release];
	return static_cast<unsigned long>(elapsedTimeMS);
}

namespace Platform {

namespace ios {
	double StartTime::m_startTime = 0;
}

gs2d::str_type::string Platform::FileLogger::GetLogPath()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString* dir = NSHomeDirectory();
	const char* szDir = [dir cStringUsingEncoding:1];
	std::string logPath(szDir);
	logPath += "/log/";

    [pool release];
	return logPath;
}

gs2d::str_type::string GetCurrentDirectoryPath()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString* path = [[NSBundle mainBundle] resourcePath];
	path = [path stringByAppendingString:@"/"];
	const char* szPath = [path cStringUsingEncoding:1];
    [pool release];
	return szPath;
}

gs2d::str_type::string AddLastSlash(const gs2d::str_type::string& path)
{
	if (path.empty())
	{
		return GS_L("");
	}
	gs2d::str_type::string r = (path);
	FixSlashes(r);
	const std::size_t lastChar = r.size()-1;

	if (r.at(lastChar) == GS_L('\\'))
	{
		r[lastChar] = GS_L('/');
		return r;
	}
	else if (r.at(lastChar) != GS_L('/'))
	{
		return r + GS_L("/");
	}
	else
	{
		return r;
	}
}

gs2d::str_type::string& FixSlashes(gs2d::str_type::string& path)
{
	return path;
}

gs2d::str_type::string GetModulePath()
{
	return GS_L("");
}

} // namespace
