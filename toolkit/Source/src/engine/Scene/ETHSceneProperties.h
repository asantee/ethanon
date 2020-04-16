#ifndef ETH_SCENE_PROPERTIES_H_
#define ETH_SCENE_PROPERTIES_H_

#include <Math/Vector3.h>

#include "../ETHTypes.h"

struct ETHSceneProperties
{
	ETHSceneProperties();
	bool ReadFromXMLFile(TiXmlElement *pElement);
	bool WriteToXMLFile(TiXmlElement *pRoot) const;
	void Reset();

	Vector3 ambient;
	Vector2 zAxisDirection;
	float parallaxIntensity;
};

#endif
