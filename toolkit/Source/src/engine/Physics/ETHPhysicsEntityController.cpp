#include "ETHPhysicsEntityController.h"

#include "../../addons/scriptbuilder.h"

#include "../ETHEngine.h"

#include "ETHPhysicsController.h"

const str_type::string ETHPhysicsEntityController::BEGIN_CONTACT_CALLBACK_PREFIX    = GS_L("ETHBeginContactCallback_");
const str_type::string ETHPhysicsEntityController::END_CONTACT_CALLBACK_PREFIX      = GS_L("ETHEndContactCallback_");
const str_type::string ETHPhysicsEntityController::PRESOLVE_CONTACT_CALLBACK_PREFIX = GS_L("ETHPreSolveContactCallback_");
const str_type::string ETHPhysicsEntityController::CONTACT_CALLBACK_ARGS            = GS_L("ETHEntity@, ETHEntity@, const vector2 &in");

ETHPhysicsEntityController::CONTACT_CALLBACKS::CONTACT_CALLBACKS() :
	beginContact(0),
	endContact(0),
	preSolveContact(0)
{
}

ETHPhysicsEntityController::ETHPhysicsEntityController(
	const ETHEntityControllerPtr& old,
	b2Body* body,
	boost::shared_ptr<b2World> world,
	asIScriptModule* module,
	asIScriptContext* context) :
	ETHRawEntityController(old, 0, 0, 0),
	m_body(body),
	m_world(world)
{
	ETHRawEntityControllerPtr raw = boost::dynamic_pointer_cast<ETHRawEntityController>(old);
	if (raw)
	{
		m_callback = raw->GetCallback();
		m_constructorCallback = raw->GetConstructorCallback();
		m_pContext = context;
		m_contactCallbacks.beginContact    = GetContactCallback(BEGIN_CONTACT_CALLBACK_PREFIX,    module);
		m_contactCallbacks.endContact      = GetContactCallback(END_CONTACT_CALLBACK_PREFIX,      module);
		m_contactCallbacks.preSolveContact = GetContactCallback(PRESOLVE_CONTACT_CALLBACK_PREFIX, module);
	}
}

asIScriptFunction* ETHPhysicsEntityController::GetContactCallback(const str_type::string& prefix, asIScriptModule* module)
{
	ETHEntity* entity = static_cast<ETHEntity*>(m_body->GetUserData());
	str_type::stringstream ss;
	ss << prefix << Platform::RemoveExtension(entity->GetEntityName().c_str());
	return module->GetFunctionByName(ss.str().c_str());
}

void ETHPhysicsEntityController::Update(const float lastFrameElapsedTime, ETHBucketManager& buckets)
{
	if (!m_body)
		return;

	if (!m_body->IsActive())
		return;

	GS2D_UNUSED_ARGUMENT(lastFrameElapsedTime);
	const Vector2 pos = ETHPhysicsSimulator::ScaleFromBox2D(m_body->GetPosition());
	const Vector2 oldPos = Vector2(m_pos.x, m_pos.y);
	if (oldPos != pos)
	{
		m_pos = Vector3(pos, GetPos().z);
		buckets.RequestBucketMove(static_cast<ETHEntity*>(m_body->GetUserData()), oldPos, pos);
	}
	m_angle =-RadianToDegree(m_body->GetAngle());
}

void ETHPhysicsEntityController::SetPos(const Vector3& pos)
{
	SetPosXY(Vector2(pos.x, pos.y));
	m_pos.z = pos.z;
}

void ETHPhysicsEntityController::SetPosXY(const Vector2& pos)
{
	if (m_body)
	{
		m_body->SetTransform(ETHPhysicsSimulator::ScaleToBox2D(pos), m_body->GetAngle());
		m_body->SetAwake(true);
	}
	m_pos.x = pos.x;
	m_pos.y = pos.y;
}

void ETHPhysicsEntityController::SetPosX(const float v)
{
	SetPosXY(Vector2(v, m_pos.y));
}

void ETHPhysicsEntityController::SetPosY(const float v)
{
	SetPosXY(Vector2(m_pos.x, v));
}

void ETHPhysicsEntityController::AddToPos(const Vector3& pos)
{
	AddToPosXY(Vector2(pos.x, pos.y));
	m_pos.z += pos.z;
}

void ETHPhysicsEntityController::AddToPosX(const float v)
{
	AddToPosXY(Vector2(v, 0.0f));
}

void ETHPhysicsEntityController::AddToPosY(const float v)
{
	AddToPosXY(Vector2(0.0f, v));
}

void ETHPhysicsEntityController::AddToPosXY(const Vector2& pos)
{
	if (m_body)
	{
		m_body->SetTransform(ETHPhysicsSimulator::ScaleToBox2D(pos) + m_body->GetPosition(), m_body->GetAngle());
		m_body->SetAwake(true);
	}
	const Vector2 currentPos(ETHPhysicsSimulator::ScaleFromBox2D(m_body->GetPosition()));
	m_pos.x = currentPos.x;
	m_pos.y = currentPos.y;
}

void ETHPhysicsEntityController::AddToAngle(const float angle)
{
	if (m_body)
	{
		m_body->SetTransform(m_body->GetPosition(),-DegreeToRadian(angle) + m_body->GetAngle());
		m_body->SetAwake(true);
	}
	m_angle += angle;
}

void ETHPhysicsEntityController::SetAngle(const float angle)
{
	if (m_body)
	{
		m_body->SetTransform(m_body->GetPosition(),-DegreeToRadian(angle));
		m_body->SetAwake(true);
	}
	m_angle = angle;
}

