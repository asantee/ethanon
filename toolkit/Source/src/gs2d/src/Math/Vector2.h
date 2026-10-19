#ifndef GS2D_VECTOR2_H_
#define GS2D_VECTOR2_H_

#include "Vector2i.h"

namespace gs2d {
namespace math {

struct PODVector2
{
	constexpr PODVector2() noexcept : x(0.0f), y(0.0f) {}
	constexpr explicit PODVector2(const float v) noexcept : x(v), y(v) {}
	constexpr PODVector2(const float vx, const float vy) noexcept : x(vx), y(vy) {}

	constexpr PODVector2(const PODVector2&) noexcept = default;
	PODVector2& operator = (const PODVector2&) noexcept = default;
	PODVector2(PODVector2&&) noexcept = default;
	PODVector2& operator = (PODVector2&&) noexcept = default;

	float x, y;
};

// The operators are plain C++ defined here so they inline: a call per operation cost more than the
// arithmetic, and the compiler already emits good code for two floats without intrinsics
struct Vector2 : public PODVector2
{
	constexpr Vector2() noexcept : PODVector2() {}
	constexpr explicit Vector2(const float v) noexcept : PODVector2(v) {}
	constexpr Vector2(const Vector2& v) noexcept : PODVector2(v.x, v.y) {}
	constexpr Vector2(const float vx, const float vy) noexcept : PODVector2(vx, vy) {}
	float Length() const noexcept;
	float SquaredLength() const noexcept { return DP2(*this, *this); }
	Vector2 operator * (const float v) const noexcept { return Vector2(x * v, y * v); }
	Vector2 operator * (const Vector2& v) const noexcept { return Vector2(x * v.x, y * v.y); }
	Vector2 operator / (const float v) const noexcept { return Vector2(x / v, y / v); }
	Vector2 operator / (const Vector2& v) const noexcept { return Vector2(x / v.x, y / v.y); }
	Vector2 operator - (const float v) const noexcept { return Vector2(x - v, y - v); }
	Vector2 operator - (const Vector2& v) const noexcept { return Vector2(x - v.x, y - v.y); }
	Vector2 operator + (const float v) const noexcept { return Vector2(x + v, y + v); }
	Vector2 operator + (const Vector2& v) const noexcept { return Vector2(x + v.x, y + v.y); }
	bool operator == (const Vector2& v) const noexcept { return (x == v.x && y == v.y); }
	bool operator != (const Vector2& v) const noexcept { return !(*this == v); }
	Vector2& operator += (const Vector2& v) noexcept { x += v.x; y += v.y; return *this; }
	Vector2& operator -= (const Vector2& v) noexcept { x -= v.x; y -= v.y; return *this; }
	Vector2& operator *= (const float f) noexcept { x *= f; y *= f; return *this; }
	Vector2& operator /= (const float f) noexcept { x /= f; y /= f; return *this; }
	Vector2i ToVector2i() const noexcept;

	static Vector2 ToVector2(const Vector2i& v) noexcept;
	static float Distance(const Vector2& a, const Vector2& b) noexcept;
	static float GetAngle(const Vector2& v) noexcept;
	static float DP2(const Vector2& a, const Vector2& b) noexcept
	{
		// Two statements, so the compiler cannot fuse them into one multiply-add, which
		// rounds once instead of twice: the result stays what the NEON code gave on ARM
		const float xx = a.x * b.x;
		const float yy = a.y * b.y;
		return xx + yy;
	}
	static Vector2 Normalize(const Vector2& v) noexcept;
	static Vector2 Vector2Min(const Vector2& a, const Vector2& b) noexcept;
	static Vector2 Vector2Max(const Vector2& a, const Vector2& b) noexcept;
	static float SquaredDistance(const Vector2& a, const Vector2& b) noexcept;
};

namespace constant {
const Vector2 HALF_VECTOR2(0.5f, 0.5f);
const Vector2 ZERO_VECTOR2(0.0f, 0.0f);
const Vector2 ONE_VECTOR2(1.0f, 1.0f);
} // namespace constant

} // namespace math
} // namespace gs2d

#endif
