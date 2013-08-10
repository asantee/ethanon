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

#include "IrrKlangAudioSample.h"

#include "IrrKlangAudio.h"

#include "../../Platform/StdAnsiFileManager.h"

#include "../../Application.h"

namespace gs2d {

IrrKlangAudioSample::IrrKlangAudioSample() :
	m_type(Audio::UNKNOWN_TYPE),
	m_engine(0),
	m_source(0),
	m_sound(0),
	m_loop(false),
	m_speed(1.0f),
	m_pan(0.0f)
{
}

IrrKlangAudioSample::~IrrKlangAudioSample()
{
	if (m_engine && m_source)
		m_engine->removeSoundSource(m_source);

	if (m_sound)
		m_sound->drop();

	if (m_engine)
		m_engine->drop();
}

bool IrrKlangAudioSample::LoadSampleFromFile(
	AudioWeakPtr audio,
	const str_type::string& fileName,
	const Platform::FileManagerPtr& fileManager,
	const Audio::SAMPLE_TYPE type)
{
	m_fullFilePath = fileName;
	Platform::FileBuffer out;
	fileManager->GetFileBuffer(fileName, out);

	if (!out)
	{
		ShowMessage(str_type::string("Couldn't load sound file: ") + fileName, GSMT_ERROR);
		return false;
	}
	return LoadSampleFromFileInMemory(audio, out->GetAddress(), out->GetBufferSize(), type);
}

bool IrrKlangAudioSample::LoadSampleFromFileInMemory(
	AudioWeakPtr audio,
	void *pBuffer,
	const unsigned int bufferLength,
	const Audio::SAMPLE_TYPE type)
{
	IrrKlangAudioPtr irrAudio = boost::dynamic_pointer_cast<IrrKlangAudio>(audio.lock());
	m_engine = irrAudio->GetEngine();
	m_engine->grab();
	m_type = type;

	// if it already exists, remove the existing data in order to load it again
	if (m_engine->getSoundSource(m_fullFilePath.c_str(), false))
		m_engine->removeSoundSource(m_fullFilePath.c_str());

	m_source = m_engine->addSoundSourceFromMemory(pBuffer, bufferLength, m_fullFilePath.c_str(), true);

	if (m_source)
	{
		m_source->setStreamMode(irrklang::ESM_AUTO_DETECT);
		m_sound = m_engine->play2D(m_source, false, true, true);
		if (m_sound)
		{
			return true;
		}
		else
		{
			ShowMessage(str_type::string("Couldn't play sound sample: ") + m_fullFilePath, GSMT_ERROR);
			return false;
		}
	}
	else
	{
		ShowMessage(str_type::string("Couldn't load sound sample: ") + m_fullFilePath, GSMT_ERROR);
		return false;
	}
}

bool IrrKlangAudioSample::Play()
{
	if (m_sound->isFinished())
	{
		m_sound->drop();
		m_sound = m_engine->play2D(m_source, m_loop, true, true);
		m_sound->setPlaybackSpeed(m_speed);
		m_sound->setPan(m_pan);
	}
	else
	{
		m_sound->setPlayPosition(0);
	}
	m_sound->setIsPaused(false);
	return true;
}

bool IrrKlangAudioSample::Stop()
{
	Pause();
	return m_sound->setPlayPosition(0);
}

bool IrrKlangAudioSample::SetVolume(const float volume)
{
	m_source->setDefaultVolume(volume);
	return true;
}

float IrrKlangAudioSample::GetVolume() const
{
	return m_source->getDefaultVolume();
}

bool IrrKlangAudioSample::IsPlaying()
{
	return (!m_sound->getIsPaused() && !m_sound->isFinished());
}

Audio::SAMPLE_STATUS IrrKlangAudioSample::GetStatus()
{
	if (IsPlaying())
		return Audio::PLAYING;
	else if (m_sound->getIsPaused() && m_sound->getPlayPosition() > 0)
		return Audio::PAUSED;
	else
		return Audio::STOPPED;
}

bool IrrKlangAudioSample::SetLoop(const bool enable)
{
	m_sound->setIsLooped(enable);
	m_loop = enable;
	return true;
}

bool IrrKlangAudioSample::GetLoop() const
{
	return m_sound->isLooped();
}

bool IrrKlangAudioSample::Pause()
{
	m_sound->setIsPaused(true);
	return true;
}

Audio::SAMPLE_TYPE IrrKlangAudioSample::GetType() const
{
	return m_type;
}

bool IrrKlangAudioSample::SetSpeed(const float speed)
{
	m_speed = math::Max(0.5f,(math::Min(2.0f, speed)));
	return m_sound->setPlaybackSpeed(speed);
}

float IrrKlangAudioSample::GetSpeed() const
{
	return m_sound->getPlaybackSpeed();
}

bool IrrKlangAudioSample::SetPan(const float pan)
{
	m_pan = pan;
	m_sound->setPan(pan);
	return true;
}

float IrrKlangAudioSample::GetPan() const
{
	return m_sound->getPan();
}

} // namespace gs2d
