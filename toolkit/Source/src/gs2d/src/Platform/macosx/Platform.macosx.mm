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

#import "Platform.macosx.h"
#import "../Platform.h"
#import "../../Video/GLES2/GLES2Video.h"

void gs2d::ShowMessage(str_type::stringstream& stream, const GS_MESSAGE_TYPE type)
{
	if (type == GSMT_ERROR)
	{
		std::cerr << "Error: " << stream.str() << std::endl;
	}
	else
	{
		std::cout << ((type == GSMT_WARNING) ? "Warning: " : "") << stream.str() << std::endl;
	}
}

/*float gs2d::GLVideo::GetElapsedTimeF(const TIME_UNITY unity) const
{
	return static_cast<float>(GetElapsedTime(unity));
}

unsigned long gs2d::GLVideo::GetElapsedTime(const TIME_UNITY unity) const
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	const double time = [[NSDate date] timeIntervalSince1970] - Platform::macosx::StartTime::m_startTime;

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
}*/

namespace Platform {

gs2d::str_type::string Platform::FileLogger::GetLogDirectory()
{
	/*NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString* dir = NSHomeDirectory();
	const char* szDir = [dir cStringUsingEncoding:1];
	std::string logPath(szDir);
	logPath += "/log/";*/

	//[pool release];
	#warning TODO
	return GS_L("");
}

gs2d::str_type::string GetModuleDirectory()
{
	#warning TODO
	return GS_L("");
}

char GetDirectorySlashA()
{
	return '/';
}

} // namespace
