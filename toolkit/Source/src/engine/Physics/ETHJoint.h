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

#ifndef ETH_JOINT_H_
#define ETH_JOINT_H_

#include <Enml/Enml.h>
#include <boost/shared_ptr.hpp>
#include <Math/GameMath.h>
#include "../Entity/ETHEntity.h"

class ETHPhysicsSimulator;

class ETHJoint
{
	static const gs2d::str_type::string REVOLUTE_JOINT_NAME;
public:
	virtual ~ETHJoint();
	virtual void KillJoint() = 0;
	static boost::shared_ptr<ETHJoint> CreateJoint(const gs2d::str_type::string& jointName, const gs2d::enml::File& file, ETHPhysicsSimulator& simulator,
												   ETHEntity* entityA, ETHEntity* entityB);
	static bool IsRevoluteJoint(const gs2d::str_type::string& jointName);

	static gs2d::math::Vector2 GetAttachPointInEntityA(const gs2d::str_type::string& jointName, const gs2d::enml::File& file);
	static gs2d::math::Vector2 GetAttachPointInEntityB(const gs2d::str_type::string& jointName, const gs2d::enml::File& file);
	static gs2d::str_type::string GetOtherEntityName(const gs2d::str_type::string& jointName, const gs2d::enml::File& file);
	static bool GetEnableLimit(const gs2d::str_type::string& jointName, const gs2d::enml::File& file);
	static bool GetEnableMotor(const gs2d::str_type::string& jointName, const gs2d::enml::File& file);
	static float GetUpperAngle(const gs2d::str_type::string& jointName, const gs2d::enml::File& file);
	static float GetLowerAngle(const gs2d::str_type::string& jointName, const gs2d::enml::File& file);
	static float GetMaxMotorTorque(const gs2d::str_type::string& jointName, const gs2d::enml::File& file);
	static float GetMotorSpeed(const gs2d::str_type::string& jointName, const gs2d::enml::File& file);
	virtual b2Joint* GetB2Joint() = 0;
	virtual void Destroy() = 0;
};

typedef boost::shared_ptr<ETHJoint> ETHJointPtr;

#endif
