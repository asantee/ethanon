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

#ifndef ETH_POLYGON_H_
#define ETH_POLYGON_H_

#include <enml/enml.h>
#include <gs2dmath.h>
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