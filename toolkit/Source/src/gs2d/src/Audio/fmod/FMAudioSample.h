#ifndef GS2D_CD_AUDIO_SAMPLE_H_
#define GS2D_CD_AUDIO_SAMPLE_H_

#include "../../Audio.h"

#include "FMAudioContext.h"

// fmod.hpp is already included by FMAudioContext.h as <fmod.hpp>. Do not include the checked-in
// copy (inc/fmod.hpp) by relative path here: on Windows that copy (2.03.x) differs from the FMOD
// SDK installed under Program Files that the DLL is copied from, and mixing the two header versions
// in one build would only be masked by the include guard. Each platform puts exactly one FMOD
// header directory on its include path (Windows: the SDK install, Android/iOS: Audio/fmod/inc).

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
