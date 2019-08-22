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
			[[UIApplication sharedApplication] openURL:[NSURL URLWithString: word1]];
			
			return true;
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
			return true;
		}
	}
	return false;
}

} // namespace Platform
