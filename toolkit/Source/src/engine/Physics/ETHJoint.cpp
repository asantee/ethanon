#include "ETHJoint.h"
#include "ETHRevoluteJoint.h"
#include "ETHPhysicsSimulator.h"

const std::string ETHJoint::REVOLUTE_JOINT_NAME = ("revoluteJoint");

ETHJoint::~ETHJoint() {}

boost::shared_ptr<ETHJoint> ETHJoint::CreateJoint(const std::string& jointName, const gs2d::enml::File& file, ETHPhysicsSimulator& simulator,
												  ETHEntity* entityA, ETHEntity* entityB)
{
	if (IsRevoluteJoint(jointName))
	{
		return ETHJointPtr(new ETHRevoluteJoint(jointName, file, simulator, entityA, entityB));
	}
	else
	{
		return ETHJointPtr();
	}
}

gs2d::math::Vector2 ETHJoint::GetAttachPointInEntityA(const std::string& jointName, const gs2d::enml::File& file)
{
	float x=0.0f, y=0.0f;
	file.GetFloat(jointName, ("attachPointAX"), &x);
	file.GetFloat(jointName, ("attachPointAY"), &y);
	return gs2d::math::Vector2(x, y);
}

gs2d::math::Vector2 ETHJoint::GetAttachPointInEntityB(const std::string& jointName, const gs2d::enml::File& file)
{
	float x=0.0f, y=0.0f;
	file.GetFloat(jointName, ("attachPointBX"), &x);
	file.GetFloat(jointName, ("attachPointBY"), &y);
	return gs2d::math::Vector2(x, y);
}

std::string ETHJoint::GetOtherEntityName(const std::string& jointName, const gs2d::enml::File& file)
{
	return file.Get(jointName, ("otherEntityName"));
}

bool ETHJoint::IsRevoluteJoint(const std::string& jointName)
{
	return (jointName.compare(0, REVOLUTE_JOINT_NAME.length(), REVOLUTE_JOINT_NAME) == 0);
}

bool ETHJoint::GetEnableLimit(const std::string& jointName, const gs2d::enml::File& file)
{
	return ETHGlobal::IsTrue(file.Get(jointName, ("enableLimit")));
}

bool ETHJoint::GetEnableMotor(const std::string& jointName, const gs2d::enml::File& file)
{
	return ETHGlobal::IsTrue(file.Get(jointName, ("enableMotor")));
}

float ETHJoint::GetUpperAngle(const std::string& jointName, const gs2d::enml::File& file)
{
	float angle = gs2d::math::constant::PI2;
	file.GetFloat(jointName, ("upperAngle"), &angle);
	return angle;
}

float ETHJoint::GetLowerAngle(const std::string& jointName, const gs2d::enml::File& file)
{
	float angle = 0.0f;
	file.GetFloat(jointName, ("lowerAngle"), &angle);
	return angle;
}

float ETHJoint::GetMaxMotorTorque(const std::string& jointName, const gs2d::enml::File& file)
{
	float value = 100.0f;
	file.GetFloat(jointName, ("maxMotorTorque"), &value);
	return value;
}

float ETHJoint::GetMotorSpeed(const std::string& jointName, const gs2d::enml::File& file)
{
	float value = 0.0f;
	file.GetFloat(jointName, ("motorSpeed"), &value);
	return value;
}
