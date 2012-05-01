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

#ifndef GS2D_UTIL_H_
#define GS2D_UTIL_H_

#include "gs2d.h"

namespace gs2d {
namespace math {

static const float HIGH_VALUE = 99999.0f;

class Shape
{
public:
	inline Shape(const std::vector<Plane> &planes)
	{
		m_planes = planes;
	}

	inline Shape(const Vector2& rectPos, const Vector2& rectSize, const bool edgePlanes = false)
	{
		m_planes.resize(edgePlanes ? 8 : 4);
		int idx = 0;

		const Vector2 halfRectSize = rectSize/2.0f;
		m_planes[idx++] = (ComputePlane(rectPos+Vector2(0.0f,-halfRectSize.y), Vector2(0.0f, 1.0f)));
		m_planes[idx++] = (ComputePlane(rectPos+Vector2(0.0f, halfRectSize.y), Vector2(0.0f,-1.0f)));
		m_planes[idx++] = (ComputePlane(rectPos+Vector2(-halfRectSize.x, 0.0f), Vector2( 1.0f, 0.0f)));
		m_planes[idx++] = (ComputePlane(rectPos+Vector2( halfRectSize.x, 0.0f), Vector2(-1.0f, 0.0f)));

		if (edgePlanes)
		{
			m_planes[idx++] = (ComputePlane(rectPos+Vector2(halfRectSize.x, halfRectSize.y), Normalize(Vector2(-1.0f, -1.0f))));
			m_planes[idx++] = (ComputePlane(rectPos+Vector2(-halfRectSize.x, halfRectSize.y), Normalize(Vector2(1.0f, -1.0f))));
			m_planes[idx++] = (ComputePlane(rectPos+Vector2(halfRectSize.x, -halfRectSize.y), Normalize(Vector2(-1.0f, 1.0f))));
			m_planes[idx++] = (ComputePlane(rectPos+Vector2(-halfRectSize.x, -halfRectSize.y), Normalize(Vector2(1.0f, 1.0f))));
		}
	}

	inline bool IntersectSphere(const Vector2& pos, const float radius)
	{
		for (std::vector<Plane>::iterator iter = m_planes.begin(); iter != m_planes.end(); iter++)
		{
			if (Abs(PlaneDotVector(*iter, pos)) < radius)
			{
				return true;
			}
		}
		return false;
	}

	inline bool OverlapSphere(const Vector2& pos, const float radius, float *penetrationDepth = 0, Vector2 *hitNormal = 0)
	{
		float lowerDotValue = HIGH_VALUE;
		Vector2 lowerHitNormal;
		for (std::vector<Plane>::iterator iter = m_planes.begin(); iter != m_planes.end(); iter++)
		{
			const float dotValue = PlaneDotVector(*iter, pos);
			if (lowerDotValue > dotValue)
			{
				lowerDotValue = dotValue;
				lowerHitNormal = Vector2(iter->x, iter->y);
			}
			if (dotValue <= -radius)
			{
				return false;
			}
			else if (dotValue >= -radius && dotValue < radius)
			{
				if (penetrationDepth)
					*penetrationDepth = dotValue + radius;
				if (hitNormal)
					*hitNormal = Vector2(iter->x, iter->y);
			}
		}
		if (penetrationDepth)
			*penetrationDepth = lowerDotValue + radius;
		if (hitNormal)
			*hitNormal = lowerHitNormal;
		return true;
	}

	inline void DrawPlanes(VideoPtr video, const GS_COLOR color = GS_WHITE)
	{
		for (std::vector<Plane>::iterator iter = m_planes.begin(); iter != m_planes.end(); iter++)
		{
			const Vector2 normal(iter->x, iter->y);
			const float angle = GetAngle(normal);
			video->DrawRectangle(normal *-iter->w, Vector2(2000, 4), color, RadianToDegree(angle), GSEO_CENTER);
		}
	}

private:
	std::vector<Plane> m_planes;
};

} // namespace math

} // namespace gs2d

#endif