#include "AudioDummy.h"

namespace gs2d {

bool AudioDummy::CreateAudioDevice(boost::any data)
{
	return true;
}

AudioSamplePtr AudioDummy::LoadSampleFromFile(
	const std::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const SAMPLE_TYPE type)
{
	return AudioSampleDummyPtr(new AudioSampleDummy);
}

AudioSamplePtr AudioDummy::LoadSampleFromFileInMemory(
	void *pBuffer,
	const unsigned int bufferLength,
	const SAMPLE_TYPE type)
{
	return AudioSampleDummyPtr(new AudioSampleDummy);
}

boost::any AudioDummy::GetAudioContext()
{
	return 0;
}

void AudioDummy::SetGlobalVolume(const float volume)
{
}

float AudioDummy::GetGlobalVolume() const
{
	return 0.0f;
}

void AudioDummy::Update()
{
}

void AudioDummy::Suspend()
{
}

void AudioDummy::Resume()
{
}

bool AudioSampleDummy::LoadSampleFromFile(
	AudioWeakPtr audio,
	const std::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const Audio::SAMPLE_TYPE type)
{
	return true;
}

bool AudioSampleDummy::LoadSampleFromFileInMemory(
	AudioWeakPtr audio,
	void *pBuffer,
	const unsigned int bufferLength,
	const Audio::SAMPLE_TYPE type)
{
	return true;
}

bool AudioSampleDummy::SetLoop(const bool enable)
{
	return true;
}

bool AudioSampleDummy::GetLoop() const
{
	return true;
}

bool AudioSampleDummy::Play()
{
	return true;
}

Audio::SAMPLE_STATUS AudioSampleDummy::GetStatus()
{
	return Audio::UNKNOWN_STATUS;
}

bool AudioSampleDummy::IsPlaying()
{
	return true;
}

bool AudioSampleDummy::Pause()
{
	return true;
}

bool AudioSampleDummy::Stop()
{
	return true;
}

Audio::SAMPLE_TYPE AudioSampleDummy::GetType() const
{
	return Audio::UNKNOWN_TYPE;
}

bool AudioSampleDummy::SetSpeed(const float speed)
{
	return true;
}

float AudioSampleDummy::GetSpeed() const
{
	return true;
}

bool AudioSampleDummy::SetVolume(const float volume)
{
	return true;
}

float AudioSampleDummy::GetVolume() const
{
	return true;
}

bool AudioSampleDummy::SetPan(const float pan)
{
	return true;
}

float AudioSampleDummy::GetPan() const
{
	return 0.0f;
}

} // namespace gs2d
