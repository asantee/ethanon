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

#include "ETHEntityRenderingManager.h"

#include "../Renderer/ETHEntitySpriteRenderer.h"
#include "../Renderer/ETHEntityParticleRenderer.h"
#include "../Renderer/ETHEntityHaloRenderer.h"

ETHEntityRenderingManager::ETHEntityRenderingManager(ETHResourceProviderPtr provider) :
	m_provider(provider)
{
}

void ETHEntityRenderingManager::RenderPieces(const ETHSceneProperties& props, const float minHeight, const float maxHeight)
{
	// Draw visible entities ordered in an alpha-friendly map
	for (std::multimap<float, ETHEntityPieceRendererPtr>::iterator iter = m_piecesToRender.begin(); iter != m_piecesToRender.end(); ++iter)
	{
		iter->second->Render(props, maxHeight, minHeight);
	}
	ReleaseMappedPieces();
	m_lights.clear();
}

void ETHEntityRenderingManager::AddDecomposedPieces(
	ETHRenderEntity* entity,
	const float minHeight,
	const float maxHeight,
	const ETHBackBufferTargetManagerPtr& backBuffer,
	const ETHSceneProperties& props)
{
	const VideoPtr& video = m_provider->GetVideo();
	const ETHShaderManagerPtr& shaderManager = m_provider->GetShaderManager();

	const bool spriteVisible = entity->IsSpriteVisible(props, backBuffer);
	
	// decompose entity sprite
	if (spriteVisible)
	{
		ETHEntityPieceRendererPtr spritePiece(
			new ETHEntitySpriteRenderer(
				entity,
				shaderManager,
				video,
				m_provider->AreLightmapsEnabled(),
				m_provider->AreRealTimeShadowsEnabled(),
				&m_lights));

		// add this entity to the multimap to sort it for an alpha-friendly rendering list
		const float depth = entity->ComputeDepth(maxHeight, minHeight);
		const float drawHash = ComputeDrawHash(video, depth, entity);

		// add the entity to the render map
		m_piecesToRender.insert(std::pair<float, ETHEntityPieceRendererPtr>(drawHash, spritePiece));
	}

	// decompose halo
	if (entity->HasLightSource() && entity->GetHalo())
	{
		const float haloZ = entity->GetPositionZ() + ((entity->GetType() == ETHEntityProperties::ET_VERTICAL) ? entity->GetCurrentSize().y : 0.0f);
		const float depth = ETHEntity::ComputeDepth(haloZ, maxHeight, minHeight);
		const float drawHash = ComputeDrawHash(video, depth, entity);

		ETHEntityPieceRendererPtr haloPiece(new ETHEntityHaloRenderer(entity, shaderManager, depth));
		m_piecesToRender.insert(std::pair<float, ETHEntityPieceRendererPtr>(drawHash, haloPiece));
	}

	// decompose the particle list for this entity
	if (entity->HasParticleSystems())
	{
		for (std::size_t t = 0; t < entity->GetNumParticleSystems(); t++)
		{
			ETHEntityPieceRendererPtr particlePiece(
				new ETHEntityParticleRenderer(entity, shaderManager, t));

			ETHParticleManagerPtr particle = entity->GetParticleManager(t);

			const float shift = ETHParticleManager::GetParticleDepthShift(ETHEntityProperties::ResolveDepthSortingMode(entity->GetType()));
			const float depth = ETHEntity::ComputeDepth(
				particle->GetZPosition() + entity->GetPositionZ() + shift,
				maxHeight,
				minHeight);

			const float drawHash = ComputeDrawHash(video, depth, entity);

			m_piecesToRender.insert(std::pair<float, ETHEntityPieceRendererPtr>(drawHash, particlePiece));
		}
	}

	// fill the light list for this frame
	if (entity->HasLightSource() && m_provider->IsRichLightingEnabled())
	{
		ETHLight light = *(entity->GetLight());
		light.color *= entity->ComputeLightIntensity();
		AddLight(BuildChildLight(light, entity->GetPosition(), entity->GetScale()), props);
	}
}

float ETHEntityRenderingManager::ComputeDrawHash(VideoPtr video, const float entityDepth, const ETHSpriteEntity* entity) const
{
	static const float precisionScale = 100.0f;
	float drawHash;
	float verticalHashShift;
	const float screenHeight = video->GetScreenSize().y * precisionScale;
	const float hashDepth = entityDepth * screenHeight;

	switch (entity->GetType())
	{
	case ETHEntityProperties::ET_HORIZONTAL:
		drawHash = hashDepth;
		break;
	case ETHEntityProperties::ET_VERTICAL:
		verticalHashShift = ((entity->GetPositionY() - video->GetCameraPos().y) * precisionScale) / screenHeight;
		drawHash = hashDepth + verticalHashShift + 0.1f;
		break;
	case ETHEntityProperties::ET_GROUND_DECAL:
	case ETHEntityProperties::ET_OPAQUE_DECAL:
		drawHash = hashDepth + 0.1f;
		break;
	default:
		drawHash = hashDepth;
	}
	return drawHash;
}

bool ETHEntityRenderingManager::IsEmpty() const
{
	return m_piecesToRender.empty();
}

void ETHEntityRenderingManager::ReleaseMappedPieces()
{
	m_piecesToRender.clear();
}

std::size_t ETHEntityRenderingManager::GetNumLights() const
{
	return m_lights.size();
}

void ETHEntityRenderingManager::AddLight(const ETHLight &light, const ETHSceneProperties& props)
{
	if (light.color == Vector3(0,0,0))
		return;

	// if the light isn't in screen, don't add it
	if (!light.staticLight)
		if (!ETHGlobal::IsSphereInScreen(light.pos, light.range, props.zAxisDirection, m_provider->GetVideo()))
			return;

	m_lights.push_back(light);
}

ETHLight ETHEntityRenderingManager::BuildChildLight(
	const ETHLight &light,
	const Vector3& parentPos,
	const Vector2& scale)
{
	ETHLight childLight = light;
	childLight.pos *= scale.y;
	childLight.range *= scale.y;
	childLight.pos += parentPos;
	return childLight;
}
