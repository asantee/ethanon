#ifndef GS2D_CD_AUDIO_SAMPLE_H_
#define GS2D_CD_AUDIO_SAMPLE_H_

#include "../../Audio.h"

#include "FMAudioContext.h"

#include <fmod.hpp>

namespace gs2d {

class FMAudioSample : public AudioSample
{
	FMOD::System* m_system;
	FMOD::Sound* m_sound;
	FMOD::Channel* m_channel;

	std::string m_fileName;
	static std::string m_currentStreamableTrack;
	float m_volume, m_speed, m_pan;
	bool m_loop;
	Audio::SAMPLE_TYPE m_type;
	static Platform::FileLogger m_logger;

public:
	FMAudioSample();
	~FMAudioSample();

	bool LoadSampleFromFile(
		AudioWeakPtr audio,
		const std::string& fileName,
		const Platform::FileManagerPtr& fileManager,
		const Audio::SAMPLE_TYPE type = Audio::UNKNOWN_TYPE);

	bool LoadSampleFromFileInMemory(
		AudioWeakPtr audio,
		void *pBuffer,
		const unsigned int bufferLength,
		const Audio::SAMPLE_TYPE type = Audio::UNKNOWN_TYPE);

	bool SetLoop(const bool enable);
	bool GetLoop() const;

	bool Play();
	Audio::SAMPLE_STATUS GetStatus();

	bool IsPlaying();

	bool Pause();
	bool Stop();

	Audio::SAMPLE_TYPE GetType() const;

	bool SetSpeed(const float speed);
	float GetSpeed() const;

	bool SetVolume(const float volume);
	float GetVolume() const;

	bool SetPan(const float pan);
	float GetPan() const;
};

} // namespace gs2d

#endif
