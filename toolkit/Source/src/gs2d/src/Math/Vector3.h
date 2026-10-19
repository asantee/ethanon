#ifndef GS2D_VECTOR3_H_
#define GS2D_VECTOR3_H_

#include "Vector2.h"

namespace gs2d {
namespace math {

struct PODVector3
{
	constexpr PODVector3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
	constexpr explicit PODVector3(const float v) noexcept : x(v), y(v), z(v) {}
	constexpr PODVector3(const float vx, const float vy, const float vz) noexcept : x(vx), y(vy), z(vz) {}

	constexpr PODVector3(const PODVector3&) noexcept = default;
	PODVector3& operator = (const PODVector3&) noexcept = default;
	PODVector3(PODVector3&&) noexcept = default;
	PODVector3& operator = (PODVector3&&) noexcept = default;

	float x, y, z;
};

// Operators are plain C++ defined here so they inline (see Vector2.h)
struct Vector3 : public PODVector3
{
	constexpr Vector3() noexcept : PODVector3() {}
	constexpr explicit  Vector3(const float v) noexcept : PODVector3(v) {}
	constexpr Vector3(const Vector3& v) noexcept : PODVector3(v.x, v.y, v.z) {}
	constexpr Vector3(const float vx, const float vy, const float vz) noexcept : PODVector3(vx, vy, vz) {}
	constexpr Vector3(const Vector2 &v2, const float vz) noexcept : PODVector3(v2.x, v2.y, vz) {}
	float Length() const noexcept;
	float SquaredLength() const noexcept;
	Vector3 operator * (const float v) const noexcept { return Vector3(x * v, y * v, z * v); }
	Vector3 operator * (const Vector3& v) const noexcept { return Vector3(x * v.x, y * v.y, z * v.z); }
	Vector3 operator / (const float v) const noexcept { return Vector3(x / v, y / v, z / v); }
	Vector3 operator / (const Vector3& v) const noexcept { return Vector3(x / v.x, y / v.y, z / v.z); }
	Vector3 operator - (const float v) const noexcept { return Vector3(x - v, y - v, z - v); }
	Vector3 operator - (const Vector3& v) const noexcept { return Vector3(x - v.x, y - v.y, z - v.z); }
	Vector3 operator + (const float v) const noexcept { return Vector3(x + v, y + v, z + v); }
	Vector3 operator + (const Vector3& v) const noexcept { return Vector3(x + v.x, y + v.y, z + v.z); }
	bool operator == (const Vector3& v) const noexcept { return (x == v.x && y == v.y && z == v.z); }
	bool operator != (const Vector3& v) const noexcept { return (x != v.x || y != v.y || z != v.z); }
	Vector3& operator += (const Vector3& v) noexcept { x += v.x; y += v.y; z += v.z; return *this; }
	Vector3& operator -= (const Vector3& v) noexcept { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3& operator *= (const float v) noexcept { x *= v; y *= v; z *= v; return *this; }
	Vector3& operator /= (const float v) noexcept { x /= v; y /= v; z /= v; return *this; }

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
