#include "ETHEntitySpriteRenderer.h"

ETHEntitySpriteRenderer::ETHEntitySpriteRenderer(
	ETHRenderEntity* entity,
	const ETHShaderManagerPtr& shaderManager,
	const VideoPtr& video) :
	ETHEntityPieceRenderer(entity),
	m_shaderManager(shaderManager),
	m_video(video)
{
}

void ETHEntitySpriteRenderer::Render(const ETHSceneProperties& props)
{
	RenderAmbientPass(props);
}

void ETHEntitySpriteRenderer::RenderAmbientPass(const ETHSceneProperties& props)
{
	m_shaderManager->BeginAmbientPass(m_entity);

	m_entity->DrawAmbientPass(props);

	m_shaderManager->EndAmbientPass();
}
