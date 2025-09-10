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

struct Vector2 : public PODVector2
{
	constexpr Vector2() noexcept : PODVector2() {}
	constexpr explicit Vector2(const float v) noexcept : PODVector2(v) {}
	constexpr Vector2(const Vector2& v) noexcept : PODVector2(v.x, v.y) {}
	constexpr Vector2(const float vx, const float vy) noexcept : PODVector2(vx, vy) {}
	float Length() const noexcept;
	float SquaredLength() const noexcept;
	Vector2 operator * (const float v) const noexcept;
	Vector2 operator * (const Vector2& v) const noexcept;
	Vector2 operator / (const float v) const noexcept;
	Vector2 operator / (const Vector2& v) const noexcept;
	Vector2 operator - (const float v) const noexcept;
	Vector2 operator - (const Vector2& v) const noexcept;
	Vector2 operator + (const float v) const noexcept;
	Vector2 operator + (const Vector2& v) const noexcept;
	bool operator == (const Vector2& v) const noexcept;
	bool operator != (const Vector2& v) const noexcept;
	Vector2& operator += (const Vector2& v) noexcept;
	Vector2& operator -= (const Vector2& v) noexcept;
	Vector2& operator *= (const float f) noexcept;
	Vector2& operator /= (const float f) noexcept;
	Vector2i ToVector2i() const noexcept;

	static Vector2 ToVector2(const Vector2i& v) noexcept;
	static float Distance(const Vector2& a, const Vector2& b) noexcept;
	static float GetAngle(const Vector2& v) noexcept;
	static float DP2(const Vector2& a, const Vector2& b) noexcept;
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
