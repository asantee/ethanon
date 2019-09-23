#ifndef ETH_PHYSICS_CONTROLLER
#define ETH_PHYSICS_CONTROLLER

#include "ETHPhysicsSimulator.h"

class ETHPhysicsController
{
	int32_t m_ref;
	ETHPhysicsEntityControllerPtr m_controller;

public:
	ETHPhysicsController(const ETHPhysicsEntityControllerPtr& controller);
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
	void SetDensity(const float density);
	void SetFriction(const float friction);
	float GetRestitution() const;
	void SetRestitution(const float res);
	ETHPhysicsEntityControllerPtr GetEntityController();
	uint32_t GetNumJoints() const;
	b2RevoluteJoint* GetRevoluteJoint(const uint32_t idx);
};

#endif
