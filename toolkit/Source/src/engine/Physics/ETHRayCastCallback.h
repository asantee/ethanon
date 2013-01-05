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

#ifndef ETH_RAY_CAST_CALLBACK_H_
#define ETH_RAY_CAST_CALLBACK_H_

#include <Box2D/Box2D.h>
#include "../Entity/ETHEntityArray.h"
#include "../Entity/ETHEntityChooser.h"
#include <Math/GameMath.h>
#include <map>

using namespace gs2d::math;
using namespace gs2d;

class ETHRayCastCallback : public b2RayCastCallback
{
	struct Contact
	{
		Contact(ETHEntity* ent, const Vector2& con, const Vector2& nor);
		ETHEntity* entity;
		Vector2 point, normal;
	};

	std::multimap<float, Contact> m_contacts;
	b2Vec2 m_a, m_b;
	const ETHEntityChooser* m_chooser;

public:
	ETHRayCastCallback(const Vector2& a, const Vector2& b, const ETHEntityChooser* chooser);
	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);
	ETHEntity* GetClosestContact(Vector2& point, Vector2& normal);
	bool GetContactEntities(ETHEntityArray& entities);
	b2Vec2 GetScaledA() const;
	b2Vec2 GetScaledB() const;
};

#endif
