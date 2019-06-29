#ifndef ETH_ENTITY_RENDERING_MANAGER_H_
#define ETH_ENTITY_RENDERING_MANAGER_H_

#include "ETHEntityPieceRenderer.h"

#include "../Resource/ETHResourceProvider.h"

#include "../Scene/ETHSceneProperties.h"

class ETHEntityRenderingManager
{
	std::multimap<float, ETHEntityPieceRendererPtr> m_piecesToRender;
	ETHResourceProviderPtr m_provider;
	std::list<ETHLight> m_lights;

public:

	static ETHLight BuildChildLight(const ETHLight &light, const Vector3& parentPos, const Vector2& scale);

	ETHEntityRenderingManager(ETHResourceProviderPtr provider);

	void RenderPieces(const ETHSceneProperties& props, const float minHeight, const float maxHeight);

	bool IsEmpty() const;

	void ReleaseMappedPieces();

	float ComputeDrawHash(VideoPtr video, const float entityDepth, const ETHSpriteEntity* entity) const;

	std::size_t GetNumLights() const;

	void AddLight(const ETHLight &light, const ETHSceneProperties& props);

	void AddDecomposedPieces(
		ETHRenderEntity* entity,
		const float minHeight,
		const float maxHeight,
		const ETHBackBufferTargetManagerPtr& backBuffer,
		const ETHSceneProperties& props,
		unsigned int& piecesAddedThisTime);
};

#endif
