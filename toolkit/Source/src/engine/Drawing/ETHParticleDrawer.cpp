#include "ETHParticleDrawer.h"
#include "../Resource/ETHResourceProvider.h"
#include "../Resource/ETHDirectories.h"

ETHParticleDrawer::ETHParticleDrawer(
	const ETHResourceProviderPtr& provider,
	ETHGraphicResourceManagerPtr graphicResources,
	ETHShaderManagerPtr shaderManager,
	const std::string& resourceDirectory,
	const std::string& fileName,
	const Vector2& pos,
	const float angle,
	const float scale) :
	m_shaderManager(shaderManager),
	m_pos(pos),
	m_angle(angle),
	m_provider(provider),
	m_fileName(fileName)
{
	ETHParticleSystem particleSystem;
	particleSystem.ReadFromFile(resourceDirectory + ETHDirectories::GetEffectsDirectory() + fileName, provider->GetFileManager());
	const Vector3 startPos(Vector3(m_pos + provider->GetVideo()->GetCameraPos(), 0.0f));
	m_particleManager = ETHParticleManagerPtr(new ETHParticleManager(provider, particleSystem, startPos, angle, scale));
	m_particleManager->LoadGraphicResources();
}

bool ETHParticleDrawer::Draw(const unsigned long lastFrameElapsedTimeMS)
{
	m_particleManager->UpdateParticleSystem(
		Vector3(m_pos, 0.0f),
		Vector2(0.0f, 0.0f),
		Vector2(0.0f, 0.0f),
		m_angle,
		static_cast<float>(lastFrameElapsedTimeMS));

	if (m_shaderManager->BeginParticlePass(*m_particleManager->GetSystem()))
	{
		m_particleManager->DrawParticleSystem(
			Vector3(1.0f, 1.0f, 1.0f),
			1.0f,
			0.0f,
			ETHParticleManager::LAYERABLE,
			Vector2(0.0f, 0.0f),
			0.0f,
			0.0f);
		m_shaderManager->EndParticlePass();
	}
	return true;
}

bool ETHParticleDrawer::IsAlive() const
{
	if (m_particleManager->GetSystem()->repeat <= 0)
	{
		m_provider->Log(m_fileName + (" rendering failed. Only temporary particle effects are allowed"), Platform::Logger::LT_ERROR);
		return false;
	}
	return !m_particleManager->Finished();
}
