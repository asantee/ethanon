#include "IOSNativeCommandListener.h"

#import <AudioToolbox/AudioServices.h>

#import <UIKit/UIKit.h>

namespace Platform {

bool IOSNativeCommmandListener::ExecuteCommand(const std::string& commandLine)
{
	@autoreleasepool {
		NSString* line = [NSString stringWithUTF8String:commandLine.c_str()];

		NSArray *words = [line componentsSeparatedByString:@" "];
		NSString* word0 = [words objectAtIndex:0];
		if ([word0 isEqual:@"open_url"])
		{
			NSString* word1 = [words objectAtIndex:1];
			UIApplication *application = [UIApplication sharedApplication];
			NSURL *URL = [NSURL URLWithString:word1];
			[application openURL:URL options:@{} completionHandler:nil];
			return true;
		}
		else if ([word0 isEqual:@"clipboard"])
		{
			if ([words count] >= 3)
			{
				UIPasteboard *pasteboard = [UIPasteboard generalPasteboard];
				pasteboard.string = [words objectAtIndex:2];
			}
		}
	}
	return false;
}

} // namespace Platform
