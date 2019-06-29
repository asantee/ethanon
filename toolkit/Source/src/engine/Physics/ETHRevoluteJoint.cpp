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
