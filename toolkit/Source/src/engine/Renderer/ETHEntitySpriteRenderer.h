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

#ifndef ETH_ENTITY_SPRITE_RENDERER_H_
#define ETH_ENTITY_SPRITE_RENDERER_H_

#include "ETHEntityPieceRenderer.h"

#include "../Shader/ETHShaderManager.h"

class ETHEntitySpriteRenderer : public ETHEntityPieceRenderer
{
	ETHShaderManagerPtr m_shaderManager;
	VideoPtr m_video;
	bool m_lightmapEnabled;
	bool m_realTimeShadowsEnabled;
	std::list<ETHLight>* m_lights;

	void RenderAmbientPass(const ETHSceneProperties& props, const float maxHeight, const float minHeight);
	void RenderLightPass(const ETHSceneProperties& props, const float maxHeight, const float minHeight);

public:
	ETHEntitySpriteRenderer(
		ETHRenderEntity* entity,
		const ETHShaderManagerPtr& shaderManager,
		const VideoPtr& video,
		const bool lightmapEnabled,
		const bool realTimeShadowsEnabled,
		std::list<ETHLight>* lights);

	void Render(const ETHSceneProperties& props, const float maxHeight, const float minHeight);
};

#endif
