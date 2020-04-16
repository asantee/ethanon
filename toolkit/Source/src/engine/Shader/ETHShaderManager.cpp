#include "ETHShaderManager.h"
#include "../Scene/ETHScene.h"

ETHShaderManager::ETHShaderManager(VideoPtr video) :
	m_lastAM(Video::AM_PIXEL)
{
	m_video = video;
}

bool ETHShaderManager::BeginAmbientPass(const ETHSpriteEntity *pRender, const float maxHeight, const float minHeight)
{
	m_lastAM = m_video->GetAlphaMode();
	return true;
}

bool ETHShaderManager::EndAmbientPass()
{
	if (m_lastAM != m_video->GetAlphaMode())
	{
		m_video->SetAlphaMode(m_lastAM);
	}
	return true;
}

bool ETHShaderManager::BeginHaloPass(const ETHLight* light)
{
	if (!light || light->haloBitmap == (""))
		return false;

	m_lastAM = m_video->GetAlphaMode();
	m_video->SetAlphaMode(Video::AM_ADD);
	return true;
}

bool ETHShaderManager::EndHaloPass()
{
	m_video->SetAlphaMode(m_lastAM);
	return true;
}

bool ETHShaderManager::BeginParticlePass(const ETHParticleSystem& system)
{
	return true;
}

bool ETHShaderManager::EndParticlePass()
{
	return true;
}
