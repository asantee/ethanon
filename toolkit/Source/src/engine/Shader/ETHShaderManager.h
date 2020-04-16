#ifndef ETH_SHADER_MANAGER_H_
#define ETH_SHADER_MANAGER_H_

#include "../ETHTypes.h"

#include "../Entity/ETHSpriteEntity.h"

class ETHShaderManager
{
public:
	ETHShaderManager(VideoPtr video);

	bool BeginAmbientPass(const ETHSpriteEntity* pRender, const float maxHeight, const float minHeight);
	bool EndAmbientPass();

	bool BeginHaloPass(const ETHLight* light);
	bool EndHaloPass();

	bool BeginParticlePass(const ETHParticleSystem& system);
	bool EndParticlePass();

private:

	VideoPtr m_video;
	Video::ALPHA_MODE m_lastAM;
};

typedef boost::shared_ptr<ETHShaderManager> ETHShaderManagerPtr;

#endif
