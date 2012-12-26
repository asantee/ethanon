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

#ifndef ETH_PHYSICS_CONTROLLER
#define ETH_PHYSICS_CONTROLLER

#include "ETHPhysicsSimulator.h"
#include "../Util/ETHGlobalScaleManager.h"

class ETHPhysicsController
{
	int m_ref;
	ETHPhysicsEntityControllerPtr m_controller;
	ETHGlobalScaleManagerPtr m_globalScaleManager;

public:
	ETHPhysicsController(const ETHPhysicsEntityControllerPtr& controller, ETHGlobalScaleManagerPtr globalScaleManager);
	void AddRef();
	void Release();
	ETHEntityProperties::BODY_SHAPE GetShape() const;
	void SetGravityScale(const float scale);
	float GetGravityScale() const;
	void SetAngularVelocity(const float velocity);
	void SetAwake(const bool awake);
	void SetBullet(const bool enable);
	bool IsAwake() const;
	bool IsBullet() const;
	float GetMass() const;
	float GetAngularVelocity() const;
	Vector2 GetLinearVelocity() const;
	void ApplyTorque(const float torque);
	void ApplyForce(const Vector2& force, const Vector2& point);
	void ApplyForceToCenter(const Vector2& force);
	void SetLinearVelocity(const Vector2& velocity);
	void ApplyAngularImpulse(const float impulse);
	void ApplyLinearImpulse(const Vector2& impulse, const Vector2& point);
	bool IsSensor() const;
	bool IsFixedRotation() const;
	float GetFriction() const;
	float GetDensity() const;
	void SetFriction(const float friction);
	float GetRestitution() const;
	void SetRestitution(const float res);
	ETHPhysicsEntityControllerPtr GetEntityController();
	unsigned int GetNumJoints() const;
	b2RevoluteJoint* GetRevoluteJoint(const unsigned int idx);
};

#endif