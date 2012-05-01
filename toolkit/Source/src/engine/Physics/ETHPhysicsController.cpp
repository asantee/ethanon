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

#include "ETHPhysicsController.h"

ETHPhysicsController::ETHPhysicsController(const ETHPhysicsEntityControllerPtr& controller) :
	m_controller(controller),
	m_ref(1)
{
}

void ETHPhysicsController::AddRef()
{
	m_ref++;
}

void ETHPhysicsController::Release()
{
	if (--m_ref == 0)
	{
		delete this;
	}
}

ETH_BODY_SHAPE ETHPhysicsController::GetShape() const
{
	b2Body* body = m_controller->m_body;
	if (!body) return ETHBS_NONE;
	const ETHEntity* entity = static_cast<ETHEntity*>(body->GetUserData());
	if (!entity) return ETHBS_NONE;
	return entity->GetShape();
}

void ETHPhysicsController::SetGravityScale(const float scale)
{
	b2Body* body = m_controller->m_body;
	if (!body) return;
	body->SetGravityScale(scale);
}

float ETHPhysicsController::GetGravityScale() const
{
	const b2Body* body = m_controller->m_body;
	if (!body) return 0.0f;
	return body->GetGravityScale();
}

void ETHPhysicsController::SetAngularVelocity(const float velocity)
{
	b2Body* body = m_controller->m_body;
	if (!body) return;
	body->SetAngularVelocity(velocity);
}

void ETHPhysicsController::SetAwake(const bool awake)
{
	b2Body* body = m_controller->m_body;
	if (!body) return;
	body->SetAwake(awake);
}

void ETHPhysicsController::SetBullet(const bool enable)
{
	b2Body* body = m_controller->m_body;
	if (!body) return;
	body->SetBullet(enable);
}

bool ETHPhysicsController::IsAwake() const
{
	const b2Body* body = m_controller->m_body;
	if (!body) return false;
	return body->IsAwake();
}

bool ETHPhysicsController::IsBullet() const
{
	const b2Body* body = m_controller->m_body;
	if (!body) return false;
	return body->IsBullet();
}

float ETHPhysicsController::GetMass() const
{
	const b2Body* body = m_controller->m_body;
	if (!body) return 0.0f;
	return body->GetMass();
}

float ETHPhysicsController::GetAngularVelocity() const
{
	const b2Body* body = m_controller->m_body;
	if (!body) return 0.0f;
	return body->GetAngularVelocity();
}

gs2d::math::Vector2 ETHPhysicsController::GetLinearVelocity() const
{
	const b2Body* body = m_controller->m_body;
	if (!body) return Vector2(0.0f, 0.0f);
	const b2Vec2& v = body->GetLinearVelocity();
	return Vector2(v.x, v.y);
}

void ETHPhysicsController::ApplyTorque(const float torque)
{
	b2Body* body = m_controller->m_body;
	if (!body) return;
	body->ApplyTorque(torque);
}

void ETHPhysicsController::ApplyForce(const Vector2& force, const Vector2& point)
{
	b2Body* body = m_controller->m_body;
	if (!body) return;
	body->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(point.x, point.y));
}

void ETHPhysicsController::ApplyForceToCenter(const Vector2& force)
{
	b2Body* body = m_controller->m_body;
	if (!body) return;
	body->ApplyForceToCenter(b2Vec2(force.x, force.y));
}

void ETHPhysicsController::SetLinearVelocity(const Vector2& velocity)
{
	b2Body* body = m_controller->m_body;
	if (!body) return;
	body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
}

void ETHPhysicsController::ApplyAngularImpulse(const float impulse)
{
	b2Body* body = m_controller->m_body;
	if (!body) return;
	body->ApplyAngularImpulse(impulse);
}

void ETHPhysicsController::ApplyLinearImpulse(const Vector2& impulse, const Vector2& point)
{
	b2Body* body = m_controller->m_body;
	if (!body) return;
	body->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(point.x, point.y));
}

bool ETHPhysicsController::IsSensor() const
{
	b2Body* body = m_controller->m_body;
	if (!body) return false;
	const ETHEntity* entity = static_cast<ETHEntity*>(body->GetUserData());
	if (!entity) return false;
	return entity->IsSensor();
}

bool ETHPhysicsController::IsFixedRotation() const
{
	b2Body* body = m_controller->m_body;
	if (!body) return false;
	const ETHEntity* entity = static_cast<ETHEntity*>(body->GetUserData());
	if (!entity) return false;
	return entity->IsFixedRotation();
}

float ETHPhysicsController::GetFriction() const
{
	b2Body* body = m_controller->m_body;
	if (!body) return 0.0f;
	return body->GetFixtureList()->GetFriction();  // TODO/TO-DO: manipulate next fixtures
}

void ETHPhysicsController::SetFriction(const float friction)
{
	b2Body* body = m_controller->m_body;
	if (!body) return;
	body->GetFixtureList()->SetFriction(friction); // TODO/TO-DO: manipulate next fixtures
}

float ETHPhysicsController::GetRestitution() const
{
	b2Body* body = m_controller->m_body;
	if (!body) return 0.0f;
	return body->GetFixtureList()->GetRestitution();  // TODO/TO-DO: manipulate next fixtures
}

void ETHPhysicsController::SetRestitution(const float res)
{
	b2Body* body = m_controller->m_body;
	if (!body) return;
	return body->GetFixtureList()->SetRestitution(res);  // TODO/TO-DO: manipulate next fixtures
}

ETHPhysicsEntityControllerPtr ETHPhysicsController::GetEntityController()
{
	return m_controller;
}

unsigned int ETHPhysicsController::GetNumJoints() const
{
	return m_controller->GetNumJoints();
}

b2RevoluteJoint* ETHPhysicsController::GetRevoluteJoint(const unsigned int idx)
{
	ETHJointPtr jointPtr = m_controller->GetJoint(idx);
	if (jointPtr)
	{
		b2Joint* joint = jointPtr->GetB2Joint();
		if (joint->GetType() == e_revoluteJoint)
			return static_cast<b2RevoluteJoint*>(joint);
	}
	return 0;
}
