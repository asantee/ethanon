#include "ETHJoint.h"
#include "ETHRevoluteJoint.h"
#include "ETHPhysicsSimulator.h"

const gs2d::str_type::string ETHJoint::REVOLUTE_JOINT_NAME = GS_L("revoluteJoint");

ETHJoint::~ETHJoint() {}

boost::shared_ptr<ETHJoint> ETHJoint::CreateJoint(const gs2d::str_type::string& jointName, const gs2d::enml::File& file, ETHPhysicsSimulator& simulator,
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

gs2d::math::Vector2 ETHJoint::GetAttachPointInEntityA(const gs2d::str_type::string& jointName, const gs2d::enml::File& file)
{
	float x=0.0f, y=0.0f;
	file.GetFloat(jointName, GS_L("attachPointAX"), &x);
	file.GetFloat(jointName, GS_L("attachPointAY"), &y);
	return gs2d::math::Vector2(x, y);
}

gs2d::math::Vector2 ETHJoint::GetAttachPointInEntityB(const gs2d::str_type::string& jointName, const gs2d::enml::File& file)
{
	float x=0.0f, y=0.0f;
	file.GetFloat(jointName, GS_L("attachPointBX"), &x);
	file.GetFloat(jointName, GS_L("attachPointBY"), &y);
	return gs2d::math::Vector2(x, y);
}

gs2d::str_type::string ETHJoint::GetOtherEntityName(const gs2d::str_type::string& jointName, const gs2d::enml::File& file)
{
	return file.Get(jointName, GS_L("otherEntityName"));
}

bool ETHJoint::IsRevoluteJoint(const gs2d::str_type::string& jointName)
{
	return (jointName.compare(0, REVOLUTE_JOINT_NAME.length(), REVOLUTE_JOINT_NAME) == 0);
}

bool ETHJoint::GetEnableLimit(const gs2d::str_type::string& jointName, const gs2d::enml::File& file)
{
	return ETHGlobal::IsTrue(file.Get(jointName, GS_L("enableLimit")));
}

bool ETHJoint::GetEnableMotor(const gs2d::str_type::string& jointName, const gs2d::enml::File& file)
{
	return ETHGlobal::IsTrue(file.Get(jointName, GS_L("enableMotor")));
}

float ETHJoint::GetUpperAngle(const gs2d::str_type::string& jointName, const gs2d::enml::File& file)
{
	float angle = gs2d::math::constant::PI2;
	file.GetFloat(jointName, GS_L("upperAngle"), &angle);
	return angle;
}

float ETHJoint::GetLowerAngle(const gs2d::str_type::string& jointName, const gs2d::enml::File& file)
{
	float angle = 0.0f;
	file.GetFloat(jointName, GS_L("lowerAngle"), &angle);
	return angle;
}

float ETHJoint::GetMaxMotorTorque(const gs2d::str_type::string& jointName, const gs2d::enml::File& file)
{
	float value = 100.0f;
	file.GetFloat(jointName, GS_L("maxMotorTorque"), &value);
	return value;
}

float ETHJoint::GetMotorSpeed(const gs2d::str_type::string& jointName, const gs2d::enml::File& file)
{
	float value = 0.0f;
	file.GetFloat(jointName, GS_L("motorSpeed"), &value);
	return value;
}
