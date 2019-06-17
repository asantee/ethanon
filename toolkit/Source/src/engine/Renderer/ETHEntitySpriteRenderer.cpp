#include "ETHEntitySpriteRenderer.h"

ETHEntitySpriteRenderer::ETHEntitySpriteRenderer(
	ETHRenderEntity* entity,
	const ETHShaderManagerPtr& shaderManager,
	const VideoPtr& video,
	const bool lightmapEnabled,
	const bool realTimeShadowsEnabled,
	std::list<ETHLight>* lights) :
	ETHEntityPieceRenderer(entity),
	m_shaderManager(shaderManager),
	m_video(video),
	m_lightmapEnabled(lightmapEnabled),
	m_realTimeShadowsEnabled(realTimeShadowsEnabled),
	m_lights(lights)
{
}

void ETHEntitySpriteRenderer::Render(const ETHSceneProperties& props, const float maxHeight, const float minHeight)
{
	RenderAmbientPass(props, maxHeight, minHeight);
	RenderLightPass(props, maxHeight, minHeight);
}

void ETHEntitySpriteRenderer::RenderAmbientPass(const ETHSceneProperties& props, const float maxHeight, const float minHeight)
{
	m_shaderManager->BeginAmbientPass(m_entity, maxHeight, minHeight);

	m_entity->DrawAmbientPass(
		maxHeight,
		minHeight,
		m_lightmapEnabled,
		props,
		m_shaderManager->GetParallaxIntensity());

	m_shaderManager->EndAmbientPass();
}

void ETHEntitySpriteRenderer::RenderLightPass(const ETHSceneProperties& props, const float maxHeight, const float minHeight)
{
	if (m_shaderManager->IsRichLightingEnabled())
	{
		for (std::list<ETHLight>::iterator iter = m_lights->begin(); iter != m_lights->end(); ++iter)
		{
			iter->SetLightScissor(m_video, props.zAxisDirection);
			if (!m_entity->IsHidden())
			{
				if (!(m_entity->IsStatic() && iter->staticLight && m_lightmapEnabled))
				{
					// light pass
					if (m_shaderManager->BeginLightPass(m_entity, &(*iter), maxHeight, minHeight, props.lightIntensity))
					{
						m_entity->DrawLightPass(props.zAxisDirection, m_shaderManager->GetParallaxIntensity());
						m_shaderManager->EndLightPass();
					}
				}
			}
			m_video->UnsetScissor();
		}
	}
}
