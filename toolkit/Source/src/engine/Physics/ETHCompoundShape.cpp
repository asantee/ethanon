#include "ETHCompoundShape.h"
#include "ETHPhysicsSimulator.h"

ETHCompoundShape::ETHCompoundShape(const std::string& enmlString) :
	m_content(enmlString)
{
	gs2d::enml::File file(enmlString);
	const unsigned int numEntities = file.GetNumEntities();
	m_entities.resize(numEntities);
	unsigned int idx = 0;
	for (std::map<std::string, gs2d::enml::Entity>::const_iterator iter = file.GetEntities().begin();
		iter != file.GetEntities().end(); ++iter)
	{
		assert(idx < numEntities);
		m_entities[idx++] = iter->second;
	}
}

const std::string& ETHCompoundShape::GetENMLDeclaration() const
{
	return m_content;
}

unsigned int ETHCompoundShape::GetNumShapes() const
{
	return static_cast<unsigned int>(m_entities.size());
}

b2Shape* ETHCompoundShape::GetShape(const unsigned int idx, const gs2d::math::Vector2& scale) const
{
	const gs2d::enml::Entity& entity = m_entities[idx];
	const ETHEntityProperties::BODY_SHAPE shapeId = ETHPhysicsSimulator::StringToShape(entity.Get(("shape")));
	if (shapeId == ETHEntityProperties::BS_BOX)
	{
		const float posX = ETHGlobal::ParseFloat(entity.Get(("posX")).c_str()) * scale.x * 0.5f;
		const float posY = ETHGlobal::ParseFloat(entity.Get(("posY")).c_str()) * scale.y * 0.5f;
		const float sizeX = ETHGlobal::ParseFloat(entity.Get(("sizeX")).c_str()) * scale.x * 0.5f;
		const float sizeY = ETHGlobal::ParseFloat(entity.Get(("sizeY")).c_str()) * scale.y * 0.5f;
		const float angle = ETHGlobal::ParseFloat(entity.Get(("angle")).c_str());
		ETHCollisionBox collision(Vector3(posX, posY, 0.0f), Vector3(sizeX, sizeY, 1.0f));
		return ETHPhysicsSimulator::GetBoxShape(collision, angle)[0];
	}
	else if (shapeId == ETHEntityProperties::BS_CIRCLE)
	{
		const float posX = ETHGlobal::ParseFloat(entity.Get(("posX")).c_str()) * scale.x * 0.5f;
		const float posY = ETHGlobal::ParseFloat(entity.Get(("posY")).c_str()) * scale.y * 0.5f;
		const float radius = ETHGlobal::ParseFloat(entity.Get(("radius")).c_str()) * (scale.x * 0.5f);
		ETHCollisionBox collision(Vector3(posX, posY, 0.0f), Vector3(radius * 2.0f, radius * 2.0f, 1.0f));
		return ETHPhysicsSimulator::GetCircleShape(collision)[0];
	}
	else
	{
		return 0;
	}
}

std::vector<b2Shape*> ETHCompoundShape::GetShapes(const gs2d::math::Vector2& scale) const
{
	std::vector<b2Shape*> r;
	for (std::size_t t = 0; t < GetNumShapes(); t++)
	{
		r.push_back(GetShape(static_cast<unsigned int>(t), scale));
	}
	return r;
}

float ETHCompoundShape::GetIndividualProperty(const unsigned int idx, const float defaultValue, const std::string& attribName) const
{
	if (idx < m_entities.size())
	{
		const gs2d::enml::Entity& entity = m_entities[idx];
		const std::string& value = entity.Get(attribName);
		if (value != (""))
		{
			return ETHGlobal::ParseFloat(entity.Get(attribName).c_str());
		}
	}
	return defaultValue;
}

float ETHCompoundShape::GetIndividualFriction(const unsigned int idx, const float defaultFriction) const
{
	return GetIndividualProperty(idx, defaultFriction, ("friction"));
}

float ETHCompoundShape::GetIndividualDensity(const unsigned int idx, const float defaultDensity) const
{
	return GetIndividualProperty(idx, defaultDensity, ("density"));
}

float ETHCompoundShape::GetIndividualRestitution(const unsigned int idx, const float defaultRestitution) const
{
	return GetIndividualProperty(idx, defaultRestitution, ("restitution"));
}
