#include "ETHContactListener.h"

#include "ETHPhysicsSimulator.h"

ETHContactListener::ETHContactListener() :
	m_disableNextContact(false),
	m_runningPreSolveContactCallback(false)
{
}

ETHContactListener::~ETHContactListener()
{
	ClearStackedEndContactCallbacks();
}

static bool GetContactData(
	const b2Contact* contact,
	ETHPhysicsEntityController** controllerA,
	ETHPhysicsEntityController** controllerB,
	ETHEntity** entityA,
	ETHEntity** entityB,
	Vector2& v2Point0,
	Vector2& v2Point1,
	Vector2& v2Normal,
	const ETHPhysicsEntityController::CONTACT_CALLBACKS::TYPE type)
{
	b2WorldManifold worldManifold; 
	contact->GetWorldManifold(&worldManifold); 

	const b2Body* bodyA = contact->GetFixtureA()->GetBody();
	*entityA = static_cast<ETHEntity*>(bodyA->GetUserData());

	*controllerA = static_cast<ETHPhysicsEntityController*>((*entityA)->GetController().get());

	const b2Body* bodyB = contact->GetFixtureB()->GetBody();
	*entityB = static_cast<ETHEntity*>(bodyB->GetUserData());
	*controllerB = static_cast<ETHPhysicsEntityController*>((*entityB)->GetController().get());

	// if both entities don't have the current callback type assigned to it, don't even bother...
	switch (type)
	{
	case ETHPhysicsEntityController::CONTACT_CALLBACKS::BEGIN:
		if (!(*controllerA)->HasBeginContactCallback() && !(*controllerB)->HasBeginContactCallback())
			return false;
		break;
	case ETHPhysicsEntityController::CONTACT_CALLBACKS::PRESOLVE:
		if (!(*controllerA)->HasPreSolveContactCallback() && !(*controllerB)->HasPreSolveContactCallback())
			return false;
		break;
	case ETHPhysicsEntityController::CONTACT_CALLBACKS::END:
		if (!(*controllerA)->HasEndContactCallback() && !(*controllerB)->HasEndContactCallback())
			return false;
		break;
	};

	const b2Vec2& point0 = worldManifold.points[0];
	const b2Vec2& point1 = worldManifold.points[1];
	const b2Vec2& normal = worldManifold.normal;
	v2Point0 = ETHPhysicsSimulator::ScaleFromBox2D(point0);
	v2Point1 = ETHPhysicsSimulator::ScaleFromBox2D(point1);
	v2Normal = Vector2(normal.x, normal.y);
	return true;
}

void ETHContactListener::BeginContact(b2Contact* contact)
{
	Vector2 point0, point1, normal;
	ETHEntity *entityA = 0, *entityB = 0;
	ETHPhysicsEntityController* controllerA = 0, *controllerB = 0;
	if (GetContactData(
		contact,
		&controllerA,
		&controllerB,
		&entityA,
		&entityB,
		point0,
		point1,
		normal,
		ETHPhysicsEntityController::CONTACT_CALLBACKS::BEGIN))
	{
		controllerA->RunBeginContactCallback(entityB, point0, point1, normal);
		controllerB->RunBeginContactCallback(entityA, point0, point1, normal);
	}
}

void ETHContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	m_runningPreSolveContactCallback = true;
	GS2D_UNUSED_ARGUMENT(oldManifold);
	Vector2 point0, point1, normal;
	ETHEntity *entityA = 0, *entityB = 0;
	ETHPhysicsEntityController* controllerA = 0, *controllerB = 0;
	if (GetContactData(
		contact,
		&controllerA,
		&controllerB,
		&entityA,
		&entityB,
		point0,
		point1,
		normal,
		ETHPhysicsEntityController::CONTACT_CALLBACKS::PRESOLVE))
	{
		controllerA->RunPreSolveContactCallback(entityB, point0, point1, normal);
		controllerB->RunPreSolveContactCallback(entityA, point0, point1, normal);
	}

	// disable the contact if the Disable[Next]Contact has been called in the contact callback script code
	if (m_disableNextContact)
	{
		contact->SetEnabled(false);
		m_disableNextContact = false;
	}
	m_runningPreSolveContactCallback = false;
}

void ETHContactListener::EndContact(b2Contact* contact)
{
	Vector2 point0, point1, normal;
	ETHEntity *entityA = 0, *entityB = 0;
	ETHPhysicsEntityController* controllerA = 0, *controllerB = 0;
	if (GetContactData(
		contact,
		&controllerA,
		&controllerB,
		&entityA,
		&entityB,
		point0,
		point1,
		normal,
		ETHPhysicsEntityController::CONTACT_CALLBACKS::END))
	{
		EndContactCallbackDataPtr data(new EndContactCallbackData);

		entityA->AddRef();
		data->entityA = entityA;
		entityB->AddRef();
		data->entityB = entityB;

		data->normal = normal;
		data->point0 = point0;
		data->point1 = point1;

		m_stackedEndContactCallbacks.push_back(data);
	}
}

void ETHContactListener::ClearStackedEndContactCallbacks()
{
	for (std::list<EndContactCallbackDataPtr>::iterator iter = m_stackedEndContactCallbacks.begin();
		iter != m_stackedEndContactCallbacks.end(); ++iter)
	{
		EndContactCallbackDataPtr data = (*iter);
		data->entityA->Release();
		data->entityB->Release();
	}
	m_stackedEndContactCallbacks.clear();
}

void ETHContactListener::RunAndClearStackedEndContactCallbacks()
{
	for (std::list<EndContactCallbackDataPtr>::iterator iter = m_stackedEndContactCallbacks.begin();
		iter != m_stackedEndContactCallbacks.end(); ++iter)
	{
		EndContactCallbackDataPtr data = (*iter);
		
		ETHPhysicsEntityController* controllerA = static_cast<ETHPhysicsEntityController*>(data->entityA->GetController().get());
		ETHPhysicsEntityController* controllerB = static_cast<ETHPhysicsEntityController*>(data->entityB->GetController().get());

		if (controllerA)
			if (controllerA->GetBody())
				controllerA->RunEndContactCallback(data->entityB, data->point0, data->point1, data->normal);

		if (controllerB)
			if (controllerB->GetBody())
				controllerB->RunEndContactCallback(data->entityA, data->point0, data->point1, data->normal);

		data->entityA->Release();
		data->entityB->Release();
	}
	m_stackedEndContactCallbacks.clear();
}

void ETHContactListener::DisableNextContact()
{
	m_disableNextContact = true;
}

bool ETHContactListener::IsRunningPreSolveContactCallback() const
{
	return m_runningPreSolveContactCallback;
}
