#include "ETHEntityRenderingManager.h"

#include "../Renderer/ETHEntitySpriteRenderer.h"
#include "../Renderer/ETHEntityParticleRenderer.h"
#include "../Renderer/ETHEntityHaloRenderer.h"

ETHEntityRenderingManager::ETHEntityRenderingManager(ETHResourceProviderPtr provider) :
	m_provider(provider)
{
}

void ETHEntityRenderingManager::RenderAndReleasePieces(const ETHSceneProperties& props, const float minHeight, const float maxHeight)
{
	// Draw visible entities ordered in an alpha-friendly map
	for (std::multimap<float, ETHEntityPieceRendererPtr>::iterator iter = m_piecesToRender.begin(); iter != m_piecesToRender.end(); ++iter)
	{
		iter->second->Render(props, maxHeight, minHeight);
	}
	ReleaseMappedPieces();
}

void ETHEntityRenderingManager::AddDecomposedPieces(
	ETHRenderEntity* entity,
	const float minHeight,
	const float maxHeight,
	const ETHBackBufferTargetManagerPtr& backBuffer,
	const ETHSceneProperties& props,
	unsigned int& piecesAddedThisTime)
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
				video));

		// add this entity to the multimap to sort it for an alpha-friendly rendering list
		const float depth = entity->ComputeDepth(maxHeight, minHeight);
		const float drawHash = ComputeDrawHash(video, depth, entity);

		// add the entity to the render map
		m_piecesToRender.insert(std::pair<float, ETHEntityPieceRendererPtr>(drawHash, spritePiece));
		
		piecesAddedThisTime++;
	}

	// decompose halo
	if (entity->HasLightSource() && entity->GetHalo())
	{
		const float haloZ = entity->GetPositionZ();
		const float depth = ETHEntity::ComputeDepth(haloZ, maxHeight, minHeight);
		const float drawHash = ComputeDrawHash(video, depth, entity);

		ETHEntityPieceRendererPtr haloPiece(new ETHEntityHaloRenderer(entity, shaderManager, depth));
		m_piecesToRender.insert(std::pair<float, ETHEntityPieceRendererPtr>(drawHash, haloPiece));
		
		piecesAddedThisTime++;
	}

	// decompose the particle list for this entity
	for (std::size_t t = 0; t < entity->GetNumParticleSystems(); t++)
	{
		ETHParticleManagerPtr particle = entity->GetParticleManager(t);

		if (!particle->IsVisible(video->GetCameraPos(), video->GetCameraPos() + video->GetScreenSizeF()))
		{
			continue;
		}
	
		ETHEntityPieceRendererPtr particlePiece(
			new ETHEntityParticleRenderer(entity, shaderManager, t));

		const float depth = ETHEntity::ComputeDepth(
			particle->GetZPosition() + entity->GetPositionZ(),
			maxHeight,
			minHeight);

		const float drawHash = ComputeDrawHash(video, depth, entity);

		m_piecesToRender.insert(std::pair<float, ETHEntityPieceRendererPtr>(drawHash, particlePiece));
		
		piecesAddedThisTime++;
	}
}

float ETHEntityRenderingManager::ComputeDrawHash(VideoPtr video, const float entityDepth, const ETHSpriteEntity* entity) const
{
	static const float precisionScale = 100.0f;
	float drawHash;
	const float screenHeight = video->GetScreenSize().y * precisionScale;
	const float hashDepth = entityDepth * screenHeight;

	switch (entity->GetType())
	{
	case ETHEntityProperties::ET_HORIZONTAL:
		drawHash = hashDepth;
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

