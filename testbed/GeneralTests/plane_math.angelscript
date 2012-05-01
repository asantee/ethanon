class Plane
{
	Plane()
	{
		x = y = z = w = 0.0f;
	}
	Plane(const float _x, const float _y, const float _z, const float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	Plane(const vector3 v3, const float _w)
	{
		x = v3.x;
		y = v3.y;
		z = v3.z;
		w = _w;
	}
	Plane(const vector2 v2, const float _z, const float _w)
	{
		x = v2.x;
		y = v2.y;
		z = _z;
		w = _w;
	}
	float x;
	float y;
	float z;
	float w;
}

Plane computePlane(const vector3 pos, const vector3 normal)
{
	return (Plane(normalize(normal), -((normal.x * pos.x) + (normal.y * pos.y) + (normal.z * pos.z))));
}

Plane computePlane(const vector2 pos, const vector2 normal)
{
	return (Plane(normalize(normal), 0, -((normal.x * pos.x) + (normal.y * pos.y))));
}

float planeDotVector(const Plane plane, const vector3 vec)
{
	return (plane.x * vec.x) + (plane.y * vec.y) + (plane.z * vec.z) + (plane.w);
}

float planeDotVector(const Plane plane, const vector2 vec)
{
	return (plane.x * vec.x) + (plane.y * vec.y) + (plane.w);
}

const float HIGH_VALUE = 99999.0f;

class Shape
{
	Plane[] m_planes;

	Shape(const vector2 rectPos, const vector2 rectSize, const float angle, const bool edgePlanes = false)
	{
		vector2 n0(0.0f, 1.0f);
		vector2 n1(0.0f,-1.0f);
		vector2 n2( 1.0f, 0.0f);
		vector2 n3(-1.0f, 0.0f);

		const vector2 halfRectSize = rectSize/2.0f;
		vector2 p0(vector2(0.0f,-halfRectSize.y));
		vector2 p1(vector2(0.0f, halfRectSize.y));
		vector2 p2(vector2(-halfRectSize.x, 0.0f));
		vector2 p3(vector2( halfRectSize.x, 0.0f));

		if (angle != 0.0f)
		{
			matrix4x4 rot = rotateZ(degreeToRadian(angle));
			n0 = toVector2(multiply(vector3(n0, 0), rot));
			n1 = toVector2(multiply(vector3(n1, 0), rot));
			n2 = toVector2(multiply(vector3(n2, 0), rot));
			n3 = toVector2(multiply(vector3(n3, 0), rot));
			p0 = toVector2(multiply(vector3(p0, 0), rot));
			p1 = toVector2(multiply(vector3(p1, 0), rot));
			p2 = toVector2(multiply(vector3(p2, 0), rot));
			p3 = toVector2(multiply(vector3(p3, 0), rot));
		}
		m_planes.resize(edgePlanes ? 8 : 4);
		int idx = 0;

		m_planes[idx++] = computePlane(rectPos+p0, n0);
		m_planes[idx++] = computePlane(rectPos+p1, n1);
		m_planes[idx++] = computePlane(rectPos+p2, n2);
		m_planes[idx++] = computePlane(rectPos+p3, n3);

		if (edgePlanes)
		{
			m_planes[idx++] = (computePlane(rectPos+vector2(halfRectSize.x, halfRectSize.y), normalize(vector2(-1.0f, -1.0f))));
			m_planes[idx++] = (computePlane(rectPos+vector2(-halfRectSize.x, halfRectSize.y), normalize(vector2(1.0f, -1.0f))));
			m_planes[idx++] = (computePlane(rectPos+vector2(halfRectSize.x, -halfRectSize.y), normalize(vector2(-1.0f, 1.0f))));
			m_planes[idx++] = (computePlane(rectPos+vector2(-halfRectSize.x, -halfRectSize.y), normalize(vector2(1.0f, 1.0f))));
		}
	}

	bool intersectSphere(const vector2 pos, const float radius)
	{
		const uint len = m_planes.length();
		for (uint t = 0; t < len; t++)
		{
			if (abs(planeDotVector(m_planes[t], pos)) < radius)
			{
				return true;
			}
		}
		return false;
	}

	bool overlapPoint(const vector2 pos)
	{
		const uint len = m_planes.length();
		for (uint t = 0; t < len; t++)
		{
			if (planeDotVector(m_planes[t], pos) < 0.0f)
			{
				return false;
			}
		}
		return true;
	}

	bool overlapSphere(const vector2 pos, const float radius, float &out penetrationDepth, vector2 &out hitNormal)
	{
		float lowerDotValue = HIGH_VALUE;
		vector2 lowerHitNormal;
		const uint len = m_planes.length();
		for (uint t = 0; t < len; t++)
		{
			const float dotValue = planeDotVector(m_planes[t], pos);
			if (lowerDotValue > dotValue)
			{
				lowerDotValue = dotValue;
				lowerHitNormal = vector2(m_planes[t].x, m_planes[t].y);
			}
			if (dotValue <= -radius)
			{
				return false;
			}
			else if (dotValue >= -radius && dotValue < radius)
			{
				penetrationDepth = dotValue + radius;
				hitNormal = vector2(m_planes[t].x, m_planes[t].y);
			}
		}
		penetrationDepth = lowerDotValue + radius;
		hitNormal = lowerHitNormal;
		return true;
	}

	/*void DrawPlanes(const uint color = 0xFFFFFFFF)
	{
		const uint len = m_planes.length();
		for (uint t = 0; t < len; t++)
		{
			const vector2 normal(m_planes[t].x, m_planes[t].y);
			const float angle = getAngle(normal);
			//DrawRectangle(normal *-m_planes[t].w, vector2(2000, 4), color, color, color, color, radianToDegree(angle));
		}
	}*/
}
