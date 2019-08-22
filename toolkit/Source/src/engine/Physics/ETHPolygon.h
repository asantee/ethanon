#ifndef ETH_POLYGON_H_
#define ETH_POLYGON_H_

#include <Enml/Enml.h>
#include <Math/Vector2.h>

#include <vector>

#include <boost/shared_ptr.hpp>

class ETHPolygon
{
	std::vector<gs2d::math::Vector2> m_vertices;
	std::string GetEntityName(const unsigned int v) const;

public:
	ETHPolygon(const std::string& enmlString);
	bool IsValid() const;
	std::size_t GetNumPolygons() const;
	const std::vector<gs2d::math::Vector2>& GetVertices() const;
	const std::string GetENMLDeclaration() const;
	std::size_t GetNumVertices() const;
};

typedef boost::shared_ptr<ETHPolygon> ETHPolygonPtr;

#endif
