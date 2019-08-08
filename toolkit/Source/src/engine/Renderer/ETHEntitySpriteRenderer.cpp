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

void ETHEntitySpriteRenderer::Render(const ETHSceneProperties& props, const float maxHeight, const float minHeight)
{
	RenderAmbientPass(props, maxHeight, minHeight);
}

void ETHEntitySpriteRenderer::RenderAmbientPass(const ETHSceneProperties& props, const float maxHeight, const float minHeight)
{
	m_shaderManager->BeginAmbientPass(m_entity, maxHeight, minHeight);

	m_entity->DrawAmbientPass(
		maxHeight,
		minHeight,
		props);

	m_shaderManager->EndAmbientPass();
}
