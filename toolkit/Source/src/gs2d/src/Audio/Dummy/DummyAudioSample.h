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

#ifndef GS2D_CD_AUDIO_SAMPLE_H_
#define GS2D_CD_AUDIO_SAMPLE_H_

#include "../../Audio.h"

#include "DummyAudioContext.h"

namespace gs2d {

class DummyAudioSample : public AudioSample
{
public:
	DummyAudioSample();
	~DummyAudioSample();

	bool LoadSampleFromFile(
		AudioWeakPtr audio,
		const str_type::string& fileName,
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