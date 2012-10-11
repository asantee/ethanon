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

#import "IOSAudio.h"
#import <SimpleAudioEngine.h>

namespace gs2d {

GS2D_API AudioPtr CreateAudio(boost::any data)
{
	AudioPtr audio = IOSAudioContext::Create(data);
	if (audio)
	{
		return audio;
	}
	else
	{
		return AudioPtr();
	}
}

boost::shared_ptr<IOSAudioContext> IOSAudioContext::Create(boost::any data)
{
	boost::shared_ptr<IOSAudioContext> p(new IOSAudioContext());
	p->weak_this = p;
	if (p->CreateAudioDevice(data))
	{
		return p;
	}
	else
	{
		return IOSAudioContextPtr();
	}
}

IOSAudioContext::IOSAudioContext() :
	m_logger(Platform::FileLogger::GetLogDirectory() + "IOSAudioContext.log.txt")
{
	SetGlobalVolume(1.0f);
}

bool IOSAudioContext::CreateAudioDevice(boost::any data)
{
	m_logger.Log("Audio device initialized", Platform::FileLogger::INFO);
	return true;
}

AudioSamplePtr IOSAudioContext::LoadSampleFromFile(const str_type::string& fileName, const Platform::FileManagerPtr& fileManager, const GS_SAMPLE_TYPE type)
{
	AudioSamplePtr sample = AudioSamplePtr(new IOSAudioSample);
	sample->LoadSampleFromFile(weak_this, fileName, fileManager, type);
	return sample;
}

AudioSamplePtr IOSAudioContext::LoadSampleFromFileInMemory(void *pBuffer, const unsigned int bufferLength, const GS_SAMPLE_TYPE type)
{
	return AudioSamplePtr();
}

boost::any IOSAudioContext::GetAudioContext()
{
	return 0;
}

bool IOSAudioContext::IsStreamable(const GS_SAMPLE_TYPE type)
{
	switch (type)
	{
	case GSST_SOUND_EFFECT:
		return false;
	case GSST_MUSIC:
		return true;
	case GSST_SOUNDTRACK:
		return true;
	case GSST_AMBIENT_SFX:
		return true;
	case GSST_UNKNOWN:
	default:
		return false;
	}
}

void IOSAudioContext::SetGlobalVolume(const float volume)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[[SimpleAudioEngine sharedEngine] setEffectsVolume:volume];
	[[SimpleAudioEngine sharedEngine] setBackgroundMusicVolume:volume];
	[pool release];
}

float IOSAudioContext::GetGlobalVolume() const
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	const float r = [[SimpleAudioEngine sharedEngine] effectsVolume]; 
	[pool release];
	return r;
}

// Audio sample

Platform::FileLogger IOSAudioSample::m_logger(Platform::FileLogger::GetLogDirectory() + "IOSAudioSample.log.txt");
str_type::string IOSAudioSample::m_currentStreamableTrack;

IOSAudioSample::IOSAudioSample() :
	m_volume(1.0f),
	m_speed(1.0f),
	m_loop(false),
	m_pan(0.0f)
{
}

IOSAudioSample::~IOSAudioSample()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString* nsFileName = [NSString stringWithUTF8String:m_fileName.c_str()];
	if (!IOSAudioContext::IsStreamable(m_type))
	{
		[[SimpleAudioEngine sharedEngine] unloadEffect:nsFileName];
	}
	/*else
	{
		[[SimpleAudioEngine sharedEngine] stopBackgroundMusic];
	}*/
	m_logger.Log(m_fileName + " file deleted", Platform::FileLogger::INFO);
	[pool release];
}

bool IOSAudioSample::LoadSampleFromFile(AudioWeakPtr audio, const str_type::string& fileName, const Platform::FileManagerPtr& fileManager, const GS_SAMPLE_TYPE type)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	m_audio = static_cast<IOSAudioContext*>(audio.lock().get());
	m_fileName = fileName;
	m_type = type;

	NSString* nsFileName = [NSString stringWithUTF8String:m_fileName.c_str()];
	if (!IOSAudioContext::IsStreamable(m_type))
	{
		[[SimpleAudioEngine sharedEngine] preloadEffect:nsFileName];
	}
	else
	{
		[[SimpleAudioEngine sharedEngine] preloadBackgroundMusic:nsFileName];
	}
	m_logger.Log(m_fileName + " file loaded", Platform::FileLogger::INFO);
	[pool release];
	return true;
}

bool IOSAudioSample::LoadSampleFromFileInMemory(AudioWeakPtr audio, void *pBuffer, const unsigned int bufferLength, const GS_SAMPLE_TYPE type)
{
	// TODO
	return false;
}

bool IOSAudioSample::SetLoop(const bool enable)
{
	m_loop = enable;
	return true;
}

bool IOSAudioSample::GetLoop() const
{
	return m_loop;
}

bool IOSAudioSample::Play()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString* fileName = [NSString stringWithUTF8String:m_fileName.c_str()];
	if (!IOSAudioContext::IsStreamable(m_type))
	{
		[[SimpleAudioEngine sharedEngine] playEffect:fileName pitch:m_speed pan:m_pan gain:m_volume];
	}
	else
	{
		if (!IsPlaying() || m_currentStreamableTrack != m_fileName)
			[[SimpleAudioEngine sharedEngine] playBackgroundMusic:fileName loop:m_loop];
		m_currentStreamableTrack = m_fileName;
	}
	[pool release];
	return true;
}

GS_SAMPLE_STATUS IOSAudioSample::GetStatus()
{
	// TODO
	return GSSS_UNKNOWN;
}

bool IOSAudioSample::IsPlaying()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	bool r;
	if (IOSAudioContext::IsStreamable(m_type))
	{
		r = [[SimpleAudioEngine sharedEngine] isBackgroundMusicPlaying];
	}
	else
	{
		r = false; // hard to track... we don't need it for now
	}
	[pool release];
	return r;
}

bool IOSAudioSample::Pause()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	if (IOSAudioContext::IsStreamable(m_type))
	{
		[[SimpleAudioEngine sharedEngine] pauseBackgroundMusic];
	}
	else
	{
		// can't pause sfx's
		return false;
	}
	[pool release];
	return true;
}

bool IOSAudioSample::Stop()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	if (IOSAudioContext::IsStreamable(m_type))
	{
		[[SimpleAudioEngine sharedEngine] stopBackgroundMusic];
	}
	else
	{
		// can't stop sfx's
		return false;
	}
	[pool release];
	return true;
}

GS_SAMPLE_TYPE IOSAudioSample::GetType() const
{
	return m_type;
}

bool IOSAudioSample::SetSpeed(const float speed)
{
	m_speed = speed;
	return true;
}

float IOSAudioSample::GetSpeed() const
{
	return m_speed;
}

bool IOSAudioSample::SetVolume(const float volume)
{
	m_volume = volume;
	return true;
}

float IOSAudioSample::GetVolume() const
{
	return m_volume;
}

bool IOSAudioSample::SetPan(const float pan)
{
	m_pan = pan;
	return true;
}

float IOSAudioSample::GetPan() const
{
	return m_pan;
}

} // namespace gs2d
