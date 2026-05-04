#include "FMAudioContext.h"

#include <sstream>

#include <fmod_errors.h>

#include <math.h>

namespace gs2d {

bool FMOD_ERRCHECK_fn(FMOD_RESULT result, const char *file, int line, const Platform::FileLogger& logger)
{
	if (result != FMOD_OK)
	{
		std::stringstream ss;
		ss << file << "(" << line << "): FMOD Error " << result << " - " << FMOD_ErrorString(result);
		logger.Log(ss.str(), Platform::FileLogger::LT_ERROR);
		return true;
	}
	return false;
}

FMOD::System* FMAudioContext::m_system = 0;
float FMAudioContext::m_soundEffectVolume = 1.0f;

AudioPtr CreateAudio(boost::any data)
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

bool FMAudioContext::m_suspended(false);

FMAudioContext::FMAudioContext() :
	m_logger(Platform::FileLogger::GetLogDirectory() + "FMAudioContext.log.txt")
{
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
	CommonInit(m_logger);
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
		std::stringstream errorString;
		errorString << "FMOD lib version doesn't match header version: ";
		errorString << std::hex << version;
		m_logger.Log(errorString.str(), Platform::Logger::LT_ERROR);
	}

	// Attempts to fix audio latency and improve capture compatibility on Android
#if defined(ANDROID)
	result = m_system->setOutput(FMOD_OUTPUTTYPE_AAUDIO);
	if (FMOD_ERRCHECK(result, m_logger))
		result = m_system->setOutput(FMOD_OUTPUTTYPE_OPENSL);
	if (FMOD_ERRCHECK(result, m_logger))
		result = m_system->setOutput(FMOD_OUTPUTTYPE_AUDIOTRACK);
	FMOD_ERRCHECK(result, m_logger);

	// Try to match the native block size for low latency
	result = m_system->setDSPBufferSize(256, 4);
	FMOD_ERRCHECK(result, m_logger);
#else
	result = m_system->setDSPBufferSize(512, 2);
	FMOD_ERRCHECK(result, m_logger);
#endif

	result = m_system->init(32, FMOD_INIT_NORMAL, 0);
	if (FMOD_ERRCHECK(result, m_logger))
		return false;

	m_logger.Log("Audio device initialized", Platform::FileLogger::LT_INFO);
	return true;
}

boost::any FMAudioContext::GetAudioContext()
{
	return m_system;
}

float FMAudioContext::GetStaticSoundEffectVolume()
{
	return m_soundEffectVolume;
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
	FMOD::ChannelGroup* channelGroup = 0;

	FMOD_RESULT result;
	result = m_system->getMasterChannelGroup(&channelGroup);

	if (FMOD_ERRCHECK(result, m_logger))
		return;
	
	if (channelGroup)
	{
		const float normalizedVolume = fmin(fmax(volume, 0.0f), 1.0f);
		result = channelGroup->setVolume(normalizedVolume);

		if (FMOD_ERRCHECK(result, m_logger))
			return;
	}
}

float FMAudioContext::GetGlobalVolume() const
{
	FMOD::ChannelGroup* channelGroup = 0;

	FMOD_RESULT result;
	result = m_system->getMasterChannelGroup(&channelGroup);

	if (FMOD_ERRCHECK(result, m_logger))
		return 0.0f;

	float volume = 0.0f;
	if (channelGroup)
	{
		result = channelGroup->getVolume(&volume);

		if (FMOD_ERRCHECK(result, m_logger))
			return 0.0f;
	}
	return volume;
}

void FMAudioContext::SetSoundEffectVolume(const float volume)
{
	m_soundEffectVolume = fmin(fmax(volume, 0.0f), 1.0f);
}

float FMAudioContext::GetSoundEffectVolume() const
{
	return m_soundEffectVolume;
}

void FMAudioContext::SetMute(const bool mute)
{
	FMOD::ChannelGroup* channelGroup = 0;

	FMOD_RESULT result;
	result = m_system->getMasterChannelGroup(&channelGroup);

	if (FMOD_ERRCHECK(result, m_logger))
		return;

	if (channelGroup)
	{
		result = channelGroup->setMute(mute);

		if (FMOD_ERRCHECK(result, m_logger))
			return;
	}
}

bool FMAudioContext::IsMute() const
{
	FMOD::ChannelGroup* channelGroup = 0;

	FMOD_RESULT result;
	result = m_system->getMasterChannelGroup(&channelGroup);

	if (FMOD_ERRCHECK(result, m_logger))
		return false;

	bool r = false;
	if (channelGroup)
	{
		result = channelGroup->getMute(&r);

		if (FMOD_ERRCHECK(result, m_logger))
			return false;
	}
	return r;

}

void FMAudioContext::Update()
{
	if (!m_system)
		return;

	const FMOD_RESULT result = m_system->update();
	FMOD_ERRCHECK(result, m_logger);
}

void FMAudioContext::Suspend()
{
	if (!m_system)
		return;

	m_suspended = true;

	const FMOD_RESULT result = m_system->mixerSuspend();
	FMOD_ERRCHECK(result, m_logger);
}

void FMAudioContext::Resume()
{
	if (!m_system)
		return;

	const FMOD_RESULT result = m_system->mixerResume();
	FMOD_ERRCHECK(result, m_logger);
	m_suspended = false;
}

bool FMAudioContext::IsSuspended()
{
	return m_suspended;
}

} // namespace gs2d
