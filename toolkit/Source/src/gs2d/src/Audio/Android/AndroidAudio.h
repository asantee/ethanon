#ifndef GS2D_ANDROID_AUDIO_H_
#define GS2D_ANDROID_AUDIO_H_

#include "../../Audio.h"
#include "../../Platform/FileLogger.h"
#include "../../Platform/android/Platform.android.h"

#include <boost/weak_ptr.hpp>

namespace gs2d {

class AndroidAudioContext : public Audio, public Platform::NativeCommandForwarder
{
	bool CreateAudioDevice(boost::any data);
	Platform::FileLogger m_logger;

	boost::weak_ptr<AndroidAudioContext> weak_this;

	AndroidAudioContext();

	float m_globalVolume;

public:
	static boost::shared_ptr<AndroidAudioContext> Create(boost::any data);

	static bool IsStreamable(const Audio::SAMPLE_TYPE type);

	AudioSamplePtr LoadSampleFromFile(
		const std::string& fileName,
		const Platform::FileManagerPtr& fileManager,
		const Audio::SAMPLE_TYPE type = Audio::UNKNOWN_TYPE);

	AudioSamplePtr LoadSampleFromFileInMemory(
		void *pBuffer,
		const unsigned int bufferLength,
		const Audio::SAMPLE_TYPE type = Audio::UNKNOWN_TYPE);

	boost::any GetAudioContext();

	void Update();

	void SetGlobalVolume(const float volume);
	float GetGlobalVolume() const;
};

typedef boost::shared_ptr<AndroidAudioContext> AndroidAudioContextPtr;
typedef boost::shared_ptr<AndroidAudioContext> AndroidAudioContextWeakPtr;

class AndroidAudioSample : public AudioSample
{
	AndroidAudioContext* m_audio;

	std::string m_fileName;
	float m_volume, m_speed;
	bool m_loop;
	Audio::SAMPLE_TYPE m_type;
	static Platform::FileLogger m_logger;

public:
	AndroidAudioSample();
	~AndroidAudioSample();

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
