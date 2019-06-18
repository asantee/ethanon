#ifndef GS2D_VECTOR2i_H_
#define GS2D_VECTOR2i_H_

namespace gs2d {
namespace math {

struct Vector2i
{
	Vector2i();
	Vector2i(const int vx, const int vy);
	Vector2i(const Vector2i& v);
	Vector2i operator * (const int& v) const;
	Vector2i operator * (const Vector2i& v) const;
	Vector2i operator / (const int& v) const;
	Vector2i operator / (const Vector2i& v) const;
	Vector2i operator - (const int& v) const;
	Vector2i operator - (const Vector2i& v) const;
	Vector2i operator + (const int& v) const;
	Vector2i operator + (const Vector2i& v) const;
	bool operator == (const Vector2i& v) const;
	bool operator != (const Vector2i& v) const;
	int x,y;
};

} // namespace math
} // namespace gs2d

#endif
