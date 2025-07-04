#ifndef GS2D_CD_AUDIO_SAMPLE_H_
#define GS2D_CD_AUDIO_SAMPLE_H_

#include "../../Audio.h"

#include "FMAudioContext.h"

#include "inc/fmod.hpp"

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
		const Audio::SAMPLE_TYPE type) override;

	bool LoadSampleFromFileInMemory(
		AudioWeakPtr audio,
		void *pBuffer,
		const unsigned int bufferLength,
		const Audio::SAMPLE_TYPE type) override;

	bool SetLoop(const bool enable) override;
	bool GetLoop() const override;

	bool Play() override;
	Audio::SAMPLE_STATUS GetStatus() override;

	bool IsPlaying() override;

	bool Pause() override;
	bool Stop() override;

	Audio::SAMPLE_TYPE GetType() const override;

	bool SetSpeed(const float speed) override;
	float GetSpeed() const override;

	bool SetVolume(const float volume) override;
	float GetVolume() const override;

	bool SetPan(const float pan) override;
	float GetPan() const override;
};

} // namespace gs2d

#endif
