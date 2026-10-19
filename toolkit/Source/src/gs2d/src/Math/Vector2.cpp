#include "Vector2.h"

#include "GameMath.h"

#include <climits>
#include <math.h>

namespace gs2d {
namespace math {

float Vector2::Length() const noexcept
{
	return sqrtf(SquaredLength());
}

// Converting a float to int is undefined for NaN and for values outside int's range,
// and platforms disagree on the result, so map NaN to 0 and clamp the rest
static int FloatToInt(const float f) noexcept
{
	if (f != f)
		return 0;
	if (f >= 2147483648.0f)
		return INT_MAX;
	if (f < -2147483648.0f)
		return INT_MIN;
	return static_cast<int>(f);
}

Vector2i Vector2::ToVector2i() const noexcept
{
	return Vector2i(FloatToInt(x), FloatToInt(y));
}

Vector2 Vector2::ToVector2(const Vector2i& v) noexcept
{
	return Vector2(static_cast<float>(v.x), static_cast<float>(v.y));
}

float Vector2::Distance(const Vector2& a, const Vector2& b) noexcept
{
	const Vector2 diff = b - a;
	return diff.Length();
}

float Vector2::GetAngle(const Vector2& v2) noexcept
{
	const float r = atan2f(v2.x, v2.y);
	return (r < 0) ? r + (2 * constant::PI) : r;
}

Vector2 Vector2::Normalize(const Vector2& v) noexcept
{
	const float a = v.Length();
	if (a <= 0.0f)
		return Vector2(0.0f, 0.0f);
	const float inv = 1.0f / a;
	return Vector2(v.x * inv, v.y * inv);
}

Vector2 Vector2::Vector2Min(const Vector2& a, const Vector2& b) noexcept
{
    return Vector2(Min(a.x, b.x), Min(a.y, b.y));
}

Vector2 Vector2::Vector2Max(const Vector2& a, const Vector2& b) noexcept
{
    return Vector2(Max(a.x, b.x), Max(a.y, b.y));
}

float Vector2::SquaredDistance(const Vector2& a, const Vector2& b) noexcept
{
    const Vector2 diff = b - a;
    return DP2(diff, diff);
}

} // namespace math
} // namespace gs2d
