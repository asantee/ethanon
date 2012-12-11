/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

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

#include "ETHContactListener.h"
#include "ETHPhysicsSimulator.h"
#include "../Entity/ETHEntity.h"

ETHContactListener::ETHContactListener() :
	m_disableNextContact(false),
	m_runningPreSolveContactCallback(false)
{
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

	// if neither entities have the current callback type assigned to it, don't even bother...
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
	v2Point0 = Vector2(point0.x, point0.y);
	v2Point1 = Vector2(point1.x, point1.y);
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
		controllerA->RunEndContactCallback(entityB, point0, point1, normal);
		controllerB->RunEndContactCallback(entityA, point0, point1, normal);
	}
}

void ETHContactListener::DisableNextContact()
{
	m_disableNextContact = true;
}

bool ETHContactListener::IsRunningPreSolveContactCallback() const
{
	return m_runningPreSolveContactCallback;
}
