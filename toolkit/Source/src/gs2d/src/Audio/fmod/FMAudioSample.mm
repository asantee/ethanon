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

#import "FMAudioSample.h"

namespace gs2d {

AudioSamplePtr FMAudioContext::LoadSampleFromFile(
	const str_type::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const Audio::SAMPLE_TYPE type)
{
	AudioSamplePtr sample = AudioSamplePtr(new FMAudioSample);
	sample->LoadSampleFromFile(weak_this, fileName, fileManager, type);
	return sample;
}

AudioSamplePtr FMAudioContext::LoadSampleFromFileInMemory(
	void *pBuffer,
	const unsigned int bufferLength,
	const Audio::SAMPLE_TYPE type)
{
	return AudioSamplePtr();
}

Platform::FileLogger FMAudioSample::m_logger(Platform::FileLogger::GetLogDirectory() + "FMAudioSample.log.txt");
str_type::string FMAudioSample::m_currentStreamableTrack;

FMAudioSample::FMAudioSample() :
	m_sound(0),
	m_channel(0),
	m_volume(1.0f),
	m_speed(1.0f),
	m_loop(false),
	m_pan(0.0f),
	m_type(Audio::UNKNOWN_TYPE)
{
}

FMAudioSample::~FMAudioSample()
{
	if (m_sound)
	{
		const FMOD_RESULT result = m_sound->release();
		FMOD_ERRCHECK(result, m_logger);
	}
	m_logger.Log(m_fileName + " file deleted", Platform::FileLogger::INFO);
}

bool FMAudioSample::LoadSampleFromFile(
	AudioWeakPtr audio,
	const str_type::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const Audio::SAMPLE_TYPE type)
{
	m_fileName = fileName;
	try
	{
		Audio *pAudio = audio.lock().get();
		m_system = boost::any_cast<FMOD::System*>(pAudio->GetAudioContext());
	}
	catch (const boost::bad_any_cast &)
	{
		str_type::stringstream ss;
		ss << GS_L("FMAudioSample::LoadSampleFromFile: Invalid fmod system");
		m_logger.Log(ss.str(), Platform::FileLogger::ERROR);
		return false;
	}

	if (FMAudioContext::IsStreamable(type))
	{
	    const FMOD_RESULT result = m_system->createStream(fileName.c_str(), FMOD_DEFAULT, 0, &m_sound);
		if (FMOD_ERRCHECK(result, m_logger))
			return false;
	}
	else
	{
		const FMOD_RESULT result = m_system->createSound(fileName.c_str(), FMOD_DEFAULT, 0, &m_sound);
		if (FMOD_ERRCHECK(result, m_logger))
			return false;
	}

	m_logger.Log(m_fileName + " file loaded", Platform::FileLogger::INFO);
	return true;
}

bool FMAudioSample::LoadSampleFromFileInMemory(
	AudioWeakPtr audio,
	void *pBuffer,
	const unsigned int bufferLength,
	const Audio::SAMPLE_TYPE type)
{
	// TODO
	return false;
}

bool FMAudioSample::Play()
{
	m_channel = 0;

	FMOD_RESULT result;
	result = m_system->playSound(m_sound, 0, false, &m_channel);
	if (FMOD_ERRCHECK(result, m_logger))
	{
		m_channel = 0;
		return false;
	}
	
	if (m_channel)
	{
		SetLoop(m_loop);
		SetSpeed(m_speed);
		SetVolume(m_volume);
		SetPan(m_pan);
	}
	return true;
}

bool FMAudioSample::SetLoop(const bool enable)
{
	m_loop = enable;
	if (m_channel)
	{
		const FMOD_RESULT result = m_channel->setMode(m_loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		
		if ((result == FMOD_ERR_INVALID_HANDLE) || (result == FMOD_ERR_CHANNEL_STOLEN))
		{
			m_channel = 0;
			return true;
		}
		
		if (FMOD_ERRCHECK(result, m_logger))
			return false;
	}
	return true;
}

bool FMAudioSample::GetLoop() const
{
	return m_loop;
}

bool FMAudioSample::Stop()
{
	if (m_channel)
	{
		const FMOD_RESULT result = m_channel->stop();

		if ((result == FMOD_ERR_INVALID_HANDLE) || (result == FMOD_ERR_CHANNEL_STOLEN))
		{
			m_channel = 0;
			return true;
		}

		if (FMOD_ERRCHECK(result, m_logger))
			return false;
	}
	return false;
}

bool FMAudioSample::IsPlaying()
{
	bool playing = false;
	if (m_channel)
	{
		const FMOD_RESULT result = m_channel->isPlaying(&playing);

		if ((result == FMOD_ERR_INVALID_HANDLE) || (result == FMOD_ERR_CHANNEL_STOLEN))
		{
			m_channel = 0;
			return false;
		}

		if (FMOD_ERRCHECK(result, m_logger))
			return false;
	}
	return playing;
}

Audio::SAMPLE_STATUS FMAudioSample::GetStatus()
{
	if (m_channel)
	{
		FMOD_RESULT result;
		bool paused = false;
		result = m_channel->getPaused(&paused);

		if ((result == FMOD_ERR_INVALID_HANDLE) || (result == FMOD_ERR_CHANNEL_STOLEN))
		{
			m_channel = 0;
			return Audio::STOPPED;
		}

		if (!FMOD_ERRCHECK(result, m_logger))
		{
			if (paused)
				return Audio::PAUSED;
		}
		
		if (IsPlaying())
		{
			return Audio::PLAYING;
		}
	}
	return Audio::STOPPED;
}

bool FMAudioSample::Pause()
{
	if (m_channel)
	{
		const FMOD_RESULT result = m_channel->setPaused(true);

		if ((result == FMOD_ERR_INVALID_HANDLE) || (result == FMOD_ERR_CHANNEL_STOLEN))
		{
			m_channel = 0;
			return false;
		}

		if (FMOD_ERRCHECK(result, m_logger))
			return false;
	}
	return true;
}

Audio::SAMPLE_TYPE FMAudioSample::GetType() const
{
	return m_type;
}

bool FMAudioSample::SetSpeed(const float speed)
{
	m_speed = speed;
	if (m_channel)
	{
		FMOD_RESULT result;
		
		float frequency;
		result = m_channel->getFrequency(&frequency);
		
		if ((result == FMOD_ERR_INVALID_HANDLE) || (result == FMOD_ERR_CHANNEL_STOLEN))
		{
			m_channel = 0;
			return true;
		}

		result = m_channel->setFrequency(speed * frequency);

		if (FMOD_ERRCHECK(result, m_logger))
			return false;
	}
	return true;
}

float FMAudioSample::GetSpeed() const
{
	return m_speed;
}

bool FMAudioSample::SetVolume(const float volume)
{
	m_volume = volume;
	if (m_channel)
	{
		const FMOD_RESULT result = m_channel->setVolume(m_volume);

		if ((result == FMOD_ERR_INVALID_HANDLE) || (result == FMOD_ERR_CHANNEL_STOLEN))
		{
			m_channel = 0;
			return true;
		}

		if (FMOD_ERRCHECK(result, m_logger))
			return false;
	}
	return true;
}

float FMAudioSample::GetVolume() const
{
	return m_volume;
}

bool FMAudioSample::SetPan(const float pan)
{
	m_pan = pan;
	if (m_channel)
	{
		const FMOD_RESULT result = m_channel->setPan(m_pan);

		if ((result == FMOD_ERR_INVALID_HANDLE) || (result == FMOD_ERR_CHANNEL_STOLEN))
		{
			m_channel = 0;
			return true;
		}

		if (FMOD_ERRCHECK(result, m_logger))
			return false;
	}
	return true;
}

float FMAudioSample::GetPan() const
{
	return m_pan;
}

} // namespace gs2d
