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

#ifndef ETH_RENDER_ENTITY_H_
#define ETH_RENDER_ENTITY_H_

#include "ETHSpriteEntity.h"

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
		const str_type::string &entityPath);
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

	bool DrawShadow(
		const float maxHeight,
		const float minHeight,
		const ETHSceneProperties& sceneProps,
		const ETHLight& light,
		ETHSpriteEntity *pParent,
		const bool maxOpacity = false,
		const bool drawToTarget = false,
		const float targetAngle = 0.0f,
		const Vector3& v3TargetPos = Vector3(0,0,0));

	bool DrawProjShadow(
		const float maxHeight,
		const float minHeight,
		const ETHSceneProperties& sceneProps,
		const ETHLight& light,
		ETHSpriteEntity *pParent,
		const bool maxOpacity,
		const bool drawToTarget,
		const float targetAngle,
		const Vector3& v3TargetPos);
};

#endif