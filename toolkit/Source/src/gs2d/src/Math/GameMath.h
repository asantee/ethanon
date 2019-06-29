#ifndef GS2D_MATH_H_
#define GS2D_MATH_H_

#include "Vector2.h"
#include "Vector4.h"

#include <math.h>

namespace gs2d {
namespace math {

namespace constant {
const float PI = 3.141592654f;
const float PI2 = PI * 2;
const float PIb = 1.570796327f;
} // namespace constant

inline float GetAngle(const Vector2& v2)
{
	const float r = atan2f(v2.x, v2.y);
	return (r < 0) ? r + (2 * constant::PI) : r;
}

inline float RadianToDegree(const float angle)
{
	return (angle / (constant::PI * 2)) * 360.0f;
}

inline float DegreeToRadian(const float angle)
{
	return angle * (constant::PI / 180.0f);
}

inline float DP2(const Vector2& a, const Vector2& b)
{
	return ((a.x * b.x) + (a.y * b.y));
}

inline float DP3(const Vector3& a, const Vector3& b)
{
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

inline float DP4(const Vector4& a, const Vector4& b)
{
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
}

inline Vector3 cross(const Vector3& v0, const Vector3& v1)
{
	return Vector3((v0.y * v1.z) - (v0.z * v1.y), (v0.z * v1.x) - (v0.x * v1.z), (v0.x * v1.y) - (v0.y * v1.x));
}

inline Vector3 Normalize(const Vector3& v0)
{
	const float a = sqrtf(DP3(v0, v0));
	if (a <= 0.0f)
		return Vector3(0,0,0);
	return Vector3(v0.x / a, v0.y / a, v0.z / a);
}

inline Vector2 Normalize(const Vector2& v0)
{
	const float a = sqrtf(DP2(v0, v0));
	if (a <= 0.0f)
		return Vector2(0,0);
	return Vector2(v0.x / a, v0.y / a);
}

inline float SquaredDistance(const Vector3& P0, const Vector3& P1)
{
	const Vector3 diff = P1 - P0;
	return DP3(diff, diff);
}

inline float SquaredDistance(const Vector2& P0, const Vector2& P1)
{
	const Vector2 diff = P1 - P0;
	return DP2(diff, diff);
}

inline float Distance(const Vector3& P0, const Vector3& P1)
{
	const Vector3 diff = P1 - P0;
	return sqrtf(DP3(diff, diff));
}

inline float Distance(const Vector2& P0, const Vector2& P1)
{
	const Vector2 diff = P1 - P0;
	return sqrtf(DP2(diff, diff));
}

template <class T>
inline T Max(const T &a, const T &b)
{
	return (a > b) ? a : b;
}

template <class T>
inline T Min(const T &a, const T &b)
{
	return (a < b) ? a : b;
}

template <class T>
inline T Abs(const T &n)
{
	return (n < 0) ? -n : n;
}

template <class T>
inline T Clamp(const T &n, const T &min, const T &max)
{
	return (n < min) ? min : (n > max) ? max : n;
}

template <class T>
inline T Square(const T &a)
{
	return a*a;
}

template <class T>
inline T Sign(const T &n)
{
	return (n < static_cast<T>(0)) ? static_cast<T>(-1) : (n > static_cast<T>(0)) ? static_cast<T>(1) : static_cast<T>(0);
}

inline Vector2 Vector2Min(const Vector2& a, const Vector2& b)
{
    return Vector2(Min(a.x, b.x), Min(a.y, b.y));
}

inline Vector2 Vector2Max(const Vector2& a, const Vector2& b)
{
    return Vector2(Max(a.x, b.x), Max(a.y, b.y));
}

typedef Vector4 Plane;
inline Plane ComputePlane(const Vector3& pos, const Vector3& normal)
{
	return (Plane(Normalize(normal), -((normal.x * pos.x) + (normal.y * pos.y) + (normal.z * pos.z))));
}

inline Plane ComputePlane(const Vector2& pos, const Vector2& normal)
{
	return (Plane(Normalize(normal), 0, -((normal.x * pos.x) + (normal.y * pos.y))));
}

inline float PlaneDotVector(const Plane& plane, const Vector3& vec)
{
	return (plane.x * vec.x) + (plane.y * vec.y) + (plane.z * vec.z) + (plane.w);
}

inline float PlaneDotVector(const Plane& plane, const Vector2& vec)
{
	return (plane.x * vec.x) + (plane.y * vec.y) + (plane.w);
}

} // namespace math
} // namespace gs2d

#endif
