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
