#include "FMAudioSample.h"

#include <climits>
#include <sstream>
#include <vector>

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

		return LoadSampleFromFileInMemory(audio, out->GetAddress(), static_cast<unsigned int>(out->GetBufferSize()), m_type);
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
	if (FMAudioContext::IsSuspended())
		return false;

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
		float volumeMultiplier = 1.0f;
		if (!FMAudioContext::IsStreamable(GetType()))
		{
			volumeMultiplier = FMAudioContext::GetStaticSoundEffectVolume();
		}

		result = m_channel->setVolume(m_volume * volumeMultiplier);

		if (FMOD_ERRCHECK(result, m_logger))
			return false;

		SetLoop(m_loop);
		SetSpeed(m_speed);
		SetPan(m_pan);
	}
	return true;
}

bool FMAudioSample::SetLoop(const bool enable)
{
	m_loop = enable;

	if (FMAudioContext::IsSuspended())
		return false;

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
	if (FMAudioContext::IsSuspended())
		return false;

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

// the fade level the mixer applies at 'clock', interpolated from the channel's fade points
static float GetFadeLevel(FMOD::Channel* channel, const unsigned long long clock)
{
	unsigned int numPoints = 0;
	if (channel->getFadePoints(&numPoints, 0, 0) != FMOD_OK || numPoints == 0)
		return 1.0f;

	std::vector<unsigned long long> clocks(numPoints);
	std::vector<float> volumes(numPoints);
	if (channel->getFadePoints(&numPoints, &clocks[0], &volumes[0]) != FMOD_OK || numPoints == 0)
		return 1.0f;

	if (clock <= clocks[0])
		return volumes[0];

	for (unsigned int t = 1; t < numPoints; t++)
	{
		if (clock <= clocks[t])
		{
			const float bias = static_cast<float>(clock - clocks[t - 1]) / static_cast<float>(clocks[t] - clocks[t - 1]);
			return volumes[t - 1] + ((volumes[t] - volumes[t - 1]) * bias);
		}
	}
	return volumes[numPoints - 1];
}

bool FMAudioSample::FadeOut(const float seconds)
{
	if (FMAudioContext::IsSuspended())
		return false;

	if (seconds <= 0.0f)
		return Stop();

	if (m_channel)
	{
		// a paused channel is already silent, there is nothing to fade
		bool paused = false;
		FMOD_RESULT result = m_channel->getPaused(&paused);

		if ((result == FMOD_ERR_INVALID_HANDLE) || (result == FMOD_ERR_CHANNEL_STOLEN))
		{
			m_channel = 0;
			return true;
		}

		if (FMOD_ERRCHECK(result, m_logger))
			return false;

		if (paused)
			return Stop();

		// fade points and delays are scheduled on the parent channel group's DSP clock,
		// which advances by the mixer's sample rate every second
		int sampleRate = 0;
		result = m_system->getSoftwareFormat(&sampleRate, 0, 0);
		if (FMOD_ERRCHECK(result, m_logger))
			return false;

		unsigned long long parentClock = 0;
		result = m_channel->getDSPClock(0, &parentClock);
		if (FMOD_ERRCHECK(result, m_logger))
			return false;

		const unsigned long long fadeEnd = parentClock + static_cast<unsigned long long>(static_cast<double>(seconds) * sampleRate);

		// the mixer interpolates linearly between fade points, so the fade stays smooth even when
		// the main thread stalls. Not setFadePointRamp: it holds the level and only ramps right before
		// 'fadeEnd'. Start from the level an earlier fade may have reached (1 when there is none),
		// then drop that fade's remaining points; this order never lets the level jump
		result = m_channel->addFadePoint(parentClock, GetFadeLevel(m_channel, parentClock));
		if (FMOD_ERRCHECK(result, m_logger))
			return false;

		result = m_channel->removeFadePoints(parentClock + 1, ULLONG_MAX);
		if (FMOD_ERRCHECK(result, m_logger))
			return false;

		result = m_channel->addFadePoint(fadeEnd, 0.0f);
		if (FMOD_ERRCHECK(result, m_logger))
			return false;

		// and stops the channel at the sample where the fade reaches silence
		result = m_channel->setDelay(0, fadeEnd, true);
		if (FMOD_ERRCHECK(result, m_logger))
			return false;
	}
	return true;
}

bool FMAudioSample::IsPlaying()
{
	if (FMAudioContext::IsSuspended())
		return false;

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
	if (FMAudioContext::IsSuspended())
		return Audio::UNKNOWN_STATUS;

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
	if (FMAudioContext::IsSuspended())
		return false;

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

	if (FMAudioContext::IsSuspended())
		return false;

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

	if (FMAudioContext::IsSuspended())
		return false;

	// adjust volume in real-time of streamable samples (e.g. musics)
	if (m_channel && FMAudioContext::IsStreamable(GetType()))
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

	if (FMAudioContext::IsSuspended())
		return false;

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
