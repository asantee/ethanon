#ifndef ETH_SCENE_PROPERTIES_H_
#define ETH_SCENE_PROPERTIES_H_

#include "../ETHTypes.h"

struct ETHSceneProperties
{
	ETHSceneProperties();
	bool ReadFromXMLFile(TiXmlElement *pElement);
	bool WriteToXMLFile(TiXmlElement *pRoot) const;
	void Reset();

	Vector3 ambient;
	float lightIntensity;
	Vector2 zAxisDirection;
	float parallaxIntensity;
};

#endif
