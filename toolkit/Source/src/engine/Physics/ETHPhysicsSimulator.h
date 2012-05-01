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

#ifndef ETH_PHYSICS_SIMULATOR_H_
#define ETH_PHYSICS_SIMULATOR_H_

#include "ETHPhysicsEntityController.h"
#include "ETHDestructionListener.h"

using namespace gs2d::math;
using namespace gs2d;

class ETHPhysicsSimulator
{
	boost::shared_ptr<b2World> m_world;
	ETHContactListener m_contactListener;

	const static b2Vec2 DEFAULT_GRAVITY;
	const static float  DEFAULT_SCALE;
	static int32 m_velocityIterations;
	static int32 m_positionIterations;
	float m_timeStepScale;
	float m_dynamicTimeStep;
	bool m_fixedTimeStep;
	float m_fixedTimeStepValue;
	float m_timeStepUpdateTime;
	ETHDestructionListener m_destructionListener;

public:
	ETHPhysicsSimulator(const float currentFpsRate);
	~ETHPhysicsSimulator();

	static std::vector<b2Shape*> GetBoxShape(const ETHCollisionBox& box, const float angle = 0.0f);
	static std::vector<b2Shape*> GetCircleShape(const ETHCollisionBox& box);
	static std::vector<b2Shape*> GetPolygonShape(const ETHCollisionBox& box, const ETHPolygonPtr& polygon);
	static ETH_BODY_SHAPE StringToShape(const gs2d::str_type::string& str);

	ETHPhysicsEntityControllerPtr CreatePhysicsController(ETHEntity *entity, asIScriptModule* module, asIScriptContext* context);
	static b2Body* CreateBody(ETHEntity *entity, const boost::shared_ptr<b2World>& world);
	static ETHPhysicsEntityControllerPtr CreatePhysicsController(ETHEntity *entity, const boost::shared_ptr<b2World>& world,
		asIScriptModule* module, asIScriptContext* context);
	void Update(const unsigned long lastFrameElapsedTime);
	static b2Vec2 ScaleToBox2D(const Vector2& v);
	static Vector2 ScaleFromBox2D(const b2Vec2& v);
	static float32 ScaleToBox2D(const float& v);
	static float ScaleFromBox2D(const float32& v);

	static void SetNumIterations(const int velocityIterations, const int positionIterations);
	static void GetNumIterations(int& velocityIterations, int& positionIterations);
	static void SetGravity(const Vector2& gravity, const boost::shared_ptr<b2World>& world);
	static Vector2 GetGravity(const boost::shared_ptr<b2World>& world);
	float GetCurrentDynamicTimeStepMS() const;
	void SetGravity(const Vector2& gravity);
	Vector2 GetGravity() const;
	void SetTimeStepScale(const float scale);
	bool IsFixedTimeStep() const;
	float GetFixedTimeStepValue() const;
	void SetFixedTimeStep(const bool enable);
	void SetFixedTimeStepValue(const float value);
	float GetTimeStepScale() const;
	ETHEntity* GetClosestContact(const Vector2& a, const Vector2& b, Vector2& point, Vector2& normal);
	ETHEntity* GetClosestContact(const Vector2& a, const Vector2& b, Vector2& point, Vector2& normal, const str_type::string& semicolonSeparatedIgnoreList);
	bool GetContactEntities(const Vector2& a, const Vector2& b, ETHEntityArray& entities);
	void ResolveJoints(ETHEntityArray& entities);
	b2Joint* CreateJoint(b2JointDef& jointDef);
};

#endif
