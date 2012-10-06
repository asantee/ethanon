/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

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

#ifndef GS2D_AUDIO_H_
#define GS2D_AUDIO_H_

#include "Platform/FileManager.h"

namespace gs2d {

enum GS_SAMPLE_STATUS
{
	GSSS_PLAYING = 0,
	GSSS_PAUSED = 1,
	GSSS_STOPPED = 2,
	GSSS_UNKNOWN = 3
};

enum GS_SAMPLE_TYPE
{
	GSST_SOUND_EFFECT = 0,
	GSST_MUSIC = 1,
	GSST_SOUNDTRACK = 2,
	GSST_AMBIENT_SFX = 3,
	GSST_UNKNOWN = 4
};

class AudioSample;
typedef boost::shared_ptr<AudioSample> AudioSamplePtr;
typedef boost::weak_ptr<AudioSample> AudioSampleWeakPtr;

class Audio;
typedef boost::shared_ptr<Audio> AudioPtr;
typedef boost::weak_ptr<Audio> AudioWeakPtr;

/**
 * \brief Abstracts an audio device context
 *
 * This class controls all audio device operations.
 * It may also be used to instantiate audio samples.
 */
class Audio
{
	friend GS2D_API AudioPtr CreateAudio(boost::any data);

	virtual bool CreateAudioDevice(boost::any data) = 0;
public:
	virtual AudioSamplePtr LoadSampleFromFile(const str_type::string& fileName, const Platform::FileManagerPtr& fileManager, const GS_SAMPLE_TYPE type = GSST_UNKNOWN) = 0;
	virtual AudioSamplePtr LoadSampleFromFileInMemory(void *pBuffer, const unsigned int bufferLength, const GS_SAMPLE_TYPE type = GSST_UNKNOWN) = 0;
	virtual boost::any GetAudioContext() = 0;
	virtual void SetGlobalVolume(const float volume) = 0;
	virtual float GetGlobalVolume() const = 0;
};

/**
 * \brief Abstracts an audio sample
 *
 * Stores and plays audio samples as soundtrack, sound effects or ambient sound.
 */
class AudioSample
{
public:
	virtual bool LoadSampleFromFile(AudioWeakPtr audio, const str_type::string& fileName, const Platform::FileManagerPtr& fileManager, const GS_SAMPLE_TYPE type = GSST_UNKNOWN) = 0;
	virtual bool LoadSampleFromFileInMemory(AudioWeakPtr audio, void *pBuffer, const unsigned int bufferLength, const GS_SAMPLE_TYPE type = GSST_UNKNOWN) = 0;

	virtual bool SetLoop(const bool enable) = 0;
	virtual bool GetLoop() const = 0;

	virtual bool Play() = 0;
	virtual GS_SAMPLE_STATUS GetStatus() = 0;

	virtual bool IsPlaying() = 0;

	virtual bool Pause() = 0;
	virtual bool Stop() = 0;

	virtual GS_SAMPLE_TYPE GetType() const = 0;

	virtual bool SetSpeed(const float speed) = 0;
	virtual float GetSpeed() const = 0;

	virtual bool SetVolume(const float volume) = 0;
	virtual float GetVolume() const = 0;

	virtual bool SetPan(const float pan) = 0;
	virtual float GetPan() const = 0;
};

/// Instantiates an Input object
GS2D_API AudioPtr CreateAudio(boost::any data);

} // namespace gs2d

#endif