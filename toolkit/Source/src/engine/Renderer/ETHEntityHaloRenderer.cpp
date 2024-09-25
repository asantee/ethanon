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

void ETHEntityHaloRenderer::Render(const ETHSceneProperties& props)
{
	if (m_shaderManager->BeginHaloPass(m_entity->GetLight()))
	{
		m_entity->DrawHalo(props.zAxisDirection, m_depth, props);
		m_shaderManager->EndHaloPass();
	}
}
