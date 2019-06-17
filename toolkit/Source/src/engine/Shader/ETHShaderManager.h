#ifndef ETH_SHADER_MANAGER_H_
#define ETH_SHADER_MANAGER_H_

#include "../ETHTypes.h"
#include "../Entity/ETHSpriteEntity.h"
#include "ETHParallaxManager.h"
#include "ETHLightingProfile.h"
#include "ETHFakeEyePositionManager.h"

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

	bool BeginLightPass(
		ETHSpriteEntity* pRender,
		const ETHLight* light,
		const float maxHeight,
		const float minHeight,
		const float lightIntensity,
		const ETHSpriteEntity* pParent = 0,
		const bool drawToTarget = false);

	bool EndLightPass();

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
	inline bool IsRichLightingEnabled() const { return m_richLighting; }

	SpritePtr GetOpaqueSprite();

private:

	ETHLightingProfilePtr FindHighestLightingProfile();

	ETHParallaxManager m_parallaxManager;
	ETHFakeEyePositionManagerPtr m_fakeEyeManager;

	std::map<PROFILE, ETHLightingProfilePtr> m_lightingProfiles;
	ETHLightingProfilePtr m_currentProfile;
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

	bool m_richLighting;
};

typedef boost::shared_ptr<ETHShaderManager> ETHShaderManagerPtr;

#endif
