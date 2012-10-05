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

#ifndef GS2D_ORIENTED_BOUNDING_BOX_H_
#define GS2D_ORIENTED_BOUNDING_BOX_H_

#include "Math.h"

namespace gs2d {
namespace math {

/**
 * \brief Describes an oriented bounding box
 *
 * This class stores and compares two boxes for oriented BB overlapping.
 * The code was originally written by Morgan McGuire and posted in this
 * thread: http://www.flipcode.com/archives/2D_OBB_Intersection.shtml
 */
class OrientedBoundingBox
{
public:
	OrientedBoundingBox(const Vector2& center, const Vector2& size, const float angle);
	bool Overlaps(const OrientedBoundingBox& other) const;

private:
	Vector2 corner[4];
	Vector2 axis[2];
	float origin[2];

	bool Overlaps1Way(const OrientedBoundingBox& other) const;
	void ComputeAxes();
};

typedef boost::shared_ptr<gs2d::math::OrientedBoundingBox> OrientedBoundingBoxPtr;

} // math
} // gs2d

#endif
