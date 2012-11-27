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

	unsigned int GetNumLights() const;

	void AddLight(const ETHLight &light, const ETHSceneProperties& props);

	void AddDecomposedPieces(
		ETHRenderEntity* entity,
		const float minHeight,
		const float maxHeight,
		const ETHBackBufferTargetManagerPtr& backBuffer,
		const ETHSceneProperties& props);
};

#endif
