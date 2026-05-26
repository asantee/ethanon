#import "FMAudioContext.h"

#import <fmod_errors.h>

#import <Foundation/Foundation.h>

#import <AVFoundation/AVAudioSession.h>

#import <UIKit/UIKit.h>

namespace gs2d {

static bool g_suspended = false;
static bool g_needsReset = false;

void CommonDefaultSuspendCallback(bool suspend, Platform::FileLogger &logger, FMOD::System* system)
{
	if (!system)
		return;

	if (suspend)
	{
		FMOD_RESULT result = system->mixerSuspend();
		FMOD_ERRCHECK_fn(result, __FILE__, __LINE__, logger);
	}
	else
	{
		FMOD_RESULT result = system->mixerResume();
		FMOD_ERRCHECK_fn(result, __FILE__, __LINE__, logger);
	}
}

void FMAudioContext::CommonInit(Platform::FileLogger &logger)
{
	AVAudioSession *session = [AVAudioSession sharedInstance];
	double rate = 24000.0; // This should match System::setSoftwareFormat 'samplerate' which defaults to 24000
	int blockSize = 512; // This should match System::setDSPBufferSize 'bufferlength' which defaults to 512

	/*BOOL success = */[session setPreferredSampleRate:rate error:nil];
	//assert(success);

	/*success = */[session setPreferredIOBufferDuration:blockSize / rate error:nil];
	//assert(success);

	//long maxChannels = [session maximumOutputNumberOfChannels];
	///*BOOL success = */[session setPreferredOutputNumberOfChannels:maxChannels error:nil];
	//assert(success);
	
	/*success = */[session setActive:TRUE error:nil];
	//assert(success);

	//
	//
	// add observers
	//
	//
	[[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
	{
		AVAudioSessionInterruptionType type = (AVAudioSessionInterruptionType)[[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] unsignedIntegerValue];
		if (type == AVAudioSessionInterruptionTypeBegan)
		{
			logger.Log("Interruption Began", Platform::Logger::LT_INFO);
			// Ignore deprecated warnings regarding AVAudioSessionInterruptionReasonAppWasSuspended and
			// AVAudioSessionInterruptionWasSuspendedKey, we protect usage for the versions where they are available
			#pragma clang diagnostic push
			#pragma clang diagnostic ignored "-Wdeprecated-declarations"

			// If the audio session was deactivated while the app was in the background, the app receives the
			// notification when relaunched. Identify this reason for interruption and ignore it.
			if (@available(iOS 16.0, tvOS 14.5, *))
			{
				// Delayed suspend-in-background notifications no longer exist, this must be a real interruption
			}
			#if !TARGET_OS_TV // tvOS never supported "AVAudioSessionInterruptionReasonAppWasSuspended"
			else if (@available(iOS 14.5, *))
			{
				if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionReasonKey] intValue] == AVAudioSessionInterruptionReasonAppWasSuspended)
				{
					logger.Log("Ignoring delayed AVAudioSessionInterruptionNotification", Platform::Logger::LT_INFO);
					return; // Ignore delayed suspend-in-background notification
				}
			}
			#endif // !TARGET_OS_TV
			else
			{
				if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionWasSuspendedKey] boolValue])
				{
					logger.Log("Ignoring delayed AVAudioSessionInterruptionNotification", Platform::Logger::LT_INFO);
					return; // Ignore delayed suspend-in-background notification
				}
			}

			CommonDefaultSuspendCallback(true, logger, m_system);
			g_suspended = true;

			#pragma clang diagnostic pop
		}
		else if (type == AVAudioSessionInterruptionTypeEnded)
		{
			logger.Log("Interruption Ended", Platform::Logger::LT_INFO);
			NSError *errorMessage = nullptr;
			if (![[AVAudioSession sharedInstance] setActive:TRUE error:&errorMessage])
			{
				// Interruption like Siri can prevent session activation, wait for did-become-active notification
				NSString *message = [NSString stringWithFormat:@"AVAudioSessionInterruptionNotification: AVAudioSession.setActive() failed: %@", errorMessage];
				logger.Log([message cStringUsingEncoding:1], Platform::Logger::LT_WARNING);
				return;
			}

			CommonDefaultSuspendCallback(false, logger, m_system);
			g_suspended = false;
		}
	}];

	[[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidBecomeActiveNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
	{
		logger.Log("Application did become active", Platform::Logger::LT_INFO);

		if (g_needsReset)
		{
			CommonDefaultSuspendCallback(true, logger, m_system);
			g_suspended = true;
		}

		NSError *errorMessage = nullptr;
		if (![[AVAudioSession sharedInstance] setActive:TRUE error:&errorMessage])
		{
			if ([errorMessage code] == AVAudioSessionErrorCodeCannotStartPlaying)
			{
				// Interruption like Screen Time can prevent session activation, but will not trigger an interruption-ended notification.
				// There is no other callback or trigger to hook into after this point, we are not in the background and there is no other audio playing.
				// Our only option is to have a sleep loop until the Audio Session can be activated again.
				while (![[AVAudioSession sharedInstance] setActive:TRUE error:nil])
				{
					usleep(20000);
				}
			}
			else
			{
				// Interruption like Siri can prevent session activation, wait for interruption-ended notification.
				NSString *message = [NSString stringWithFormat:@"UIApplicationDidBecomeActiveNotification: AVAudioSession.setActive() failed: %@", errorMessage];
				logger.Log([message cStringUsingEncoding:1], Platform::Logger::LT_WARNING);
				return;
			}
		}

		// It's possible the system missed sending us an interruption end, so recover here
		if (g_suspended)
		{
			CommonDefaultSuspendCallback(false, logger, m_system);
			g_needsReset = false;
			g_suspended = false;
		}
	}];

	[[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionMediaServicesWereResetNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
	{
		logger.Log("Media services were reset", Platform::Logger::LT_INFO);
		if ([UIApplication sharedApplication].applicationState == UIApplicationStateBackground || g_suspended)
		{
			// Received the reset notification while in the background, need to reset the AudioUnit when we come back to foreground.
			g_needsReset = true;
		}
		else
		{
			// In the foregound but something chopped the media services, need to do a reset.
			CommonDefaultSuspendCallback(true, logger, m_system);
			CommonDefaultSuspendCallback(false, logger, m_system);
		}
	}];
}

} // namespace gs2d

