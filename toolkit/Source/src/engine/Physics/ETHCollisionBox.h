#ifndef ETH_COLLISION_BOX_H_
#define ETH_COLLISION_BOX_H_

#include <Math/Vector3.h>

#include "../ETHTypes.h"

struct ETHCollisionBox
{
	ETHCollisionBox();
	ETHCollisionBox(const Vector3 &v3Pos, const Vector3 &v3Size);
	bool Collide(const ETHCollisionBox &other, const Vector3 &entityPos0, const Vector3 &entityPos1) const;
	bool ReadFromXMLFile(TiXmlElement *pElement);
	TiXmlElement* WriteToXMLFile(TiXmlElement *pRoot) const;
	void Scale(const Vector2& scale);
	Vector3 pos, size;
};

struct ETHCollisionInScene : public ETHCollisionBox
{
	ETHCollisionInScene();
	ETHCollisionInScene(const Vector3 &v3Pos, const Vector3 &v3Size, const int id);
	ETHCollisionInScene(const ETHCollisionBox &box, const int id);
	int ownerID;
};



#endif
