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

#ifndef ETH_COMPOUND_SHAPE_H_
#define ETH_COMPOUND_SHAPE_H_

#include <Box2D/Box2D.h>
#include <gs2dmath.h>
#include <vector>
#include <enml/enml.h>

class ETHCompoundShape
{
public:
	ETHCompoundShape(const gs2d::str_type::string& enmlString);
	unsigned int GetNumShapes() const;
	const gs2d::str_type::string& GetENMLDeclaration() const;
	std::vector<b2Shape*> GetShapes(const gs2d::math::Vector2& scale) const;

private:
	b2Shape* GetShape(const unsigned int idx, const gs2d::math::Vector2& scale) const;
	gs2d::str_type::string m_content;
	std::vector<gs2d::enml::Entity> m_entities;
};

typedef boost::shared_ptr<ETHCompoundShape> ETHCompoundShapePtr;

#endif
