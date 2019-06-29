#ifndef GS2D_VECTOR3_H_
#define GS2D_VECTOR3_H_

#include "Vector2.h"

namespace gs2d {
namespace math {

struct PODVector3 { float x, y, z; };

struct Vector3 : public PODVector3
{
	Vector3();
	Vector3(const float v);
	Vector3(const Vector3& v);
	Vector3(const float vx, const float vy, const float vz);
	Vector3(const Vector2 &v2, const float vz);
	float Length() const;
	float SquaredLength() const;
	Vector3 operator * (const float v) const;
	Vector3 operator * (const Vector3& v) const;
	Vector3 operator / (const float v) const;
	Vector3 operator / (const Vector3& v) const;
	Vector3 operator - (const float v) const;
	Vector3 operator - (const Vector3& v) const;
	Vector3 operator + (const float v) const;
	Vector3 operator + (const Vector3& v) const;
	bool operator == (const Vector3& v) const;
	bool operator != (const Vector3& v) const;
	Vector3& operator += (const Vector3& v);
	Vector3& operator -= (const Vector3& v);
	Vector3& operator *= (const float v);
	Vector3& operator /= (const float v);

	static float DP3(const Vector3& a, const Vector3& b);
	static Vector3 cross(const Vector3& a, const Vector3& b);
	static Vector3 Normalize(const Vector3& v);
	static float SquaredDistance(const Vector3& a, const Vector3& b);
	static float Distance(const Vector3& a, const Vector3& b);
};

namespace constant {
const Vector3 HALF_VECTOR3(0.5f, 0.5f, 0.5f);
const Vector3 ZERO_VECTOR3(0.0f, 0.0f, 0.0f);
const Vector3 ONE_VECTOR3(1.0f, 1.0f, 1.0f);
} // namespace constant

} // namespace math
} // namespace gs2d

#endif
