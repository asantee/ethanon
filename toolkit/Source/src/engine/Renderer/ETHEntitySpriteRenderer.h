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
