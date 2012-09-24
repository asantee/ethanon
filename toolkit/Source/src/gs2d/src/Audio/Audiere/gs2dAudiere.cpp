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

#include "gs2dAudiere.h"
#include "../../gs2d.h"
#include "../../unicode/utf8converter.h"

namespace gs2d {
using namespace math;

GS2D_API AudioPtr CreateAudio(boost::any data)
{
	AudiereContextPtr audio = AudiereContext::Create(data);
	if (audio)
	{
		return audio;
	}
	return AudioPtr();
}

AudiereContext::AudiereContext() :
	m_globalVolume(1.0f)
{
}

boost::shared_ptr<AudiereContext> AudiereContext::Create(boost::any data)
{
	boost::shared_ptr<AudiereContext> p(new AudiereContext());
	p->weak_this = p;
	if (p->CreateAudioDevice(data))
	{
		return p;
	}
	else
	{
		return AudiereContextPtr();
	}
}

AudioSamplePtr AudiereContext::LoadSampleFromFile(const std::wstring& fileName, const Platform::FileManagerPtr& fileManager, const GS_SAMPLE_TYPE type)
{
	AudioSamplePtr audio = AudioSamplePtr(new AudiereSample);
	if (!audio->LoadSampleFromFile(weak_this, fileName, fileManager, type))
	{
		return AudioSamplePtr();
	}
	return audio;
}

AudioSamplePtr AudiereContext::LoadSampleFromFileInMemory(void *pBuffer, const unsigned int bufferLength, const GS_SAMPLE_TYPE type)
{
	AudioSamplePtr audio = AudioSamplePtr(new AudiereSample);
	if (!audio->LoadSampleFromFileInMemory(weak_this, pBuffer, bufferLength, type))
	{
		return AudioSamplePtr();
	}
	return audio;
}

bool AudiereContext::CreateAudioDevice(boost::any data)
{
	m_device = audiere::OpenDevice();
	if (!m_device)
	{
		ShowMessage(L"Audiere initialization failed - AudiereContext::CreateAudioDevice");
		return false;
	}
	return true;
}

boost::any AudiereContext::GetAudioContext()
{
	return m_device;
}

void AudiereContext::SetGlobalVolume(const float volume)
{
	m_globalVolume = volume;
}

float AudiereContext::GetGlobalVolume() const
{
	return m_globalVolume;
}

AudiereSample::AudiereSample()
{
	m_output = NULL;
	m_status = GSSS_STOPPED;
	m_position = 0;
	m_type = GSST_UNKNOWN;
	m_volume = 0.0f;
}

AudiereSample::~AudiereSample()
{
	if (m_output)
	{
		m_output->stop();
	}
}

bool AudiereSample::LoadSampleFromFile(AudioWeakPtr audio, const std::wstring& fileName,
									   const Platform::FileManagerPtr& fileManager, const GS_SAMPLE_TYPE type)
{
	Platform::FileBuffer out;
	fileManager->GetFileBuffer(fileName, out);
	bool r = false;

	if (out)
	{
		r = LoadSampleFromFileInMemory(audio, out->GetAddress(), out->GetBufferSize(), type);
	}

	if (!r)
	{
		std::wstringstream ss;
		ss << L"Failed while loading the file: " << fileName;
		ShowMessage(ss);
		return false;
	}
	return r;
}

bool AudiereSample::LoadSampleFromFileInMemory(AudioWeakPtr audio, void *pBuffer, const unsigned int bufferLength, const GS_SAMPLE_TYPE type)
{
	m_audio = audio;
	m_type = type;

	bool stream;
	switch (m_type)
	{
	case GSST_MUSIC:
	case GSST_AMBIENT_SFX:
	case GSST_SOUNDTRACK:
		stream = true;
		break;
	case GSST_SOUND_EFFECT:
	case GSST_UNKNOWN:
	default:
		stream = false;
		break;
	};

	audiere::AudioDevicePtr device;
	try
	{
		device = boost::any_cast<audiere::AudioDevicePtr>(m_audio.lock()->GetAudioContext());
	}
	catch (const boost::bad_any_cast &)
	{
		return false;
	}

	audiere::FilePtr file = audiere::CreateMemoryFile(pBuffer, bufferLength);
	m_output = audiere::OpenSound(device, file, stream);

	if (!m_output)
	{
		return false;
	}
	else
	{
		SetVolume(1.0f);
		return true;
	}
}

GS_SAMPLE_TYPE AudiereSample::GetType() const
{
	return m_type;
}

GS_SAMPLE_STATUS AudiereSample::GetStatus()
{
	if (!m_output->isPlaying() && m_status == GSSS_PLAYING)
		m_status = GSSS_STOPPED;

	return m_status;
}

bool AudiereSample::IsPlaying()
{
	return (GetStatus() == GSSS_PLAYING);
}

bool AudiereSample::SetLoop(const bool enable)
{
	m_output->setRepeat(enable);
	return true;
}

bool AudiereSample::GetLoop() const
{
	return m_output->getRepeat();
}

bool AudiereSample::Play()
{
	if (m_status == GSSS_PAUSED)
	{
		m_output->setPosition(m_position);
	}
	else
	{
		if (m_output->isPlaying())
			m_output->reset();
	}
	m_output->setVolume(m_audio.lock().get()->GetGlobalVolume() * m_volume);
	m_output->play();
	m_status = GSSS_PLAYING;
	return true;
}

bool AudiereSample::Pause()
{
	if (m_output->isPlaying())
	{
		m_position = m_output->getPosition();
		m_output->stop();
	}
	m_status = GSSS_PAUSED;
	return true;
}

bool AudiereSample::Stop()
{
	m_output->stop();
	m_output->reset();
	m_position = 0;
	m_status = GSSS_STOPPED;
	return true;
}

bool AudiereSample::SetSpeed(const float speed)
{
	m_output->setPitchShift(Min(Max(speed, 0.5f), 2.0f));
	return true;
}

float AudiereSample::GetSpeed() const
{
	return m_output->getPitchShift();
}

bool AudiereSample::SetPan(const float pan)
{
	m_output->setPan(Min(Max(pan, -1.0f), 1.0f));
	return true;
}

float AudiereSample::GetPan() const
{
	return m_output->getPan();
}

bool AudiereSample::SetVolume(const float volume)
{
	m_volume = volume;
	m_output->setVolume(m_volume * m_audio.lock().get()->GetGlobalVolume());
	return true;
}

float AudiereSample::GetVolume() const
{
	return m_volume;
}

} // namespace gs2d
