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

#include "AndroidAudio.h"
#include <exception>
#include "../../Platform/android/Platform.android.h"

namespace gs2d {

GS2D_API AudioPtr CreateAudio(boost::any data)
{
	AudioPtr audio = AndroidAudioContext::Create(data);
	if (audio)
	{
		return audio;
	}
	else
	{
		return AudioPtr();
	}
}

boost::shared_ptr<AndroidAudioContext> AndroidAudioContext::Create(boost::any data)
{
	boost::shared_ptr<AndroidAudioContext> p(new AndroidAudioContext());
	p->weak_this = p;
	if (p->CreateAudioDevice(data))
	{
		return p;
	}
	else
	{
		return AndroidAudioContextPtr();
	}
}

AndroidAudioContext::AndroidAudioContext() :
	m_logger(Platform::FileLogger::GetLogDirectory() + "AndroidAudioContext.log.txt"),
	m_globalVolume(1.0f)
{
}

bool AndroidAudioContext::CreateAudioDevice(boost::any data)
{
	m_logger.Log("Audio device initialized", Platform::FileLogger::INFO);
	return true;
}

AudioSamplePtr AndroidAudioContext::LoadSampleFromFile(
	const str_type::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const Audio::SAMPLE_TYPE type)
{
	AudioSamplePtr sample = AudioSamplePtr(new AndroidAudioSample);
	sample->LoadSampleFromFile(weak_this, fileName, fileManager, type);
	return sample;
}

AudioSamplePtr AndroidAudioContext::LoadSampleFromFileInMemory(
	void *pBuffer,
	const unsigned int bufferLength,
	const Audio::SAMPLE_TYPE type)
{
	return AudioSamplePtr();
}

boost::any AndroidAudioContext::GetAudioContext()
{
	str_type::string out;
	ForwardCommands(out);
	return out;
}

bool AndroidAudioContext::IsStreamable(const Audio::SAMPLE_TYPE type)
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

void AndroidAudioContext::SetGlobalVolume(const float volume)
{
	m_globalVolume = volume;
	Command(Platform::NativeCommandAssembler::SetGlobalVolume(m_globalVolume));
}

float AndroidAudioContext::GetGlobalVolume() const
{
	return m_globalVolume;
}

// Audio sample

Platform::FileLogger AndroidAudioSample::m_logger(Platform::FileLogger::GetLogDirectory() + "AndroidAudioSample.log.txt");

AndroidAudioSample::AndroidAudioSample() :
	m_volume(1.0f),
	m_speed(1.0f),
	m_loop(false)
{
}

AndroidAudioSample::~AndroidAudioSample()
{
	if (!AndroidAudioContext::IsStreamable(m_type))
	{
		const str_type::string cmd = Platform::NativeCommandAssembler::DeleteSound(m_fileName);
		m_audio->Command(cmd, true);
		m_logger.Log(cmd, Platform::FileLogger::INFO);
	}
}

bool AndroidAudioSample::LoadSampleFromFile(
	AudioWeakPtr audio,
	const str_type::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const Audio::SAMPLE_TYPE type)
{
	m_audio = static_cast<AndroidAudioContext*>(audio.lock().get());
	m_fileName = fileName;
	m_type = type;

	str_type::string cmd;
	if (!AndroidAudioContext::IsStreamable(m_type))
	{
		cmd = Platform::NativeCommandAssembler::LoadSound(m_fileName);
	}
	else
	{
		cmd = Platform::NativeCommandAssembler::LoadMusic(m_fileName);
	}
	m_audio->Command(cmd);
	m_logger.Log(cmd, Platform::FileLogger::INFO);
	return true;
}

bool AndroidAudioSample::LoadSampleFromFileInMemory(
	AudioWeakPtr audio,
	void *pBuffer,
	const unsigned int bufferLength,
	const Audio::SAMPLE_TYPE type)
{
	// TODO
	return false;
}

bool AndroidAudioSample::SetLoop(const bool enable)
{
	m_loop = enable;
	return true;
}

bool AndroidAudioSample::GetLoop() const
{
	return m_loop;
}

bool AndroidAudioSample::Play()
{
	if (!AndroidAudioContext::IsStreamable(m_type))
	{
		m_audio->Command(Platform::NativeCommandAssembler::PlaySound(m_fileName, m_volume, m_loop, m_speed));
	}
	else
	{
		m_audio->Command(Platform::NativeCommandAssembler::PlayMusic(m_fileName, m_volume, m_loop, m_speed));
	}
	return true;
}

Audio::SAMPLE_STATUS AndroidAudioSample::GetStatus()
{
	return Audio::UNKNOWN_STATUS;
}

bool AndroidAudioSample::IsPlaying()
{
	// TODO
	return false;
}

bool AndroidAudioSample::Pause()
{
	// TODO
	return false;
}

bool AndroidAudioSample::Stop()
{
	if (!AndroidAudioContext::IsStreamable(m_type))
	{
		// can't stop pool samples on android...
		return false;
	}
	else
	{
		m_audio->Command(Platform::NativeCommandAssembler::StopMusic(m_fileName));
		return true;
	}
}

Audio::SAMPLE_TYPE AndroidAudioSample::GetType() const
{
	return m_type;
}

bool AndroidAudioSample::SetSpeed(const float speed)
{
	m_speed = speed;
	return true;
}

float AndroidAudioSample::GetSpeed() const
{
	return m_speed;
}

bool AndroidAudioSample::SetVolume(const float volume)
{
	m_volume = volume;
	return true;
}

float AndroidAudioSample::GetVolume() const
{
	return m_volume;
}

bool AndroidAudioSample::SetPan(const float pan)
{
	// TODO
	return false;
}

float AndroidAudioSample::GetPan() const
{
	// TODO
	return 0.0f;
}

} // namespace gs2d
