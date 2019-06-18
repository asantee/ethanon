#ifndef GS2D_VECTOR2_H_
#define GS2D_VECTOR2_H_

#include "Vector2i.h"

namespace gs2d {
namespace math {

struct PODVector2 { float x, y; };

struct Vector2 : public PODVector2
{
	Vector2();
	Vector2(const float& v);
	Vector2(const Vector2& v);
	Vector2(const float& vx, const float& vy);
	float Length() const;
	float SquaredLength() const;
	Vector2 operator * (const float v) const;
	Vector2 operator * (const Vector2& v) const;
	Vector2 operator / (const float v) const;
	Vector2 operator / (const Vector2& v) const;
	Vector2 operator - (const float v) const;
	Vector2 operator - (const Vector2& v) const;
	Vector2 operator + (const float v) const;
	Vector2 operator + (const Vector2& v) const;
	bool operator == (const Vector2& v) const;
	bool operator != (const Vector2& v) const;
	Vector2& operator += (const Vector2& v);
	Vector2& operator -= (const Vector2& v);
	Vector2& operator *= (const float f);
	Vector2& operator /= (const float f);
	Vector2i ToVector2i() const;
};

Vector2 ToVector2(const Vector2i& v);

namespace constant {
const Vector2 HALF_VECTOR2(0.5f, 0.5f);
const Vector2 ZERO_VECTOR2(0.0f, 0.0f);
const Vector2 ONE_VECTOR2(1.0f, 1.0f);
} // namespace constant

} // namespace math
} // namespace gs2d

#endif
