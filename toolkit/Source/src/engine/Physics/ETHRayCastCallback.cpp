#include "ETHRayCastCallback.h"
#include "ETHPhysicsSimulator.h"

#define GS2D_UNUSED_ARGUMENT(argument) ((void)(argument))

ETHRayCastCallback::Contact::Contact(ETHEntity* ent, const Vector2& con, const Vector2& nor)
{
	entity  = (ent);
	point = (con);
	normal  = (nor);
}

ETHRayCastCallback::ETHRayCastCallback(const Vector2& a, const Vector2& b, const ETHEntityChooser* chooser) :
	m_chooser(chooser)
{
	m_a = ETHPhysicsSimulator::ScaleToBox2D(a);
	m_b = ETHPhysicsSimulator::ScaleToBox2D(b);
}

float32 ETHRayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
{
	UNUSED_ARGUMENT(fraction);
	const Vector2 v2Point(point.x, point.y);
	const Vector2 v2Normal(normal.x, normal.y);
	ETHEntity* entity = static_cast<ETHEntity*>(fixture->GetBody()->GetUserData());
	if (m_chooser->Choose(entity))
	{
		m_contacts.insert(std::pair<float, Contact>(Vector2::SquaredDistance(Vector2(m_a.x, m_a.y), v2Point), Contact(entity, ETHPhysicsSimulator::ScaleFromBox2D(point), v2Normal)));
	}
	return 1.0f;
}

ETHEntity* ETHRayCastCallback::GetClosestContact(Vector2& point, Vector2& normal)
{
	std::multimap<float, Contact>::iterator iter = m_contacts.begin();
	if (iter != m_contacts.end())
	{
		Contact& contact = (*iter).second;
		point = contact.point;
		normal = contact.normal;
		contact.entity->AddRef();
		return contact.entity;
	}
	else
	{
		return 0;
	}
}

bool ETHRayCastCallback::GetContactEntities(ETHEntityArray& entities)
{
	for (std::multimap<float, Contact>::iterator iter = m_contacts.begin(); iter != m_contacts.end(); ++iter)
	{
		entities.push_back((*iter).second.entity);
	}
	return (!m_contacts.empty());
}

b2Vec2 ETHRayCastCallback::GetScaledA() const
{
	return m_a;
}

b2Vec2 ETHRayCastCallback::GetScaledB() const
{
	return m_b;
}
