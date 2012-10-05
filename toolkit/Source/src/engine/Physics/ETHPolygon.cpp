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
		ENML_SSCANF(Xvalue.c_str(), GS_L("%f"), &x);
		ENML_SSCANF(Yvalue.c_str(), GS_L("%f"), &y);
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
