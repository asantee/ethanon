#include "ETHEntityHaloRenderer.h"

#define UNUSED_ARGUMENT(argument) ((void)(argument))

ETHEntityHaloRenderer::ETHEntityHaloRenderer(
	ETHRenderEntity* entity,
	const ETHShaderManagerPtr& shaderManager,
	const float depth) :
	ETHEntityPieceRenderer(entity),
	m_shaderManager(shaderManager),
	m_depth(depth)
{
}

void ETHEntityHaloRenderer::Render(const ETHSceneProperties& props, const float maxHeight, const float minHeight)
{
	UNUSED_ARGUMENT(minHeight);
	UNUSED_ARGUMENT(maxHeight);
	if (m_shaderManager->BeginHaloPass(m_entity->GetLight()))
	{
		m_entity->DrawHalo(props.zAxisDirection, m_depth);
		m_shaderManager->EndHaloPass();
	}
}
