#ifndef ETH_PARTICLE_DRAWER_H_
#define ETH_PARTICLE_DRAWER_H_

#include "ETHDrawable.h"
#include "../Particles/ETHParticleManager.h"
#include "../Shader/ETHShaderManager.h"

class ETHParticleDrawer : public ETHDrawable
{
public:
	ETHParticleDrawer(
		const ETHResourceProviderPtr& provider,
		ETHGraphicResourceManagerPtr graphicResources,
		ETHShaderManagerPtr shaderManager,
		const std::string& resourceDirectory,
		const std::string& fileName,
		const Vector2& pos,
		const float angle,
		const float scale);

	bool Draw(const unsigned long lastFrameElapsedTimeMS);
	bool IsAlive() const;

private:
	ETHResourceProviderPtr m_provider;
	ETHParticleManagerPtr m_particleManager;
	ETHShaderManagerPtr m_shaderManager;
	Vector2 m_pos;
	float m_angle;
	std::string m_fileName;
};

#endif
