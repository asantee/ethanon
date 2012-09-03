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
	bool RunContactCallback(const int functionId, ETHEntity* other, Vector2& point0, Vector2& point1, Vector2& normal);

protected:
	b2Body* m_body;
	boost::shared_ptr<b2World> m_world;
	int m_beginContactCallbackId;
	int m_endContactCallbackId;
	std::vector<ETHJointPtr> m_joints;
	int GetContactCallbackId(const str_type::string& prefix, asIScriptModule* module);
	bool IsValidFunction(const int functionId) const;

public:
	ETHPhysicsEntityController(const ETHEntityControllerPtr& old, b2Body* body, boost::shared_ptr<b2World> world,
		asIScriptModule* module, asIScriptContext* context);
	void Update(const unsigned long lastFrameElapsedTime, ETHBucketManager& buckets);
	void SetPos(const Vector3& pos);
	void AddToPos(const Vector3& pos);
	void AddToAngle(const float angle);
	void SetAngle(const float angle);
	void Destroy();
	void Scale(const Vector2& scale, ETHEntity* entity);
	bool HasBeginContactCallback() const;
	bool HasEndContactCallback() const;
	bool RunBeginContactCallback(ETHEntity* other, Vector2& point0, Vector2& point1, Vector2& normal);
	bool RunEndContactCallback(ETHEntity* other, Vector2& point0, Vector2& point1, Vector2& normal);
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
};

typedef boost::shared_ptr<ETHPhysicsEntityController> ETHPhysicsEntityControllerPtr;

#endif
