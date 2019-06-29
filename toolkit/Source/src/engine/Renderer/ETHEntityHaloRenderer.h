#ifndef ETH_ENTITY_HALO_RENDERER_H_
#define ETH_ENTITY_HALO_RENDERER_H_

#include "ETHEntityPieceRenderer.h"

#include "../Shader/ETHShaderManager.h"

class ETHEntityHaloRenderer : public ETHEntityPieceRenderer
{
	ETHShaderManagerPtr m_shaderManager;
	float m_depth;

public:
	ETHEntityHaloRenderer(
		ETHRenderEntity* entity,
		const ETHShaderManagerPtr& shaderManager,
		const float depth);

	void Render(const ETHSceneProperties& props, const float maxHeight, const float minHeight);
};

#endif
