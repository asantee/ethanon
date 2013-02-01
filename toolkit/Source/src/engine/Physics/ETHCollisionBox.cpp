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

#include "ETHCollisionBox.h"
#include "ETHEntityProperties.h"

ETHCollisionBox::ETHCollisionBox()
{
	pos = size = Vector3(0,0,0);
}

ETHCollisionBox::ETHCollisionBox(const Vector3 &v3Pos, const Vector3 &v3Size)
{
	pos = v3Pos;
	size = v3Size;
}

bool ETHCollisionBox::ReadFromXMLFile(TiXmlElement *pElement)
{
	ETHEntityProperties::ReadVector3PropertyFromXmlElement(pElement, GS_L("Position"), pos);
	ETHEntityProperties::ReadVector3PropertyFromXmlElement(pElement, GS_L("Size"), size);
	return true;
}

TiXmlElement* ETHCollisionBox::WriteToXMLFile(TiXmlElement *pRoot) const
{
	TiXmlElement *pCollisionRoot = new TiXmlElement(GS_L("Collision"));
	pRoot->LinkEndChild(pCollisionRoot); 

	ETHEntityProperties::SetVector3PropertyToXmlElement(pCollisionRoot, GS_L("Position"), pos);
	ETHEntityProperties::SetVector3PropertyToXmlElement(pCollisionRoot, GS_L("Size"), size);
	return pCollisionRoot;
}

ETHCollisionInScene::ETHCollisionInScene()
{
	pos = size = Vector3(0,0,0);
	ownerID = 0;
}

ETHCollisionInScene::ETHCollisionInScene(const Vector3 &v3Pos, const Vector3 &v3Size, const int id)
{
	ownerID = id;
	pos = v3Pos;
	size = v3Size;
}

ETHCollisionInScene::ETHCollisionInScene(const ETHCollisionBox &box, const int id)
{
	ownerID = id;
	pos = box.pos;
	size = box.size;
}

bool ETHCollisionBox::Collide(const ETHCollisionBox &other, const Vector3 &entityPos0, const Vector3 &entityPos1) const
{
	// TODO: more robust collision implementation for line intersection, rotation and sliding
	const Vector3 halfSize0 = size/2;
	const Vector3 halfSize1 = other.size/2;

	const Vector3 v3Min0 = pos+entityPos0-halfSize0;
	const Vector3 v3Max0 = pos+entityPos0+halfSize0;

	const Vector3 v3Min1 = other.pos+entityPos1-halfSize1;
	const Vector3 v3Max1 = other.pos+entityPos1+halfSize1;

	if (v3Min0.x > v3Max1.x)
		return false;
	if (v3Min0.y > v3Max1.y)
		return false;

	if (v3Max0.x < v3Min1.x)
		return false;
	if (v3Max0.y < v3Min1.y)
		return false;

	// peform the z-axis comparision at the end because it's less likely to happen
	if (v3Max0.z < v3Min1.z)
		return false;
	if (v3Min0.z > v3Max1.z)
		return false;

	return true;
}

void ETHCollisionBox::Scale(const Vector2& scale)
{
	pos.x *= scale.x;
	pos.y *= scale.y;
	size.x *= scale.x;
	size.y *= scale.y;
}

