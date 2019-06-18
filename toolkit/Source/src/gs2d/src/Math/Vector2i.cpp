#include "Vector2i.h"

namespace gs2d {
namespace math {

Vector2i::Vector2i() :
	x(0),
	y(0)
{
}

Vector2i::Vector2i(const int vx, const int vy) :
	x(vx),
	y(vy)
{
}

Vector2i::Vector2i(const Vector2i& v) :
	x(v.x),
	y(v.y)
{
}

Vector2i Vector2i::operator * (const int& v) const
{
	return Vector2i(x * v, y * v);
}

Vector2i Vector2i::operator * (const Vector2i& v) const
{
	return Vector2i(x * v.x, y * v.y);
}

Vector2i Vector2i::operator / (const int& v) const
{
	return Vector2i(x / v, y / v);
}

Vector2i Vector2i::operator / (const Vector2i& v) const
{
	return Vector2i(x / v.x, y / v.y);
}

Vector2i Vector2i::operator - (const int& v) const
{
	return Vector2i(x - v, y - v);
}

Vector2i Vector2i::operator - (const Vector2i& v) const
{
	return Vector2i(x - v.x, y - v.y);
}

Vector2i Vector2i::operator + (const int& v) const
{
	return Vector2i(x + v, y + v);
}

Vector2i Vector2i::operator + (const Vector2i& v) const
{
	return Vector2i(x + v.x, y + v.y);
}

bool Vector2i::operator == (const Vector2i& v) const
{
	return (x == v.x && y == v.y);
}

bool Vector2i::operator != (const Vector2i& v) const
{
	return (x != v.x || y != v.y);
}

} // namespace math
} // namespace gs2d
