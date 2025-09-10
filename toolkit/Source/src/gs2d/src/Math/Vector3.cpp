#include "Vector3.h"

#include <math.h>

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

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

Vector3 Vector3::operator * (const float v) const noexcept
{
	return { x * v, y * v, z * v };
}

Vector3 Vector3::operator * (const Vector3& v) const noexcept
{
	return { x * v.x, y * v.y, z * v.z };
}

Vector3 Vector3::operator / (const float v) const noexcept
{
	#if defined(__ARM_NEON) || defined(__ARM_NEON__)
		// Load x,y; handle z separately to avoid over-reads.
		float32x2_t xy = vld1_f32(&x);

		// Compute 1/v with NEON reciprocal + two NR refinement steps.
		float32x2_t vv    = vdup_n_f32(v);
		float32x2_t recip = vrecpe_f32(vv);                 // initial approx
		recip = vmul_f32(vrecps_f32(vv, recip), recip);     // 1st refine
		recip = vmul_f32(vrecps_f32(vv, recip), recip);     // 2nd refine

		// Multiply x,y by 1/v
		float32x2_t rxy = vmul_f32(xy, recip);

		Vector3 out;
		vst1_f32(&out.x, rxy);
		out.z = z * vget_lane_f32(recip, 0);                // same recip for z
		return out;
	#else
		return { x / v, y / v, z / v };
	#endif
}

Vector3 Vector3::operator / (const Vector3& v) const noexcept
{
	#if defined(__ARM_NEON) || defined(__ARM_NEON__)
		// --- x and y: vectorized reciprocal ---
		float32x2_t a_xy    = vld1_f32(&x);      // this.x, this.y
		float32x2_t b_xy    = vld1_f32(&v.x);    // v.x, v.y

		float32x2_t rcp_xy  = vrecpe_f32(b_xy);                      // initial approx
		rcp_xy = vmul_f32(vrecps_f32(b_xy, rcp_xy), rcp_xy);         // refine 1
		rcp_xy = vmul_f32(vrecps_f32(b_xy, rcp_xy), rcp_xy);         // refine 2

		float32x2_t out_xy  = vmul_f32(a_xy, rcp_xy);                // (x/v.x, y/v.y)

		// --- z: scalar reciprocal via NEON (duplicate, refine, take lane 0) ---
		float32x2_t b_z     = vdup_n_f32(v.z);
		float32x2_t rcp_z   = vrecpe_f32(b_z);
		rcp_z = vmul_f32(vrecps_f32(b_z, rcp_z), rcp_z);             // refine 1
		rcp_z = vmul_f32(vrecps_f32(b_z, rcp_z), rcp_z);             // refine 2
		float out_z         = z * vget_lane_f32(rcp_z, 0);

		Vector3 out;
		vst1_f32(&out.x, out_xy);
		out.z = out_z;
		return out;
	#else
		return { x / v.x, y / v.y, z / v.z };
	#endif
}

Vector3 Vector3::operator - (const float v) const noexcept
{
	return Vector3(x - v, y - v, z - v);
}

Vector3 Vector3::operator - (const Vector3& v) const noexcept
{
	return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3 Vector3::operator + (const float v) const noexcept
{
	return Vector3(x + v, y + v, z + v);
}

Vector3 Vector3::operator + (const Vector3& v) const noexcept
{
	return Vector3(x + v.x, y + v.y, z + v.z);
}

bool Vector3::operator == (const Vector3& v) const noexcept
{
	return (x == v.x && y == v.y && z == v.z);
}

bool Vector3::operator != (const Vector3& v) const noexcept
{
	return (x != v.x || y != v.y || z != v.z);
}

Vector3& Vector3::operator += (const Vector3& v) noexcept
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3& Vector3::operator -= (const Vector3& v) noexcept
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3& Vector3::operator *= (const float v) noexcept
{
	x *= v;
	y *= v;
	z *= v;
	return *this;
}

Vector3& Vector3::operator /= (const float v) noexcept
{
	#if defined(__aarch64__)
		// AArch64: 1 scalar divide + muls is typically optimal
		const float inv = 1.0f / v;
		x *= inv;
		y *= inv;
		z *= inv;
		return *this;
	#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
		// ARMv7 NEON: reciprocal approximation + Newton–Raphson refinements
		float32x2_t xy    = vld1_f32(&x);          // load x,y
		float32x2_t vv    = vdup_n_f32(v);         // [v, v]
		float32x2_t recip = vrecpe_f32(vv);        // initial 1/v estimate
		recip = vmul_f32(vrecps_f32(vv, recip), recip); // refine 1
		recip = vmul_f32(vrecps_f32(vv, recip), recip); // refine 2

		xy = vmul_f32(xy, recip);                  // (x/v, y/v)
		vst1_f32(&x, xy);                          // store x,y
		z *= vget_lane_f32(recip, 0);              // reuse same reciprocal for z
		return *this;
	#else
		x /= v;
		y /= v;
		z /= v;
		return *this;
	#endif
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
