#ifndef ETH_JOINT_H_
#define ETH_JOINT_H_

#include <Enml/Enml.h>
#include <boost/shared_ptr.hpp>
#include <Math/GameMath.h>
#include "../Entity/ETHEntity.h"

class ETHPhysicsSimulator;

class ETHJoint
{
	static const std::string REVOLUTE_JOINT_NAME;
public:
	virtual ~ETHJoint();
	virtual void KillJoint() = 0;
	static boost::shared_ptr<ETHJoint> CreateJoint(const std::string& jointName, const gs2d::enml::File& file, ETHPhysicsSimulator& simulator,
												   ETHEntity* entityA, ETHEntity* entityB);
	static bool IsRevoluteJoint(const std::string& jointName);

	static gs2d::math::Vector2 GetAttachPointInEntityA(const std::string& jointName, const gs2d::enml::File& file);
	static gs2d::math::Vector2 GetAttachPointInEntityB(const std::string& jointName, const gs2d::enml::File& file);
	static std::string GetOtherEntityName(const std::string& jointName, const gs2d::enml::File& file);
	static bool GetEnableLimit(const std::string& jointName, const gs2d::enml::File& file);
	static bool GetEnableMotor(const std::string& jointName, const gs2d::enml::File& file);
	static float GetUpperAngle(const std::string& jointName, const gs2d::enml::File& file);
	static float GetLowerAngle(const std::string& jointName, const gs2d::enml::File& file);
	static float GetMaxMotorTorque(const std::string& jointName, const gs2d::enml::File& file);
	static float GetMotorSpeed(const std::string& jointName, const gs2d::enml::File& file);
	virtual b2Joint* GetB2Joint() = 0;
	virtual void Destroy() = 0;
};

typedef boost::shared_ptr<ETHJoint> ETHJointPtr;

#endif
