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

#import "fmod_errors.h"

#import <Foundation/Foundation.h>

#import <AVFoundation/AVAudioSession.h>

#import <UIKit/UIKit.h>

namespace gs2d {

bool FMOD_ERRCHECK_fn(FMOD_RESULT result, const char *file, int line, Platform::FileLogger& logger)
{
	if (result != FMOD_OK)
	{
		gs2d::str_type::stringstream ss;
		ss << file << "(" << line << "): FMOD Error " << result << " - " << FMOD_ErrorString(result);
		logger.Log(ss.str(), Platform::FileLogger::ERROR);
		return true;
	}
	return false;
}

FMOD::System* gSystem = 0;

void Common_Init()
{
    /*
        Optimize audio session for FMOD defaults
    */
    double rate = 24000.0;
    int blockSize = 512;
    long channels = 2;
    BOOL success = false;
    AVAudioSession *session = [AVAudioSession sharedInstance];

    // Make our category 'solo' for the best chance at getting our desired settings
    // Use AVAudioSessionCategoryPlayAndRecord if you need microphone input
    success = [session setCategory:AVAudioSessionCategorySoloAmbient error:nil];
    assert(success);
    
    // Set our preferred rate and activate the session to test it
    success = [session setPreferredSampleRate:rate error:nil];
    assert(success);
    success = [session setActive:TRUE error:nil];
    assert(success);
    
    // Query the actual supported rate and max channels
    rate = [session sampleRate];
    channels = [session respondsToSelector:@selector(maximumOutputNumberOfChannels)] ? [session maximumOutputNumberOfChannels] : 2;
    
    // Deactivate the session so we can change parameters without route changes each time
    success = [session setActive:FALSE error:nil];
    assert(success);
    
    // Set the duration and channels based on known supported values
    success = [session setPreferredIOBufferDuration:blockSize / rate error:nil];
    assert(success);
    if ([session respondsToSelector:@selector(setPreferredOutputNumberOfChannels:error:)])
    {
        success = [session setPreferredOutputNumberOfChannels:channels error:nil];
        assert(success);
    }
    
    /*
        Set up some observers for various notifications
    */
    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
    {
		if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue] == AVAudioSessionInterruptionTypeBegan)
		{
			if (gSystem)
			{
				FMOD_RESULT result = gSystem->mixerSuspend();
				assert(result == FMOD_OK);
			}
		}
		else
		{
			BOOL success = [[AVAudioSession sharedInstance] setActive:TRUE error:nil];
			assert(success);

			if (gSystem)
			{
				FMOD_RESULT result = gSystem->mixerResume();
				assert(result == FMOD_OK);
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
    success = [session setActive:TRUE error:nil];
    assert(success);
}

GS2D_API AudioPtr CreateAudio(boost::any data)
{
	AudioPtr audio = FMAudioContext::Create(data);
	if (audio)
	{
		return audio;
	}
	else
	{
		return AudioPtr();
	}
}

boost::shared_ptr<FMAudioContext> FMAudioContext::Create(boost::any data)
{
	boost::shared_ptr<FMAudioContext> p(new FMAudioContext());
	p->weak_this = p;
	if (p->CreateAudioDevice(data))
	{
		return p;
	}
	else
	{
		return FMAudioContextPtr();
	}
}

FMAudioContext::FMAudioContext() :
	m_globalVolume(1.0f),
	m_logger(Platform::FileLogger::GetLogDirectory() + "FMAudioContext.log.txt")
{
	SetGlobalVolume(1.0f);
}

FMAudioContext::~FMAudioContext()
{
	FMOD_RESULT result;
	result = m_system->close();
	FMOD_ERRCHECK(result, m_logger);
	result = m_system->release();
	FMOD_ERRCHECK(result, m_logger);
}

bool FMAudioContext::CreateAudioDevice(boost::any data)
{
	Common_Init();

	FMOD_RESULT result;
	unsigned int version;

	result = FMOD::System_Create(&m_system);
	if (FMOD_ERRCHECK(result, m_logger))
		return false;

	result = m_system->getVersion(&version);
	if (FMOD_ERRCHECK(result, m_logger))
		return false;

	if (version < FMOD_VERSION)
	{
		m_logger.Log("FMOD lib version doesn't match header version.", Platform::FileLogger::ERROR);
	}

	result = m_system->init(32, FMOD_INIT_NORMAL, 0);
	if (FMOD_ERRCHECK(result, m_logger))
		return false;

	gSystem = m_system;

	m_logger.Log("Audio device initialized", Platform::FileLogger::INFO);
	return true;
}

boost::any FMAudioContext::GetAudioContext()
{
	return m_system;
}

bool FMAudioContext::IsStreamable(const Audio::SAMPLE_TYPE type)
{
	switch (type)
	{
	case Audio::SOUND_EFFECT:
		return false;
	case Audio::MUSIC:
		return true;
	case Audio::SOUNDTRACK:
		return true;
	case Audio::AMBIENT_SFX:
		return true;
	case Audio::UNKNOWN_TYPE:
	default:
		return false;
	}
}

void FMAudioContext::SetGlobalVolume(const float volume)
{
	m_globalVolume = ((volume > 1.0f ? 1.0f : volume) < 0.0f) ? 0.0f : volume;

	FMOD::ChannelGroup* channelGroup = 0;

	FMOD_RESULT result;
	result = m_system->getMasterChannelGroup(&channelGroup);

	if (FMOD_ERRCHECK(result, m_logger))
		return;
	
	if (channelGroup)
	{
		channelGroup->setVolume(m_globalVolume);
	}
}

float FMAudioContext::GetGlobalVolume() const
{
	return m_globalVolume;
}

void FMAudioContext::Update()
{
	if (m_system)
	{
		const FMOD_RESULT result = m_system->update();
		FMOD_ERRCHECK(result, m_logger);
	}
}

} // namespace gs2d
