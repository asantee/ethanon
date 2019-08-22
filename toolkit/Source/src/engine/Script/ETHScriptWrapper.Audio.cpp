#include "ETHScriptWrapper.h"

bool ETHScriptWrapper::LoadMusic(const std::string &file)
{
	if (WarnIfRunsInMainFunction(("LoadMusic")))
		return false;

	std::stringstream ss;
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();
	ss << fileIOHub->GetResourceDirectory() << file;
	if (!m_provider->GetAudioResourceManager()->AddFile(m_provider->GetAudio(), fileIOHub, ss.str(), Audio::MUSIC))
	{
		ShowMessage(("Could not load the file: ") + file, ETH_ERROR, false);
		return false;
	}
	return true;
}

bool ETHScriptWrapper::LoadSoundEffect(const std::string &file)
{
	if (WarnIfRunsInMainFunction(("LoadSoundEffect")))
		return false;

	std::stringstream ss;
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();
	ss << fileIOHub->GetResourceDirectory() << file;
	if (!m_provider->GetAudioResourceManager()->AddFile(m_provider->GetAudio(), fileIOHub, ss.str(), Audio::SOUND_EFFECT))
	{
		ShowMessage(("Could not load the file: ") + file, ETH_ERROR, false);
		return false;
	}
	return true;
}

bool ETHScriptWrapper::PlaySample(const std::string &file)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, (""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->Play();
	return true;
}

bool ETHScriptWrapper::LoopSample(const std::string &file, const bool loop)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, (""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->SetLoop(loop);
	return true;
}

bool ETHScriptWrapper::StopSample(const std::string &file)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, (""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->Stop();
	return true;
}

bool ETHScriptWrapper::PauseSample(const std::string &file)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, (""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->Pause();
	return true;
}

bool ETHScriptWrapper::SetSampleVolume(const std::string &file, const float volume)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, (""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	const float volumeFix = Max(Min(volume, 1.0f), 0.0f);
	pSample->SetVolume(volumeFix);
	return true;
}

bool ETHScriptWrapper::SetSamplePan(const std::string &file, const float pan)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, (""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->SetPan(pan);
	return true;
}

bool ETHScriptWrapper::SetSampleSpeed(const std::string &file, const float speed)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, (""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->SetSpeed(speed);
	return true;
}

bool ETHScriptWrapper::SampleExists(const std::string &file)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, (""), Audio::UNKNOWN_TYPE);

	if (!pSample)
		return false;
	return true;
}

bool ETHScriptWrapper::IsSamplePlaying(const std::string &file)
{
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();

	AudioSamplePtr pSample =
		m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), fileIOHub, file, (""), Audio::UNKNOWN_TYPE);

	if (!pSample)
	{
		ShowMessage(("File not found: ") + file, ETH_ERROR, false);
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

