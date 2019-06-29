#include "ETHEntityHaloRenderer.h"

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
	GS2D_UNUSED_ARGUMENT(minHeight);
	GS2D_UNUSED_ARGUMENT(maxHeight);
	if (m_shaderManager->BeginHaloPass(m_entity->GetLight()))
	{
		m_entity->DrawHalo(props.zAxisDirection, m_depth);
		m_shaderManager->EndHaloPass();
	}
}
