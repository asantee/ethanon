/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

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

public:
	ETHRenderEntity(const str_type::string& filePath, ETHResourceProviderPtr provider, const int nId =-1);
	ETHRenderEntity(TiXmlElement *pElement, ETHResourceProviderPtr provider);
	ETHRenderEntity(ETHResourceProviderPtr provider, const ETHEntityProperties& properties, const float angle, const float scale);
	ETHRenderEntity(ETHResourceProviderPtr provider);

	// rendering methods
	bool IsSpriteVisible() const;
	bool DrawLightPass(const Vector2 &zAxisDirection, const bool drawToTarget = false);
	bool DrawHalo(const float maxHeight, const float minHeight, const bool rotateHalo, const Vector2& zAxisDirection);
	bool DrawAmbientPass(const float maxHeight, const float minHeight, const bool enableLightmaps, const ETHSceneProperties& sceneProps);
	bool DrawParticles(const unsigned int n, const float maxHeight, const float minHeight, const ETHSceneProperties& sceneProps);
	void DrawCollisionBox(const bool drawBase, SpritePtr pOutline, const GS_COLOR& dwColor, const Vector2 &zAxisDirection) const;
	bool DrawShadow(const float maxHeight, const float minHeight, const ETHSceneProperties& sceneProps, const ETHLight& light, ETHSpriteEntity *pParent,
					const bool maxOpacity = false, const bool drawToTarget = false, const float targetAngle = 0.0f,
					const Vector3& v3TargetPos = Vector3(0,0,0));
	bool DrawProjShadow(const float maxHeight, const float minHeight, const ETHSceneProperties& sceneProps, const ETHLight& light,
						 ETHSpriteEntity *pParent, const bool maxOpacity, const bool drawToTarget,
						 const float targetAngle, const Vector3& v3TargetPos);
};

#endif