/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#ifndef ETH_COLLISION_BOX_H_
#define ETH_COLLISION_BOX_H_

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