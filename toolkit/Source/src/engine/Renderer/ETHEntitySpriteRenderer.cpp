/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

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

					// shadow pass
					if (m_realTimeShadowsEnabled)
					{
						const bool roundUp = m_video->IsRoundingUpPosition();
						if (m_entity->GetProperties()->castShadow)
						{
							m_video->RoundUpPosition(false);
							m_video->SetScissor(false);
							if (m_shaderManager->BeginShadowPass(m_entity, &(*iter), maxHeight, minHeight))
							{
								m_entity->DrawShadow(maxHeight, minHeight, props, *iter, 0);
								m_shaderManager->EndShadowPass();
							}
							m_video->SetScissor(true);
						}
						m_video->RoundUpPosition(roundUp);
					}
				}
			}
			m_video->UnsetScissor();
		}
	}
}
