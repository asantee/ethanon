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

#import "CDAudioSample.h"
#import <SimpleAudioEngine.h>

namespace gs2d {

AudioSamplePtr CDAudioContext::LoadSampleFromFile(
	const str_type::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const Audio::SAMPLE_TYPE type)
{
	AudioSamplePtr sample = AudioSamplePtr(new CDAudioSample);
	sample->LoadSampleFromFile(weak_this, fileName, fileManager, type);
	return sample;
}

AudioSamplePtr CDAudioContext::LoadSampleFromFileInMemory(
	void *pBuffer,
	const unsigned int bufferLength,
	const Audio::SAMPLE_TYPE type)
{
	return AudioSamplePtr();
}

Platform::FileLogger CDAudioSample::m_logger(Platform::FileLogger::GetLogDirectory() + "CDAudioSample.log.txt");
str_type::string CDAudioSample::m_currentStreamableTrack;

CDAudioSample::CDAudioSample() :
	m_volume(1.0f),
	m_speed(1.0f),
	m_loop(false),
	m_pan(0.0f)
{
}

CDAudioSample::~CDAudioSample()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString* nsFileName = [NSString stringWithUTF8String:m_fileName.c_str()];
	if (!CDAudioContext::IsStreamable(m_type))
	{
		[[SimpleAudioEngine sharedEngine] unloadEffect:nsFileName];
	}
	m_logger.Log(m_fileName + " file deleted", Platform::FileLogger::INFO);
	[pool release];
}

bool CDAudioSample::LoadSampleFromFile(
	AudioWeakPtr audio,
	const str_type::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const Audio::SAMPLE_TYPE type)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	m_audio = static_cast<CDAudioContext*>(audio.lock().get());
	m_fileName = fileName;
	m_type = type;

	NSString* nsFileName = [NSString stringWithUTF8String:m_fileName.c_str()];
	if (!CDAudioContext::IsStreamable(m_type))
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

bool CDAudioSample::LoadSampleFromFileInMemory(
	AudioWeakPtr audio,
	void *pBuffer,
	const unsigned int bufferLength,
	const Audio::SAMPLE_TYPE type)
{
	// TODO
	return false;
}

bool CDAudioSample::SetLoop(const bool enable)
{
	m_loop = enable;
	return true;
}

bool CDAudioSample::GetLoop() const
{
	return m_loop;
}

bool CDAudioSample::Play()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString* fileName = [NSString stringWithUTF8String:m_fileName.c_str()];
	if (!CDAudioContext::IsStreamable(m_type))
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

Audio::SAMPLE_STATUS CDAudioSample::GetStatus()
{
	// TODO
	return Audio::UNKNOWN_STATUS;
}

bool CDAudioSample::IsPlaying()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	bool r;
	if (CDAudioContext::IsStreamable(m_type))
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

bool CDAudioSample::Pause()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	if (CDAudioContext::IsStreamable(m_type))
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

bool CDAudioSample::Stop()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	if (CDAudioContext::IsStreamable(m_type))
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

Audio::SAMPLE_TYPE CDAudioSample::GetType() const
{
	return m_type;
}

bool CDAudioSample::SetSpeed(const float speed)
{
	m_speed = speed;
	return true;
}

float CDAudioSample::GetSpeed() const
{
	return m_speed;
}

bool CDAudioSample::SetVolume(const float volume)
{
	m_volume = volume;
	return true;
}

float CDAudioSample::GetVolume() const
{
	return m_volume;
}

bool CDAudioSample::SetPan(const float pan)
{
	m_pan = pan;
	return true;
}

float CDAudioSample::GetPan() const
{
	return m_pan;
}

} // namespace gs2d
