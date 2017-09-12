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

	bool BeginShadowPass(const ETHSpriteEntity* pRender, const ETHLight* light, const float maxHeight, const float minHeight);
	bool EndShadowPass();

	bool BeginHaloPass(const ETHLight* light);
	bool EndHaloPass();

	bool BeginParticlePass(const ETHParticleSystem& system);
	bool EndParticlePass();

	SpritePtr GetProjShadow();

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
	SpritePtr m_projShadow, m_opaqueSprite;
	ShaderPtr m_shadowVS;
	ShaderPtr m_defaultVS;
	ShaderPtr m_highlightPS;
	ShaderPtr m_solidColorPS;
	ShaderPtr m_particle;
	ShaderPtr m_defaultStaticAmbientVS;
	ShaderPtr m_verticalStaticAmbientVS;
	bool m_richLighting;
};

typedef boost::shared_ptr<ETHShaderManager> ETHShaderManagerPtr;

#endif
