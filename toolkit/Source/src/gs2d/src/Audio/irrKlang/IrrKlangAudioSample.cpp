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

#include "IrrKlangAudioSample.h"

namespace gs2d {

bool IrrKlangAudioSample::LoadSampleFromFile(
	AudioWeakPtr audio,
	const str_type::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const GS_SAMPLE_TYPE type)
{
	return false;
}

bool IrrKlangAudioSample::LoadSampleFromFileInMemory(AudioWeakPtr audio, void *pBuffer, const unsigned int bufferLength, const GS_SAMPLE_TYPE type)
{
	return false;
}

bool IrrKlangAudioSample::SetLoop(const bool enable)
{
	return false;
}

bool IrrKlangAudioSample::GetLoop() const
{
	return false;
}

bool IrrKlangAudioSample::Play()
{
	return false;
}

GS_SAMPLE_STATUS IrrKlangAudioSample::GetStatus()
{
	return GSSS_UNKNOWN;
}

bool IrrKlangAudioSample::IsPlaying()
{
	return false;
}

bool IrrKlangAudioSample::Pause()
{
	return false;
}

bool IrrKlangAudioSample::Stop()
{
	return false;
}

GS_SAMPLE_TYPE IrrKlangAudioSample::GetType() const
{
	return GSST_UNKNOWN;
}

bool IrrKlangAudioSample::SetSpeed(const float speed)
{
	return false;
}

float IrrKlangAudioSample::GetSpeed() const
{
	return false;
}

bool IrrKlangAudioSample::SetVolume(const float volume)
{
	return false;
}

float IrrKlangAudioSample::GetVolume() const
{
	return 0.0f;
}

bool IrrKlangAudioSample::SetPan(const float pan)
{
	return false;
}

float IrrKlangAudioSample::GetPan() const
{
	return 0.0f;
}

} // namespace gs2d
