#include "ETHPolygon.h"

ETHPolygon::ETHPolygon(const gs2d::str_type::string& enmlString)
{
	gs2d::enml::File file(enmlString);
	gs2d::enml::Entity* entity;
	unsigned int v = 0;
	while ((entity  = file.GetEntity(GetEntityName(v))) != 0)
	{
		const gs2d::str_type::string& Xvalue = entity->Get(GS_L("x"));
		const gs2d::str_type::string& Yvalue = entity->Get(GS_L("y"));
		float x = 0.0f, y = 0.0f;
		GS2D_SSCANF(Xvalue.c_str(), GS_L("%f"), &x);
		GS2D_SSCANF(Yvalue.c_str(), GS_L("%f"), &y);
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

gs2d::str_type::string ETHPolygon::GetEntityName(const unsigned int v) const
{
	gs2d::str_type::stringstream ss; ss << GS_L("v") << v;
	return ss.str();
}

const gs2d::str_type::string ETHPolygon::GetENMLDeclaration() const
{
	gs2d::str_type::stringstream ss;
	if (!IsValid())
		ss << GS_L("/* warning: invalid polygon */ ");
	for (std::size_t t = 0; t < m_vertices.size(); t++)
	{
		ss << GS_L("v") << t << GS_L("{")
			<< GS_L("x=") << m_vertices[t].x << GS_L(";y=") << m_vertices[t].y
			<< GS_L(";} ");
	}
	return ss.str();
}
