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

#include "ETHRevoluteJoint.h"

static ETHPhysicsEntityControllerPtr GetPhysicsController(ETHEntity* entity)
{
	return boost::dynamic_pointer_cast<ETHPhysicsEntityController>(entity->GetController());
}

ETHRevoluteJoint::ETHRevoluteJoint(const gs2d::str_type::string& jointName, const enml::File& file, ETHPhysicsSimulator& simulator, ETHEntity* entityA, ETHEntity* entityB) :
	m_joint(NULL)
{
	b2Body* bodyA = GetPhysicsController(entityA)->GetBody();
	b2Body* bodyB = GetPhysicsController(entityB)->GetBody();
	b2RevoluteJointDef jointDef;
	jointDef.Initialize(bodyA, bodyB, bodyA->GetWorldCenter());

	jointDef.collideConnected = false;
	jointDef.localAnchorA = ComputeAnchorPosition(entityA, ETHJoint::GetAttachPointInEntityA(jointName, file));
	jointDef.localAnchorB = ComputeAnchorPosition(entityB, ETHJoint::GetAttachPointInEntityB(jointName, file));
	jointDef.userData = entityA;

	m_world = bodyA->GetWorld();

	jointDef.lowerAngle = GetLowerAngle(jointName, file);
	jointDef.upperAngle = GetUpperAngle(jointName, file);
	jointDef.enableLimit = GetEnableLimit(jointName, file);
	jointDef.maxMotorTorque = GetMaxMotorTorque(jointName, file);
	jointDef.motorSpeed = GetMotorSpeed(jointName, file);
	jointDef.enableMotor = GetEnableMotor(jointName, file);

	m_joint = static_cast<b2RevoluteJoint*>(simulator.CreateJoint(jointDef));
}

ETHRevoluteJoint::~ETHRevoluteJoint()
{
	Destroy();
}

b2Vec2 ETHRevoluteJoint::ComputeAnchorPosition(ETHEntity *entity, const Vector2& anchorPoint)
{
	const Vector2 boxSize(ETHGlobal::ToVector2(entity->GetCollisionBox().size) * entity->GetScale() * 0.5f);
	return ETHPhysicsSimulator::ScaleToBox2D(anchorPoint * boxSize);
}

b2Joint* ETHRevoluteJoint::GetB2Joint()
{
	return m_joint;
}

void ETHRevoluteJoint::KillJoint()
{
	m_joint = NULL;
}

void ETHRevoluteJoint::Destroy()
{
	if (m_joint)
	{
		m_world->DestroyJoint(m_joint);
		m_joint = NULL;
	}
}
