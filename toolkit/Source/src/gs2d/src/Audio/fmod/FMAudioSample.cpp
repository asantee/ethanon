#include "FMAudioSample.h"

#include <sstream>

namespace gs2d {

AudioSamplePtr FMAudioContext::LoadSampleFromFile(
	const std::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const Audio::SAMPLE_TYPE type)
{
	AudioSamplePtr sample = AudioSamplePtr(new FMAudioSample);
	if (!sample->LoadSampleFromFile(weak_this, fileName, fileManager, type))
	{
		sample = nullptr;
	}
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
std::string FMAudioSample::m_currentStreamableTrack;

FMAudioSample::FMAudioSample() :
	m_sound(0),
	m_channel(0),
	m_volume(1.0f),
	m_speed(1.0f),
	m_loop(false),
	m_pan(0.0f),
	m_type(Audio::UNKNOWN_TYPE),
	m_fileName("")
{
}

FMAudioSample::~FMAudioSample()
{
	if (m_sound)
	{
		const FMOD_RESULT result = m_sound->release();
		FMOD_ERRCHECK(result, m_logger);
	}
	m_logger.Log(m_fileName + " file deleted", Platform::FileLogger::LT_INFO);
}

bool FMAudioSample::LoadSampleFromFile(
	AudioWeakPtr audio,
	const std::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const Audio::SAMPLE_TYPE type)
{
	m_fileName = fileName;
	m_type = type;

    if (!FMAudioContext::IsStreamable(type))
	{
		Platform::FileBuffer out;
		fileManager->GetFileBuffer(m_fileName, out);
		if (!out)
		{
			m_logger.Log(m_fileName + " could not load buffer", Platform::Logger::LT_ERROR);
			return false;
		}

		return LoadSampleFromFileInMemory(audio, out->GetAddress(), out->GetBufferSize(), m_type);
	}
	else
	{
		try
		{
			Audio* pAudio = audio.lock().get();
			m_system = boost::any_cast<FMOD::System*>(pAudio->GetAudioContext());
		}
		catch (const boost::bad_any_cast&)
		{
			std::stringstream ss;
			ss << "FMAudioSample::LoadSampleFromFile: Invalid fmod system";
			m_logger.Log(ss.str(), Platform::Logger::LT_ERROR);
			return false;
		}

#if defined(ANDROID)
		if (fileManager->IsPacked())
		{
            const std::string prefix = "assets/";
            if (m_fileName.substr(0, prefix.size()) == prefix)
            {
                m_fileName.replace(0, prefix.size(), "file:///android_asset/");
            }
		}
#endif

		const FMOD_RESULT result = m_system->createSound(m_fileName.c_str(), FMOD_CREATESTREAM, 0, &m_sound);

		if (FMOD_ERRCHECK(result, m_logger))
			return false;

		m_logger.Log(m_fileName + " file loaded", Platform::Logger::LT_INFO);
        return true;
	}
}

bool FMAudioSample::LoadSampleFromFileInMemory(
	AudioWeakPtr audio,
	void* pBuffer,
	const unsigned int bufferLength,
	const Audio::SAMPLE_TYPE type)
{
	if (FMAudioContext::IsStreamable(type))
	{
		std::stringstream ss;
		ss << "FMAudioSample::LoadSampleFromFileInMemory: streaming music samples from virtual memory is not supported";
		m_logger.Log(ss.str(), Platform::Logger::LT_ERROR);
		return false;
	}

	try
	{
		Audio* pAudio = audio.lock().get();
		m_system = boost::any_cast<FMOD::System*>(pAudio->GetAudioContext());
	}
	catch (const boost::bad_any_cast&)
	{
		std::stringstream ss;
		ss << "FMAudioSample::LoadSampleFromFileInMemory: Invalid fmod system";
		m_logger.Log(ss.str(), Platform::Logger::LT_ERROR);
		return false;
	}

	const FMOD_MODE mode = (FMOD_OPENMEMORY | FMOD_CREATESAMPLE);

	FMOD_CREATESOUNDEXINFO info;
	memset(&info, 0, sizeof(FMOD_CREATESOUNDEXINFO));

	info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	info.length = static_cast<unsigned int>(bufferLength);
	info.suggestedsoundtype = FMOD_SOUND_TYPE_MPEG;

	const FMOD_RESULT result = m_system->createSound(static_cast<char*>(pBuffer), mode, &info, &m_sound);

    if (FMOD_ERRCHECK(result, m_logger))
        return false;

    m_logger.Log(m_fileName + " file loaded", Platform::Logger::LT_INFO);

	return true;
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
		FMOD_RESULT result = m_channel->setPitch(speed);

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
