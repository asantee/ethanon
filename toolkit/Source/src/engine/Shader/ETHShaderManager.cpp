#include "ETHShaderManager.h"
#include "../Scene/ETHScene.h"

ETHShaderManager::ETHShaderManager(VideoPtr video, const str_type::string& shaderPath, const bool richLighting) :
	m_lastAM(Video::AM_PIXEL)
{
	m_video = video;

	m_opaqueSprite = SpritePtr(new Sprite(m_video.get(), ETHGlobal::GetDataResourceFullPath(shaderPath, "default_nm.png"), 1.0f));
}

void ETHShaderManager::SetParallaxIntensity(const float intensity)
{
	m_parallaxIntensity = intensity;
}

float ETHShaderManager::GetParallaxIntensity() const
{
	return m_parallaxIntensity;
}

Vector2 ETHShaderManager::ComputeParallaxOffset(const VideoPtr& video, const Vector3& pos, const float& individualParallaxIntensity) const
{
	const Vector2 screenSize(video->GetScreenSizeF());
	const Vector2 halfScreenSize(screenSize * 0.5f);
	const Vector2 screenSpacePos(Vector2(pos.x, pos.y) - video->GetCameraPos());
	const float intensity = pos.z * individualParallaxIntensity * m_parallaxIntensity * Sprite::PARALLAX_INTENSITY_FIX;
	return ((screenSpacePos - halfScreenSize) / screenSize.y) * intensity;
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

SpritePtr ETHShaderManager::GetOpaqueSprite()
{
	return m_opaqueSprite;
}

bool ETHShaderManager::BeginHaloPass(const ETHLight* light)
{
	if (!light || light->haloBitmap == GS_L(""))
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
