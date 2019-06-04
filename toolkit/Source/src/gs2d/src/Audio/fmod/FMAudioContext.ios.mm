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

#import "FMAudioContext.h"

#import <fmod_errors.h>

#import <Foundation/Foundation.h>

#import <AVFoundation/AVAudioSession.h>

#import <UIKit/UIKit.h>

namespace gs2d {

void FMAudioContext::CommonInit(Platform::FileLogger &logger)
{
    /*
        Optimize audio session for FMOD defaults
    */
    double rate = 24000.0;
    int blockSize = 512;
    long channels = 2;
    //BOOL success = false;
    AVAudioSession *session = [AVAudioSession sharedInstance];

    // Make our category 'solo' for the best chance at getting our desired settings
    // Use AVAudioSessionCategoryPlayAndRecord if you need microphone input
    /*success = */[session setCategory:AVAudioSessionCategorySoloAmbient error:nil];
    //assert(success);

    // Set our preferred rate and activate the session to test it
    /*success = */[session setPreferredSampleRate:rate error:nil];
    //assert(success);
    /*success = */[session setActive:TRUE error:nil];
    //assert(success);
	
    // Query the actual supported rate and max channels
    rate = [session sampleRate];
    channels = [session respondsToSelector:@selector(maximumOutputNumberOfChannels)] ? [session maximumOutputNumberOfChannels] : 2;
	
    // Deactivate the session so we can change parameters without route changes each time
    /*success = */[session setActive:FALSE error:nil];
    //assert(success);
	
    // Set the duration and channels based on known supported values
    /*success = */[session setPreferredIOBufferDuration:blockSize / rate error:nil];
    //assert(success);
    if ([session respondsToSelector:@selector(setPreferredOutputNumberOfChannels:error:)])
    {
        /*success = */[session setPreferredOutputNumberOfChannels:channels error:nil];
        //assert(success);
    }
	
    /*
        Set up some observers for various notifications
    */
    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
    {
		NSLog(@"Start Interruption");
		if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue] == AVAudioSessionInterruptionTypeBegan)
		{
			NSLog(@"Interruption started");
			if (m_system)
			{
				FMOD_RESULT result = m_system->mixerSuspend();
				FMOD_ERRCHECK(result, logger);
			}
		}
		else
		{
			NSLog(@"Interruption ended");
			NSError* error;
			BOOL success;
			const unsigned int maxTries = 40;
			unsigned int tryCount = 0;
			while (!(success = [[AVAudioSession sharedInstance] setActive:TRUE error:&error]) && tryCount < maxTries)
			{
				++tryCount;
				if (error != nil)
				{
					NSLog(@"%@", [error description]);
				}
				[NSThread sleepForTimeInterval:0.1f];
			}
			logger.Log("AVAudioSession setActive failed!", Platform::Logger::ERROR);

			if (m_system)
			{
				FMOD_RESULT result = m_system->mixerResume();
				FMOD_ERRCHECK(result, logger);
			}
		}
    }];

    if (&AVAudioSessionSilenceSecondaryAudioHintNotification)
    {
        [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionSilenceSecondaryAudioHintNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
        {
            bool began = [[notification.userInfo valueForKey:AVAudioSessionSilenceSecondaryAudioHintTypeKey] intValue] == AVAudioSessionSilenceSecondaryAudioHintTypeBegin;
            NSLog(@"Silence secondary audio %@", began ? @"Began" : @"Ended");
        }];
    }

    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionRouteChangeNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
    {
        NSNumber *reason = [[notification userInfo] valueForKey:AVAudioSessionRouteChangeReasonKey];
        AVAudioSessionPortDescription *oldOutput = [[[notification userInfo] valueForKey:AVAudioSessionRouteChangePreviousRouteKey] outputs][0];
        AVAudioSessionPortDescription *newOutput = [[[AVAudioSession sharedInstance] currentRoute] outputs][0];

        const char *reasonString = NULL;
        switch ([reason intValue])
        {
            case AVAudioSessionRouteChangeReasonNewDeviceAvailable:         reasonString = "New Device Available";              break;
            case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:       reasonString = "Old Device Unavailable";            break;
            case AVAudioSessionRouteChangeReasonCategoryChange:             reasonString = "Category Change";                   break;
            case AVAudioSessionRouteChangeReasonOverride:                   reasonString = "Override";                          break;
            case AVAudioSessionRouteChangeReasonWakeFromSleep:              reasonString = "Wake From Sleep";                   break;
            case AVAudioSessionRouteChangeReasonNoSuitableRouteForCategory: reasonString = "No Suitable Route For Category";    break;
            case AVAudioSessionRouteChangeReasonRouteConfigurationChange:   reasonString = "Configuration Change";              break;
            default:                                                        reasonString = "Unknown";
        }

        NSLog(@"Output route has changed from %dch %@ to %dch %@ due to '%s'", (int)[[oldOutput channels] count], [oldOutput portName], (int)[[newOutput channels] count], [newOutput portName], reasonString);
    }];

    if (&AVAudioSessionMediaServicesWereLostNotification)
    {
        [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionMediaServicesWereLostNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
        {
            NSLog(@"Media services were lost");
        }];
    }
    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionMediaServicesWereResetNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
    {
        NSLog(@"Media services were reset");
    }];

    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidBecomeActiveNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
    {
        NSLog(@"Application did become active");
    }];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillResignActiveNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
    {
        NSLog(@"Application will resign active");
    }];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidEnterBackgroundNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
    {
        NSLog(@"Application did enter background");
    }];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillEnterForegroundNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
    {
        NSLog(@"Application will enter foreground");
    }];

    /*
        Activate the audio session
    */
    /*success = */[session setActive:TRUE error:nil];
    //assert(success);
}

} // namespace gs2d
