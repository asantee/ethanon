#ifndef ETH_LIGHTMAP_GEN_H_
#define ETH_LIGHTMAP_GEN_H_

#include "../Entity/ETHRenderEntity.h"

class ETHLightmapGen
{
public:
	ETHLightmapGen(
		ETHRenderEntity* entity,
		boost::shared_ptr<ETHShaderManager> shaderManager,
		std::list<ETHLight>::iterator iBegin,
		std::list<ETHLight>::iterator iEnd,
		ETHBucketManager& buckets,
		const Vector3& oldPos,
		const Vector3& newPos,
		const float minHeight,
		const float maxHeight,
		const ETHSceneProperties &sceneProps);
};

#endif
