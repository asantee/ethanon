#ifndef ETH_REVOLUTE_JOINT_H_
#define ETH_REVOLUTE_JOINT_H_

#include "ETHJoint.h"

#include "ETHPhysicsController.h"

class ETHRevoluteJoint : public ETHJoint
{
public:
	ETHRevoluteJoint(const std::string& jointName, const enml::File& file, ETHPhysicsSimulator& simulator, ETHEntity* entityA, ETHEntity* entityB);
	~ETHRevoluteJoint();
	void KillJoint();

	static b2Vec2 ComputeAnchorPosition(ETHEntity *entity, const Vector2& anchorPoint);
	b2Joint* GetB2Joint();
	void Destroy();
private:
	b2RevoluteJoint* m_joint;
	b2World* m_world;
};

typedef boost::shared_ptr<ETHRevoluteJoint> ETHRevoluteJointPtr;

#endif
