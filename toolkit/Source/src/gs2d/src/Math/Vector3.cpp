#include "Vector3.h"

#include <math.h>

namespace gs2d {
namespace math {

float Vector3::Length() const noexcept
{
	return sqrtf(x * x + y * y + z * z);
}

float Vector3::SquaredLength() const noexcept
{
	return (x * x + y * y + z * z);
}

float Vector3::DP3(const Vector3& a, const Vector3& b)
{
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

Vector3 Vector3::cross(const Vector3& a, const Vector3& b)
{
	return Vector3((a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x));
}

Vector3 Vector3::Normalize(const Vector3& v)
{
	const float a = v.Length();
	if (a <= 0.0f)
		return Vector3(0.0f, 0.0f, 0.0f);
	const float inv = 1.0f / a;
	return Vector3(v.x * inv, v.y * inv, v.z * inv);
}

float Vector3::SquaredDistance(const Vector3& a, const Vector3& b)
{
	const Vector3 diff = b - a;
	return DP3(diff, diff);
}

float Vector3::Distance(const Vector3& a, const Vector3& b)
{
	const Vector3 diff = b - a;
	return sqrtf(DP3(diff, diff));
}

} // namespace math
} // namespace gs2d
