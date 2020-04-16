#ifndef ETH_LIGHT_H_
#define ETH_LIGHT_H_

#include <Math/Vector3.h>

#include "../ETHTypes.h"

struct ETHLight
{
	ETHLight(const bool active);
	bool ReadFromXMLFile(TiXmlElement *pElement);
	bool WriteToXMLFile(TiXmlElement *pRoot) const;
	bool IsActive() const;
	ETH_BOOL staticLight;
	Vector3 pos;
	Vector3 color;
	float range;
	float haloSize;
	float haloBrightness;
	ETH_BOOL castShadows;
	std::string haloBitmap;
private:
	ETH_BOOL active;
};

#endif
