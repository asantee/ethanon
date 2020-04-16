#include "ETHPolygon.h"

#include <string>
#include <sstream>

#ifdef _MSC_VER
  #define GS2D_SSCANF sscanf_s
#else
  #define GS2D_SSCANF sscanf
#endif

ETHPolygon::ETHPolygon(const std::string& enmlString)
{
	gs2d::enml::File file(enmlString);
	gs2d::enml::Entity* entity;
	unsigned int v = 0;
	while ((entity  = file.GetEntity(GetEntityName(v))) != 0)
	{
		const std::string& Xvalue = entity->Get(("x"));
		const std::string& Yvalue = entity->Get(("y"));
		float x = 0.0f, y = 0.0f;
		GS2D_SSCANF(Xvalue.c_str(), ("%f"), &x);
		GS2D_SSCANF(Yvalue.c_str(), ("%f"), &y);
		m_vertices.push_back(gs2d::math::Vector2(x, y));
		++v;
	}
}

bool ETHPolygon::IsValid() const
{
	return (m_vertices.size() >= 3);
}

std::size_t ETHPolygon::GetNumPolygons() const
{
	return m_vertices.size() - 2;
}

std::size_t ETHPolygon::GetNumVertices() const
{
	return m_vertices.size();
}

const std::vector<gs2d::math::Vector2>& ETHPolygon::GetVertices() const
{
	return m_vertices;
}

std::string ETHPolygon::GetEntityName(const unsigned int v) const
{
	std::stringstream ss; ss << ("v") << v;
	return ss.str();
}

const std::string ETHPolygon::GetENMLDeclaration() const
{
	std::stringstream ss;
	if (!IsValid())
		ss << ("/* warning: invalid polygon */ ");
	for (std::size_t t = 0; t < m_vertices.size(); t++)
	{
		ss << ("v") << t << ("{")
			<< ("x=") << m_vertices[t].x << (";y=") << m_vertices[t].y
			<< (";} ");
	}
	return ss.str();
}
