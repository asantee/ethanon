#ifndef ETH_RENDER_ENTITY_H_
#define ETH_RENDER_ENTITY_H_

#include "ETHSpriteEntity.h"

#include "../Shader/ETHBackBufferTargetManager.h"

#include <boost/unordered/unordered_map.hpp>

class ETHRenderEntity : public ETHSpriteEntity
{
	friend class ETHLightmapGen;

	bool ShouldUseFourTriangles(const float parallaxIntensity) const;

public:
	ETHRenderEntity(const str_type::string& filePath, ETHResourceProviderPtr provider, const int nId =-1);
	ETHRenderEntity(
		TiXmlElement *pElement,
		ETHResourceProviderPtr provider,
		ETHEntityCache& entityCache,
		const str_type::string &entityPath,
		const bool shouldGenerateNewID);
	ETHRenderEntity(ETHResourceProviderPtr provider, const ETHEntityProperties& properties, const float angle, const float scale);
	ETHRenderEntity(ETHResourceProviderPtr provider);

	// rendering methods
	bool IsSpriteVisible(
		const ETHSceneProperties& sceneProps,
		const ETHBackBufferTargetManagerPtr& backBuffer) const;

	bool DrawLightPass(const Vector2 &zAxisDirection, const float parallaxIntensity, const bool drawToTarget = false);

	bool DrawHalo(
		const Vector2& zAxisDirection,
		const float depth);

	bool DrawAmbientPass(
		const float maxHeight,
		const float minHeight,
		const bool enableLightmaps,
		const ETHSceneProperties& sceneProps,
		const float parallaxIntensity);

	bool DrawParticles(
		const std::size_t n,
		const float maxHeight,
		const float minHeight,
		const ETHSceneProperties& sceneProps);

	void DrawCollisionBox(
		SpritePtr pOutline,
		const Color& dwColor,
		const Vector2 &zAxisDirection) const;
};

#endif
