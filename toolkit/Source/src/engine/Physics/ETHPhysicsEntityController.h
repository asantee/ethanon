#ifndef ETH_PHYSICS_CONTROLLER_H_
#define ETH_PHYSICS_CONTROLLER_H_

#include "../Entity/ETHEntity.h"
#include "../Entity/ETHEntityArray.h"
#include "ETHJoint.h"

using namespace gs2d::math;
using namespace gs2d;

class ETHPhysicsController;

class ETHPhysicsEntityController : public ETHRawEntityController
{
	friend class ETHPhysicsController;
	bool RunContactCallback(asIScriptFunction* func, ETHEntity* other, Vector2& point0, Vector2& point1, Vector2& normal);

public:

	static const str_type::string BEGIN_CONTACT_CALLBACK_PREFIX;
	static const str_type::string END_CONTACT_CALLBACK_PREFIX;
	static const str_type::string PRESOLVE_CONTACT_CALLBACK_PREFIX;
	static const str_type::string CONTACT_CALLBACK_ARGS;

	struct CONTACT_CALLBACKS
	{
		CONTACT_CALLBACKS();
		enum TYPE
		{
			BEGIN = 0,
			END = 1,
			PRESOLVE = 2
		};
		asIScriptFunction* beginContact;
		asIScriptFunction* endContact;
		asIScriptFunction* preSolveContact;
	};

	ETHPhysicsEntityController(const ETHEntityControllerPtr& old, b2Body* body, boost::shared_ptr<b2World> world,
		asIScriptModule* module, asIScriptContext* context);
	void Update(const float lastFrameElapsedTime, ETHBucketManager& buckets);
	void SetPos(const Vector3& pos);
	void AddToPos(const Vector3& pos);
	void AddToAngle(const float angle);
	void SetAngle(const float angle);
	void Destroy();
	void Scale(const Vector2& scale, ETHEntity* entity);

	bool HasBeginContactCallback() const;
	bool HasEndContactCallback() const;
	bool HasPreSolveContactCallback() const;

	bool RunBeginContactCallback(ETHEntity* other, Vector2& point0, Vector2& point1, Vector2& normal);
	bool RunEndContactCallback(ETHEntity* other, Vector2& point0, Vector2& point1, Vector2& normal);
	bool RunPreSolveContactCallback(ETHEntity* other, Vector2& point0, Vector2& point1, Vector2& normal);

	bool ResolveJoints(ETHEntityArray& entities, const ETHEntityProperties& properties, ETHPhysicsSimulator& simulator);
	void RemoveJoints();
	void KillJoint(b2Joint* joint);
	std::size_t GetNumJoints() const;
	ETHJointPtr GetJoint(const std::size_t joindIdx);
	b2Body* GetBody();

	void SetPosXY(const Vector2& pos);
	void SetPosX(const float v);
	void SetPosY(const float v);
	void AddToPosXY(const Vector2& pos);
	void AddToPosX(const float v);
	void AddToPosY(const float v);

protected:
	b2Body* m_body;
	boost::shared_ptr<b2World> m_world;
	CONTACT_CALLBACKS m_contactCallbacks;
	std::vector<ETHJointPtr> m_joints;
	asIScriptFunction* GetContactCallback(const str_type::string& prefix, asIScriptModule* module);
	bool IsValidFunction(asIScriptFunction* func) const;
};

typedef boost::shared_ptr<ETHPhysicsEntityController> ETHPhysicsEntityControllerPtr;

#endif
