#include "ETHEntityParticleRenderer.h"

ETHEntityParticleRenderer::ETHEntityParticleRenderer(
	ETHRenderEntity* entity,
	const ETHShaderManagerPtr& shaderManager,
	std::size_t particleIndex) :
	ETHEntityPieceRenderer(entity),
	m_shaderManager(shaderManager),
	m_particleIndex(particleIndex)
{
}

void ETHEntityParticleRenderer::Render(const ETHSceneProperties& props, const float maxHeight, const float minHeight)
{
	if (m_shaderManager->BeginParticlePass(*m_entity->GetParticleManager(m_particleIndex)->GetSystem()))
	{
		m_entity->DrawParticles(m_particleIndex, maxHeight, minHeight, props);
		m_shaderManager->EndParticlePass();
	}
}
