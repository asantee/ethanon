#ifndef ETH_RAY_CAST_CALLBACK_H_
#define ETH_RAY_CAST_CALLBACK_H_

#include <Box2D/Box2D.h>
#include "../Entity/ETHEntityArray.h"
#include "../Entity/ETHEntityChooser.h"
#include <Math/GameMath.h>

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
