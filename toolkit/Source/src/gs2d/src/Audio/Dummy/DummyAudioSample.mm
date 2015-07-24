/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#import "DummyAudioSample.h"

namespace gs2d {

AudioSamplePtr DummyAudioContext::LoadSampleFromFile(
	const str_type::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const Audio::SAMPLE_TYPE type)
{
	AudioSamplePtr sample = AudioSamplePtr(new DummyAudioSample);
	sample->LoadSampleFromFile(weak_this, fileName, fileManager, type);
	return sample;
}

AudioSamplePtr DummyAudioContext::LoadSampleFromFileInMemory(
	void *pBuffer,
	const unsigned int bufferLength,
	const Audio::SAMPLE_TYPE type)
{
	return AudioSamplePtr();
}

DummyAudioSample::DummyAudioSample()
{
}

DummyAudioSample::~DummyAudioSample()
{
}

bool DummyAudioSample::LoadSampleFromFile(
	AudioWeakPtr audio,
	const str_type::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const Audio::SAMPLE_TYPE type)
{
	return false;
}

bool DummyAudioSample::LoadSampleFromFileInMemory(
	AudioWeakPtr audio,
	void *pBuffer,
	const unsigned int bufferLength,
	const Audio::SAMPLE_TYPE type)
{
	return false;
}

bool DummyAudioSample::SetLoop(const bool enable)
{
	return false;
}

bool DummyAudioSample::GetLoop() const
{
	return false;
}

bool DummyAudioSample::Play()
{
	return false;
}

bool DummyAudioSample::Stop()
{
	return false;
}

bool DummyAudioSample::IsPlaying()
{
	return false;
}

Audio::SAMPLE_STATUS DummyAudioSample::GetStatus()
{
	return Audio::UNKNOWN_STATUS;
}

bool DummyAudioSample::Pause()
{
	return false;
}

Audio::SAMPLE_TYPE DummyAudioSample::GetType() const
{
	return Audio::UNKNOWN_TYPE;
}

bool DummyAudioSample::SetSpeed(const float speed)
{
	return false;
}

float DummyAudioSample::GetSpeed() const
{
	return 0.0f;
}

bool DummyAudioSample::SetVolume(const float volume)
{
	return false;
}

float DummyAudioSample::GetVolume() const
{
	return 0.0f;
}

bool DummyAudioSample::SetPan(const float pan)
{
	return 0.0f;
}

float DummyAudioSample::GetPan() const
{
	return 0.0f;
}

} // namespace gs2d
