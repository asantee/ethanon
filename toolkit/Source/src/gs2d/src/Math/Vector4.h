#ifndef GS2D_VECTOR4_H_
#define GS2D_VECTOR4_H_

#include "Vector2.h"
#include "Vector3.h"

namespace gs2d {
namespace math {

struct Vector4
{
	Vector4();
	Vector4(const float& v);
	Vector4(const Vector4& v);
	Vector4(float vx, float vy, float vz, float vw);
	Vector4(const Vector2& v2, float vz, float vw);
	Vector4(const Vector2& a, const Vector2& b);
	Vector4(const Vector3& v3, float vw);
	Vector4 operator * (const float& v) const;
	Vector4 operator * (const Vector4& v) const;
	Vector4 operator / (const float& v) const;
	Vector4 operator / (const Vector4& v) const;
	Vector4 operator - (const float& v) const;
	Vector4 operator - (const Vector4& v) const;
	Vector4 operator + (const float& v) const;
	Vector4 operator + (const Vector4& v) const;
	bool operator == (const Vector4& v) const;
	bool operator != (const Vector4& v) const;
	Vector4& operator += (const Vector4& v);
	Vector4& operator -= (const Vector4& v);
	Vector4& operator *= (const float& v);
	Vector4& operator /= (const float& v);
	float x,y,z,w;

	static float DP4(const Vector4& a, const Vector4& b);

	typedef Vector4 Plane;

	static Plane ComputePlane(const Vector3& pos, const Vector3& normal);
	static float PlaneDotVector(const Plane& plane, const Vector3& vec);
	static Plane ComputePlane(const Vector2& pos, const Vector2& normal);
	static float PlaneDotVector(const Plane& plane, const Vector2& vec);
};

namespace constant {
const Vector4 ONE_VECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
const Vector4 ZERO_VECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
} // namespace constant

} // namespace math
} // namespace gs2d

#endif
