#ifndef ETH_LIGHTING_PROFILE_H_
#define ETH_LIGHTING_PROFILE_H_

#include "../Entity/ETHSpriteEntity.h"

class ETHLightingProfile
{
public:
	virtual bool BeginLightPass(ETHSpriteEntity *pRender, Vector3 &v3LightPos, const Vector2 &v2Size,
		const ETHLight* light, const float maxHeight, const float minHeight, const float lightIntensity,
		const bool drawToTarget) = 0;
	virtual bool EndLightPass() = 0;

	virtual SpritePtr GetDefaultNormalMap() = 0;
};

typedef boost::shared_ptr<ETHLightingProfile> ETHLightingProfilePtr;

#endif
