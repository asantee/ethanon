#ifndef ETH_RENDER_ENTITY_H_
#define ETH_RENDER_ENTITY_H_

#include "ETHSpriteEntity.h"

#include "../Shader/ETHBackBufferTargetManager.h"

#include <boost/unordered/unordered_map.hpp>

class ETHRenderEntity : public ETHSpriteEntity
{
public:
	ETHRenderEntity(const std::string& filePath, ETHResourceProviderPtr provider, const int nId =-1);
	ETHRenderEntity(
		TiXmlElement *pElement,
		ETHResourceProviderPtr provider,
		ETHEntityCache& entityCache,
		const std::string &entityPath,
		const bool shouldGenerateNewID);
	ETHRenderEntity(ETHResourceProviderPtr provider, const ETHEntityProperties& properties, const float angle, const float scale);
	ETHRenderEntity(ETHResourceProviderPtr provider);

	// rendering methods
	bool IsSpriteVisible(
		const ETHSceneProperties& sceneProps,
		const ETHBackBufferTargetManagerPtr& backBuffer) const;

	bool DrawHalo(
		const Vector2& zAxisDirection,
		const float depth,
		const ETHSceneProperties& sceneProps);

	bool DrawAmbientPass(
		const float maxHeight,
		const float minHeight,
		const ETHSceneProperties& sceneProps);

	bool DrawParticles(
		const std::size_t n,
		const float maxHeight,
		const float minHeight,
		const ETHSceneProperties& sceneProps);

	void DrawCollisionBox(
		const ETHSceneProperties& sceneProps,
		SpritePtr pOutline,
		const Color& color,
		const Vector2 &zAxisDirection) const;
};

#endif
