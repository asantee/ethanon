#ifndef ETH_ENTITY_RENDERING_MANAGER_H_
#define ETH_ENTITY_RENDERING_MANAGER_H_

#include "ETHEntityPieceRenderer.h"

#include "../Resource/ETHResourceProvider.h"

#include "../Scene/ETHSceneProperties.h"

class ETHEntityRenderingManager
{
	std::multimap<float, ETHEntityPieceRendererPtr> m_piecesToRender;
	ETHResourceProviderPtr m_provider;

public:

	ETHEntityRenderingManager(ETHResourceProviderPtr provider);

	void RenderAndReleasePieces(const ETHSceneProperties& props);

	bool IsEmpty() const;

	void ReleaseMappedPieces();

	void AddDecomposedPieces(
		ETHRenderEntity* entity,
		const ETHBackBufferTargetManagerPtr& backBuffer,
		const ETHSceneProperties& props,
		unsigned int& piecesAddedThisTime);
};

#endif
