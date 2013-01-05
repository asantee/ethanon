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

#include "ETHScriptWrapper.h"

bool ETHScriptWrapper::LoadMusic(const str_type::string &file)
{
	if (WarnIfRunsInMainFunction(GS_L("LoadMusic")))
		return false;

	str_type::stringstream ss;
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();
	ss << fileIOHub->GetResourceDirectory() << file;
	if (!m_provider->GetAudioResourceManager()->AddFile(m_provider->GetAudio(), fileIOHub, ss.str(), Audio::MUSIC))
	{
		ShowMessage(GS_L("Could not load the file: ") + file, ETH_ERROR, false);
		return false;
	}
	return true;
}

bool ETHScriptWrapper::LoadSoundEffect(const str_type::string &file)
{
	if (WarnIfRunsInMainFunction(GS_L("LoadSoundEffect")))
		return false;

	str_type::stringstream ss;
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();
	ss << fileIOHub->GetResourceDirectory() << file;
	if (!m_provider->GetAudioResourceManager()->AddFile(m_provider->GetAudio(), fileIOHub, ss.str(), Audio::SOUND_EFFECT))
	{
		ShowMessage(GS_L("Could not load the file: ") + file, ETH_ERROR, false);
		return false;
	}
	return true;
}

bool ETHScriptWrapper::PlaySample(const str_type::string &file)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, GS_L(""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->Play();
	return true;
}

bool ETHScriptWrapper::LoopSample(const str_type::string &file, const bool loop)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, GS_L(""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->SetLoop(loop);
	return true;
}

bool ETHScriptWrapper::StopSample(const str_type::string &file)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, GS_L(""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->Stop();
	return true;
}

bool ETHScriptWrapper::PauseSample(const str_type::string &file)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, GS_L(""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->Pause();
	return true;
}

bool ETHScriptWrapper::SetSampleVolume(const str_type::string &file, const float volume)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, GS_L(""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	const float volumeFix = Max(Min(volume, 1.0f), 0.0f);
	pSample->SetVolume(volumeFix);
	return true;
}

bool ETHScriptWrapper::SetSamplePan(const str_type::string &file, const float pan)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, GS_L(""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->SetPan(pan);
	return true;
}

bool ETHScriptWrapper::SetSampleSpeed(const str_type::string &file, const float speed)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, GS_L(""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->SetSpeed(speed);
	return true;
}

bool ETHScriptWrapper::SampleExists(const str_type::string &file)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, GS_L(""), Audio::UNKNOWN_TYPE);

	if (!pSample)
		return false;
	return true;
}

bool ETHScriptWrapper::IsSamplePlaying(const str_type::string &file)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, GS_L(""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	return pSample->IsPlaying();
}

void ETHScriptWrapper::SetGlobalVolume(const float volume)
{
	m_provider->GetAudio()->SetGlobalVolume(volume);
}

float ETHScriptWrapper::GetGlobalVolume()
{
	return m_provider->GetAudio()->GetGlobalVolume();
}