void ETHPhysicsEntityController::Destroy()
{
	// only kill joints. Box2D will destroy them automatically
	for (std::size_t t = 0; t < m_joints.size(); t++)
	{
		if (m_joints[t])
		{
			m_joints[t]->KillJoint();
		}
	}
	m_joints.clear();
	if (m_body)
	{
		static_cast<ETHEntity*>(m_body->GetUserData())->Release();
		m_world->DestroyBody(m_body);
		m_body = 0;
	}
}

void ETHPhysicsEntityController::Scale(const Vector2& scale, ETHEntity* entity)
{
	if (!m_body)
		return;
	GS2D_UNUSED_ARGUMENT(scale);
	Destroy();
	m_body = ETHPhysicsSimulator::CreateBody(entity, m_world);
}

bool ETHPhysicsEntityController::HasBeginContactCallback() const
{
	return IsValidFunction(m_contactCallbacks.beginContact);
}

bool ETHPhysicsEntityController::HasEndContactCallback() const
{
	return IsValidFunction(m_contactCallbacks.endContact);
}

bool ETHPhysicsEntityController::HasPreSolveContactCallback() const
{
	return IsValidFunction(m_contactCallbacks.preSolveContact);
}

bool ETHPhysicsEntityController::RunContactCallback(asIScriptFunction* func, ETHEntity* other, Vector2& point0, Vector2& point1, Vector2& normal)
{
	if (!IsValidFunction(func))
		return true;

	ETHEntity* entity = static_cast<ETHEntity*>(m_body->GetUserData());

	if (m_pContext->Prepare(func) < 0) return false;
	if (m_pContext->SetArgObject(0, entity) < 0) return false;
	if (m_pContext->SetArgObject(1, other) < 0) return false;
	if (m_pContext->SetArgObject(2, &point0) < 0) return false;
	if (m_pContext->SetArgObject(3, &point1) < 0) return false;
	if (m_pContext->SetArgObject(4, &normal) < 0) return false;
	ETHGlobal::ExecuteContext(m_pContext, func, false);
	return true;
}

bool ETHPhysicsEntityController::RunBeginContactCallback(ETHEntity* other, Vector2& point0, Vector2& point1, Vector2& normal)
{
	return RunContactCallback(m_contactCallbacks.beginContact, other, point0, point1, normal);
}

bool ETHPhysicsEntityController::RunEndContactCallback(ETHEntity* other, Vector2& point0, Vector2& point1, Vector2& normal)
{
	return RunContactCallback(m_contactCallbacks.endContact, other, point0, point1, normal);
}

bool ETHPhysicsEntityController::RunPreSolveContactCallback(ETHEntity* other, Vector2& point0, Vector2& point1, Vector2& normal)
{
	return RunContactCallback(m_contactCallbacks.preSolveContact, other, point0, point1, normal);
}

bool ETHPhysicsEntityController::IsValidFunction(asIScriptFunction* func) const
{
	return (func != 0);
}

b2Body* ETHPhysicsEntityController::GetBody()
{
	return m_body;
}

bool ETHPhysicsEntityController::ResolveJoints(ETHEntityArray& entities, const ETHEntityProperties& properties, ETHPhysicsSimulator& simulator)
{
	if (properties.enmlJointDefinitions == GS_L(""))
		return false;

	const std::size_t numEntities = entities.size();
	const enml::File file(properties.enmlJointDefinitions);
	std::list<str_type::string> jointNames;
	file.GetEntityNameList(jointNames);
	for (std::list<str_type::string>::const_iterator iter = jointNames.begin(); iter != jointNames.end(); ++iter)
	{
		const str_type::string& jointName = *iter;

		// get the other entity name from the joint definition
		str_type::string otherEntityName = ETHJoint::GetOtherEntityName(jointName, file);
		int otherEntityID = -1;

		// if the other entity name is not declared in the joint definition, look for it into the custom data
		if (otherEntityName == GS_L(""))
		{
			if (properties.Check(jointName) == ETHCustomData::DT_STRING)
			{
				properties.GetString(jointName, otherEntityName);
			}
			else if (properties.Check(jointName) == ETHCustomData::DT_INT)
			{
				properties.GetInt(jointName, otherEntityID);
			}
			else
			{
				continue;
			}
		}

		// iterate over entities to find the other-entity
		for (std::size_t t = 0; t < numEntities; t++)
		{
			const unsigned int idx = static_cast<unsigned int>(t);
			if (entities[idx]->GetEntityName() == otherEntityName || entities[idx]->GetID() == otherEntityID)
			{
				boost::shared_ptr<ETHJoint> joint =
					ETHJoint::CreateJoint(jointName, file, simulator, static_cast<ETHEntity*>(m_body->GetUserData()), (entities[idx]));
				if (joint)
					m_joints.push_back(joint);
				break;
			}
		}
	}
	return true;
}

void ETHPhysicsEntityController::RemoveJoints()
{
	m_joints.clear();
}

void ETHPhysicsEntityController::KillJoint(b2Joint* joint)
{
	for (std::size_t t = 0; t < m_joints.size(); t++)
	{
		if (m_joints[t])
		{
			if (joint == m_joints[t]->GetB2Joint())
			{
				m_joints[t]->KillJoint();
				m_joints[t].reset();
				return;
			}
		}
	}
}

std::size_t ETHPhysicsEntityController::GetNumJoints() const
{
	return m_joints.size();
}

ETHJointPtr ETHPhysicsEntityController::GetJoint(const std::size_t joindIdx)
{
	if (joindIdx < GetNumJoints())
		return m_joints[joindIdx];
	else
		return ETHJointPtr();
}
