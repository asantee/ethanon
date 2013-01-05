/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#include "IOSNativeCommandListener.h"

#include "../Platform.h"

#import <AudioToolbox/AudioServices.h>

namespace Platform {

IOSNativeCommmandListener::IOSNativeCommmandListener(gs2d::VideoPtr video) :
	m_video(video)
{
}

void IOSNativeCommmandListener::ParseAndExecuteCommands(const gs2d::str_type::string& commands)
{
	std::vector<gs2d::str_type::string> commandLines = Platform::SplitString(commands, GS_L("\n"));
	for (std::size_t t = 0; t < commandLines.size(); t++)
	{
		ExecuteCommand(commandLines[t]);
	}
}

void IOSNativeCommmandListener::ExecuteCommand(const gs2d::str_type::string &commandLine)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	NSString* line = [NSString stringWithUTF8String:commandLine.c_str()];

	NSArray *words = [line componentsSeparatedByString:@" "];
	NSString* word0 = [words objectAtIndex:0];
	if ([word0 isEqual:@"open_url"])
	{
		NSString* word1 = [words objectAtIndex:1];
		[[UIApplication sharedApplication] openURL:[NSURL URLWithString: word1]];
	}
	else if ([word0 isEqual:@"vibrate"])
	{
		NSString* word1 = [words objectAtIndex:1];
		const double vibrateTime = [word1 doubleValue];
		// if the vibrate time requested is to low, let's not even trigger the
		// event since iOS doesn't allow specific vibration intervals
		if (vibrateTime > 60)
		{
			AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
		}
	}

	[pool release];
}

} // namespace Platform
