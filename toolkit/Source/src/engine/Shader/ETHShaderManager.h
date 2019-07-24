#ifndef ETH_SHADER_MANAGER_H_
#define ETH_SHADER_MANAGER_H_

#include "../ETHTypes.h"

#include "../Entity/ETHSpriteEntity.h"

#include "ETHParallaxManager.h"

class ETHShaderManager
{
public:
	enum PROFILE
	{
		PIXEL_LIGHTING_DIFFUSE_SPECULAR = 1
	};

	ETHShaderManager(VideoPtr video, const str_type::string& shaderPath, const bool richLighting);

	bool BeginAmbientPass(const ETHSpriteEntity* pRender, const float maxHeight, const float minHeight);
	bool EndAmbientPass();

	bool BeginHaloPass(const ETHLight* light);
	bool EndHaloPass();

	bool BeginParticlePass(const ETHParticleSystem& system);
	bool EndParticlePass();

	inline void SetParallaxNormalizedOrigin(const Vector2& normalizedOrigin) { m_parallaxManager.SetNormalizedOrigin(normalizedOrigin); }
	inline Vector2 GetParallaxNormalizedOrigin() const { return m_parallaxManager.GetNormalizedOrigin(); }
	inline void SetParallaxIntensity(const float intensity) { m_parallaxManager.SetIntensity(intensity); }
	inline float GetParallaxIntensity() const { return m_parallaxManager.GetIntensity(); }
	inline Vector2 ComputeParallaxOffset(const VideoPtr& video, const Vector3& pos, const float& individualParallaxIntensity) const { return m_parallaxManager.ComputeOffset(video, pos, individualParallaxIntensity); }
	inline void SetParallaxVerticalIntensity(const float intensity) { m_parallaxManager.SetVerticalIntensity(intensity); }
	inline float GetParallaxVerticalIntensity() const { return m_parallaxManager.GetVerticalIntensity(); }

	SpritePtr GetOpaqueSprite();

private:

	ETHParallaxManager m_parallaxManager;

	VideoPtr m_video;
	Video::ALPHA_MODE m_lastAM;

	SpritePtr m_opaqueSprite;

	ShaderPtr m_defaultShader;
	ShaderPtr m_highlightShader;
	ShaderPtr m_solidColorShader;
	ShaderPtr m_particleShader;
	ShaderPtr m_particleHighlightShader;
	ShaderPtr m_ambientShader;
	ShaderPtr m_ambientAddShader;
};

typedef boost::shared_ptr<ETHShaderManager> ETHShaderManagerPtr;

#endif
