#include "Vector2.h"

#include "GameMath.h"

#if defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

#include <math.h>

namespace gs2d {
namespace math {

#if defined(__ARM_NEON__)
	float sqrtest(float a)
	{
		// need to "transfer" or "convert" the scalar input
		// to a vector of two
		// - optimally we would not need an instruction for that
		// but we would just let the processor calculate the instruction
		// for all the lanes in the register
		float32x2_t a2 = vdup_n_f32(a);

		// next we create a mask that is all ones for the legal
		// domain of 1/sqrt(x)
		auto is_legal = vreinterpret_f32_u32(vcgt_f32(a2, vdup_n_f32(0.0f)));

		// calculate two reciprocal estimates in parallel
		float32x2_t a2est = vrsqrte_f32(a2);

		// we need to mask the result, so that effectively
		// all non-legal values of a2est are zeroed
		a2est = vand_u32(is_legal, a2est);

		// x * 1/sqrt(x) == sqrt(x)
		a2 = vmul_f32(a2, a2est);

		// finally we get only the zero lane of the result
		// discarding the other half
		return vget_lane_f32(a2, 0);
	}
#endif

Vector2::Vector2()
{
	x = y = 0.0f;
}

Vector2::Vector2(const float& v)
{
	x = y = v;
}

Vector2::Vector2(const Vector2& v)
{
	*this = v;
}

Vector2::Vector2(const float& vx, const float& vy)
{
	x = vx;
	y = vy;
}

float Vector2::Length() const
{
	#if defined(__ARM_NEON__)
		return sqrtest(SquaredLength());
	#else
		return sqrtf(SquaredLength());
	#endif
}

float Vector2::SquaredLength() const
{
	#if defined(__ARM_NEON__)
		float32x2_t v = vmul_f32(*(float32x2_t*)&x, *(float32x2_t *)&x);
		v = vpadd_f32(v, v);
		return (vget_lane_f32(v, 0));
	#else
		return ((x * x) + (y * y));
	#endif
}

Vector2 Vector2::operator * (const float v) const
{
	#if defined(__ARM_NEON__)
		float32x2_t r = vmul_f32(*(float32x2_t*)&x, vdup_n_f32((float32_t)v));
		return *(Vector2*)&r;
	#else
		return Vector2(x * v, y * v);
	#endif
}

Vector2 Vector2::operator * (const Vector2& v) const
{
	#if defined(__ARM_NEON__)
		float32x2_t r = vmul_f32(*(float32x2_t*)&x, *(float32x2_t*)&v);
		return *(Vector2*)&r;
	#else
		return Vector2(x * v.x, y * v.y);
	#endif
}

Vector2 Vector2::operator / (const float v) const
{
	#if defined(__ARM_NEON__)
		float32x2_t values = vdup_n_f32((float32_t)v);
		float32x2_t estimate = vrecpe_f32(values);
		estimate = vmul_f32(vrecps_f32(values, estimate), estimate);
		estimate = vmul_f32(vrecps_f32(values, estimate), estimate);
		float32x2_t r = vmul_f32(*(float32x2_t *)&x, estimate);
		return *(Vector2*)&r;
	#else
		return Vector2(x / v, y / v);
	#endif
}

Vector2 Vector2::operator / (const Vector2& v) const
{
	#if defined(__ARM_NEON__)
		float32x2_t *vLeft = (float32x2_t *)&x;
		float32x2_t *vRight = (float32x2_t *)&v;
		float32x2_t estimate = vrecpe_f32(*vRight);
		estimate = vmul_f32(vrecps_f32(*vRight, estimate), estimate);
		estimate = vmul_f32(vrecps_f32(*vRight, estimate), estimate);
		float32x2_t r = vmul_f32(*vLeft, estimate);
		return *(Vector2*)&r;
	#else
		return Vector2(x / v.x, y / v.y);
	#endif
}

Vector2 Vector2::operator - (const float v) const
{
	#if defined(__ARM_NEON__)
		float32x2_t r = vsub_f32(*(float32x2_t *)&x, vdup_n_f32((float32_t)v));
		return *(Vector2*)&r;
	#else
		return Vector2(x - v, y - v);
	#endif
}

Vector2 Vector2::operator - (const Vector2& v) const
{
	#if defined(__ARM_NEON__)
		float32x2_t r = vsub_f32(*(float32x2_t*)&x, *(float32x2_t*)&v);
		return *(Vector2*)&r;
	#else
		return Vector2(x - v.x, y - v.y);
	#endif
}

Vector2 Vector2::operator + (const float v) const
{
	#if defined(__ARM_NEON__)
		float32x2_t r = vadd_f32(*(float32x2_t *)&x, vdup_n_f32((float32_t)v));
		return *(Vector2*)&r;
	#else
		return Vector2(x + v, y + v);
	#endif
}

Vector2 Vector2::operator + (const Vector2& v) const
{
	#if defined(__ARM_NEON__)
		float32x2_t r = vadd_f32(*(float32x2_t*)&x, *(float32x2_t*)&v);
		return *(Vector2*)&r;
	#else
		return Vector2(x + v.x, y + v.y);
	#endif
}

bool Vector2::operator == (const Vector2& v) const
{
	#if defined(__ARM_NEON_)
		float32x2_t v1 = *(float32x2_t *)&x;
		float32x2_t v2 = *(float32x2_t *)&v;
		uint32x2_t vCmp = vceq_f32(v1, v2);
		uint32x2_t vAnd = vand_u32(vCmp, vext_u32(vCmp, vCmp, 1));
		vAnd = vand_u32(vAnd, vdup_n_u32(1));
		return (bool)vget_lane_u32(vAnd, 0);
	#else
		return (x == v.x && y == v.y);
	#endif
}

bool Vector2::operator != (const Vector2& v) const
{
	return !(*this == v);
}

Vector2& Vector2::operator += (const Vector2& v)
{
	#if defined(__ARM_NEON__)
		*(float32x2_t*)this = vadd_f32(*(float32x2_t*)&x, *(float32x2_t*)&v);
		return *this;
	#else
		*this = Vector2(x, y) + v;
		return *this;
	#endif
}

Vector2& Vector2::operator -= (const Vector2& v)
{
	#if defined(__ARM_NEON__)
		*(float32x2_t*)this = vsub_f32(*(float32x2_t*)&x, *(float32x2_t*)&v);
		return *this;
	#else
		*this = Vector2(x, y) - v;
		return *this;
	#endif
}

Vector2& Vector2::operator *= (const float f)
{
	#if defined(__ARM_NEON__)
		*(float32x2_t*)this = vmul_f32(*(float32x2_t*)&x, vdup_n_f32((float32_t)f));
		return *this;
	#else
		*this = Vector2(x, y) * f;
		return *this;
	#endif
}

Vector2& Vector2::operator /= (const float f)
{
	#if defined(__ARM_NEON__)
		float32x2_t values = vdup_n_f32((float32_t)f);
		float32x2_t estimate = vrecpe_f32(values);
		estimate = vmul_f32(vrecps_f32(values, estimate), estimate);
		estimate = vmul_f32(vrecps_f32(values, estimate), estimate);
		*(float32x2_t*)this = vmul_f32(*(float32x2_t *)&x, estimate);
		return *this;
	#else
		*this = Vector2(x, y) / f;
		return *this;
	#endif
}

Vector2i Vector2::ToVector2i() const
{
	return Vector2i(static_cast<int>(x), static_cast<int>(y));
}

Vector2 Vector2::ToVector2(const Vector2i& v)
{
	return Vector2(static_cast<float>(v.x), static_cast<float>(v.y));
}

float Vector2::Distance(const Vector2& a, const Vector2& b)
{
	const Vector2 diff = b - a;
	return diff.Length();
}

float Vector2::GetAngle(const Vector2& v2)
{
	const float r = atan2f(v2.x, v2.y);
	return (r < 0) ? r + (2 * constant::PI) : r;
}

float Vector2::DP2(const Vector2& a, const Vector2& b)
{
	#if defined(__ARM_NEON__)
		float32x2_t v = vmul_f32(*(float32x2_t*)&a, *(float32x2_t *)&b);
		v = vpadd_f32(v, v);
		return (vget_lane_f32(v, 0));
	#else
	    return ((a.x * b.x) + (a.y * b.y));
	#endif
}

Vector2 Vector2::Normalize(const Vector2& v)
{
    const float a = v.Length();
    if (a <= 0.0f)
        return Vector2(0, 0);
    return (v / a);
}

Vector2 Vector2::Vector2Min(const Vector2& a, const Vector2& b)
{
    return Vector2(Min(a.x, b.x), Min(a.y, b.y));
}

Vector2 Vector2::Vector2Max(const Vector2& a, const Vector2& b)
{
    return Vector2(Max(a.x, b.x), Max(a.y, b.y));
}

float Vector2::SquaredDistance(const Vector2& a, const Vector2& b)
{
    const Vector2 diff = b - a;
    return DP2(diff, diff);
}

} // namespace math
} // namespace gs2d
