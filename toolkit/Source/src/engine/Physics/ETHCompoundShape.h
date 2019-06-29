#ifndef ETH_COMPOUND_SHAPE_H_
#define ETH_COMPOUND_SHAPE_H_

#include <Math/Vector2.h>

#include <Box2D/Box2D.h>

#include <Math/GameMath.h>
#include <Enml/Enml.h>

#include <vector>

class ETHCompoundShape
{
public:
	ETHCompoundShape(const gs2d::str_type::string& enmlString);
	unsigned int GetNumShapes() const;
	const gs2d::str_type::string& GetENMLDeclaration() const;
	std::vector<b2Shape*> GetShapes(const gs2d::math::Vector2& scale) const;
	float GetIndividualFriction(const unsigned int idx, const float defaultFriction) const;
	float GetIndividualDensity(const unsigned int idx, const float defaultDensity) const;
	float GetIndividualRestitution(const unsigned int idx, const float defaultRestitution) const;
	float GetIndividualProperty(const unsigned int idx, const float defaultValue, const gs2d::str_type::string& attribName) const;

private:
	b2Shape* GetShape(const unsigned int idx, const gs2d::math::Vector2& scale) const;
	gs2d::str_type::string m_content;
	std::vector<gs2d::enml::Entity> m_entities;
};

typedef boost::shared_ptr<ETHCompoundShape> ETHCompoundShapePtr;

#endif
