#ifndef ETH_PIXEL_LIGHT_DIFFUSE_SPECULAR_H_
#define ETH_PIXEL_LIGHT_DIFFUSE_SPECULAR_H_

#include "ETHLightingProfile.h"
#include "ETHFakeEyePositionManager.h"

class ETHPixelLightDiffuseSpecular : public ETHLightingProfile
{
public:
	ETHPixelLightDiffuseSpecular(
		VideoPtr video,
		const str_type::string& shaderPath,
		ETHFakeEyePositionManagerPtr fakeEyeManager);

	bool BeginLightPass(ETHSpriteEntity *pRender, Vector3 &v3LightPos, const Vector2 &v2Size,
		const ETHLight* light, const float maxHeight, const float minHeight, const float lightIntensity,
		const bool drawToTarget = false);
	bool EndLightPass();

	SpritePtr GetDefaultNormalMap();

private:
	VideoPtr m_video;
	SpritePtr m_defaultNM;
	ETHFakeEyePositionManagerPtr m_fakeEyeManager;
	Video::ALPHA_MODE m_lastAM;
	ShaderPtr m_pixelLightShader;
	ShaderPtr m_pixelLightSpecularShader;

	static const float LIGHT_PRECISION_DOWNSCALE;
};

#endif
