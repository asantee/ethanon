#include "Vector2.h"

#if defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

#include <math.h>

namespace gs2d {
namespace math {

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
	return sqrtf(SquaredLength());
}

float Vector2::SquaredLength() const
{
	#if defined(__ARM_NEON__)
		float32x2_t v = vmul_f32(*(float32x2_t*)&x, *(float32x2_t *)&x);
		v = vpadd_f32(v, v);
		return (vget_lane_f32(v, 0));
	#else
		return (x * x + y * y);
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
	*this = Vector2(x, y) + v;
	return *this;
}

Vector2& Vector2::operator -= (const Vector2& v)
{
	*this = Vector2(x, y) - v;
	return *this;
}

Vector2& Vector2::operator *= (const float f)
{
	*this = Vector2(x, y) * f;
	return *this;
}

Vector2& Vector2::operator /= (const float f)
{
	*this = Vector2(x, y) / f;
	return *this;
}

Vector2i Vector2::ToVector2i() const
{
	return Vector2i(static_cast<int>(x), static_cast<int>(y));
}

Vector2 ToVector2(const Vector2i& v)
{
	return Vector2(static_cast<float>(v.x), static_cast<float>(v.y));
}

} // namespace math
} // namespace gs2d
