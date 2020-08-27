#include "FMAudioContext.h"

#include "inc/fmod_errors.h"

#include <sstream>

namespace gs2d {

bool FMOD_ERRCHECK_fn(FMOD_RESULT result, const char *file, int line, Platform::FileLogger& logger)
{
	if (result != FMOD_OK)
	{
		std::stringstream ss;
		ss << file << "(" << line << "): FMOD Error " << result << " - " << FMOD_ErrorString(result);
		logger.Log(ss.str(), Platform::FileLogger::ERROR);
		return true;
	}
	return false;
}

FMOD::System* FMAudioContext::m_system = 0;

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

FMAudioContext::FMAudioContext() :
	m_globalVolume(1.0f),
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
		std::stringstream error_string;
		error_string << "FMOD lib version doesn't match header version: ";
		error_string << std::hex << version;
		m_logger.Log(error_string.str(), Platform::FileLogger::ERROR);
	}

	// Attemps to fix audio latency on Android
//#if defined(ANDROID)
	//result = m_system->setOutput(FMOD_OUTPUTTYPE_AUDIOTRACK);
	//FMOD_ERRCHECK(result, m_logger);

	//result = m_system->setDSPBufferSize(512, 2);
	//FMOD_ERRCHECK(result, m_logger);
//#endif

	result = m_system->init(32, FMOD_INIT_NORMAL, 0);
	if (FMOD_ERRCHECK(result, m_logger))
		return false;

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

void FMAudioContext::Suspend()
{
	if (m_system)
	{
		const FMOD_RESULT result = m_system->mixerSuspend();
		FMOD_ERRCHECK(result, m_logger);
	}
}

void FMAudioContext::Resume()
{
	if (m_system)
	{
		const FMOD_RESULT result = m_system->mixerResume();
		FMOD_ERRCHECK(result, m_logger);
	}
}
} // namespace gs2d
