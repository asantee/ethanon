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

#include "OrientedBoundingBox.h"

namespace gs2d {
namespace math {

OrientedBoundingBox::OrientedBoundingBox(const Vector2& center, const Vector2& size, const float angle)
{
	Vector2 x( cosf(angle), sinf(angle));
	Vector2 y(-sinf(angle), cosf(angle));

	x *= size.x / 2;
	y *= size.y / 2;

	corner[0] = center - x - y;
	corner[1] = center + x - y;
	corner[2] = center + x + y;
	corner[3] = center - x + y;

	ComputeAxes();
}

bool OrientedBoundingBox::Overlaps(const OrientedBoundingBox& other) const
{
	return Overlaps1Way(other) && other.Overlaps1Way(*this);
}

bool OrientedBoundingBox::Overlaps1Way(const OrientedBoundingBox& other) const
{
	for (std::size_t a = 0; a < 2; ++a)
	{

		float t = DP2(other.corner[0], axis[a]);

		float tMin = t;
		float tMax = t;

		for (std::size_t c = 1; c < 4; ++c)
		{
			t = DP2(other.corner[c], axis[a]);

			if (t < tMin)
			{
				tMin = t;
			}
			else if (t > tMax)
			{
				tMax = t;
			}
		}

		if ((tMin > 1 + origin[a]) || (tMax < origin[a]))
		{
			return false;
		}
	}
	return true;
}

void OrientedBoundingBox::ComputeAxes()
{
	axis[0] = corner[1] - corner[0]; 
	axis[1] = corner[3] - corner[0]; 

	for (std::size_t a = 0; a < 2; ++a)
	{
		axis[a] /= axis[a].SquaredLength();
		origin[a] = DP2(corner[0], axis[a]);
	}
}

} // math
} // gs2d
