#import "FMAudioContext.h"

#import "fmod_errors.h"

#import <Foundation/Foundation.h>

#import <AVFoundation/AVAudioSession.h>

#import <UIKit/UIKit.h>

namespace gs2d {

static bool g_suspended = false;

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
    /*success = */[session setCategory:AVAudioSessionCategoryAmbient withOptions:AVAudioSessionCategoryOptionMixWithOthers error:nil];
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
		const bool began = [[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue] == AVAudioSessionInterruptionTypeBegan;

		if (began == g_suspended)
		{
			return;
		}
		
		if (began)
		{
			NSLog(@"Interruption started");
			if (m_system)
			{
				FMOD_RESULT result = m_system->mixerSuspend();
				FMOD_ERRCHECK_fn(result, __FILE__, __LINE__, logger);
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
			logger.Log("AVAudioSession setActive failed!", Platform::Logger::LT_ERROR);

			if (m_system)
			{
				FMOD_RESULT result = m_system->mixerResume();
				FMOD_ERRCHECK_fn(result, __FILE__, __LINE__, logger);
			}
		}

		if (began && [[notification.userInfo valueForKey:AVAudioSessionInterruptionWasSuspendedKey] boolValue])
		{
			return;
		}

		g_suspended = began;
		if (!began)
		{
			[[AVAudioSession sharedInstance] setActive:TRUE error:nil];
		}

		/*if (gSuspendCallback)
		{
			gSuspendCallback(began);
		}*/
	}];

	[[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidBecomeActiveNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
	{
		#ifndef TARGET_OS_TV
			if (!g_suspended)
			{
				return;
			}
		#else
			/*if (gSuspendCallback)
			{
				gSuspendCallback(true);
			}*/
		#endif

		[[AVAudioSession sharedInstance] setActive:TRUE error:nil];
		/*if (gSuspendCallback)
		{
			gSuspendCallback(false);
		}*/
		g_suspended = false;
	}];

	
	/* [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
    {
		NSLog(@"Start Interruption");
		if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue] == AVAudioSessionInterruptionTypeBegan)
		{
			NSLog(@"Interruption started");
			if (m_system)
			{
				FMOD_RESULT result = m_system->mixerSuspend();
				FMOD_ERRCHECK_fn(result, __FILE__, __LINE__, logger);
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
			logger.Log("AVAudioSession setActive failed!", Platform::Logger::LT_ERROR);

			if (m_system)
			{
				FMOD_RESULT result = m_system->mixerResume();
				FMOD_ERRCHECK_fn(result, __FILE__, __LINE__, logger);
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
    }*/

    /*
        Activate the audio session
    */
    /*success = */[session setActive:TRUE error:nil];
    //assert(success);
}

} // namespace gs2d
