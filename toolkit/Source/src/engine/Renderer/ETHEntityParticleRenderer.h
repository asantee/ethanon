#ifndef ETH_ENTITY_PARTICLE_RENDERER_H_
#define ETH_ENTITY_PARTICLE_RENDERER_H_

#include "ETHEntityPieceRenderer.h"

#include "../Shader/ETHShaderManager.h"

class ETHEntityParticleRenderer : public ETHEntityPieceRenderer
{
	ETHShaderManagerPtr m_shaderManager;
	std::size_t m_particleIndex;

public:
	ETHEntityParticleRenderer(
		ETHRenderEntity* entity,
		const ETHShaderManagerPtr& shaderManager,
		std::size_t particleIndex);

	void Render(const ETHSceneProperties& props, const float maxHeight, const float minHeight);
};

#endif
