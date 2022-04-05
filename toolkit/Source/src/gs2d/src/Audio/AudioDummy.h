#ifndef GS2D_AUDIO_DUMMY_H_
#define GS2D_AUDIO_DUMMY_H_

#include "Audio.h"

namespace gs2d {

class AudioSampleDummy;
typedef boost::shared_ptr<AudioSampleDummy> AudioSampleDummyPtr;

class AudioDummy : public Audio
{
	bool CreateAudioDevice(boost::any data) override;

public:
	AudioSamplePtr LoadSampleFromFile(
		const std::string& fileName,
		const Platform::FileManagerPtr& fileManager,
		const SAMPLE_TYPE type) override;

	AudioSamplePtr LoadSampleFromFileInMemory(
		void *pBuffer,
		const unsigned int bufferLength,
		const SAMPLE_TYPE type) override;

	boost::any GetAudioContext() override;
	void SetGlobalVolume(const float volume) override;
	float GetGlobalVolume() const override;

	void Update() override;
};

typedef boost::shared_ptr<AudioDummy> AudioDummyPtr;

class AudioSampleDummy : public AudioSample
{
public:
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
