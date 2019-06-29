#ifndef ETH_POLYGON_H_
#define ETH_POLYGON_H_

#include <Enml/Enml.h>
#include <Math/Vector2.h>
#include <vector>

class ETHPolygon
{
	std::vector<gs2d::math::Vector2> m_vertices;
	gs2d::str_type::string GetEntityName(const unsigned int v) const;

public:
	ETHPolygon(const gs2d::str_type::string& enmlString);
	bool IsValid() const;
	std::size_t GetNumPolygons() const;
	const std::vector<gs2d::math::Vector2>& GetVertices() const;
	const gs2d::str_type::string GetENMLDeclaration() const;
	std::size_t GetNumVertices() const;
};

typedef boost::shared_ptr<ETHPolygon> ETHPolygonPtr;

#endif
