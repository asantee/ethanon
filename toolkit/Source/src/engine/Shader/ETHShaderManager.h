#ifndef ETH_SHADER_MANAGER_H_
#define ETH_SHADER_MANAGER_H_

#include "../ETHTypes.h"

#include "../Entity/ETHSpriteEntity.h"

class ETHShaderManager
{
public:
	ETHShaderManager(VideoPtr video, const std::string& shaderPath);

	bool BeginAmbientPass(const ETHSpriteEntity* pRender, const float maxHeight, const float minHeight);
	bool EndAmbientPass();

	bool BeginHaloPass(const ETHLight* light);
	bool EndHaloPass();

	bool BeginParticlePass(const ETHParticleSystem& system);
	bool EndParticlePass();

	void SetParallaxIntensity(const float intensity);
	float GetParallaxIntensity() const;
	Vector2 ComputeParallaxOffset(const VideoPtr& video, const Vector3& pos, const float& individualParallaxIntensity) const;

	SpritePtr GetOpaqueSprite();

private:

	float m_parallaxIntensity;

	VideoPtr m_video;
	Video::ALPHA_MODE m_lastAM;

	SpritePtr m_opaqueSprite;
};

typedef boost::shared_ptr<ETHShaderManager> ETHShaderManagerPtr;

#endif
