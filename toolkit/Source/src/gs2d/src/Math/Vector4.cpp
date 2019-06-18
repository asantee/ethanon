#include "Vector4.h"

namespace gs2d {
namespace math {

Vector4::Vector4() :
    x(0.0f),
    y(0.0f),
    z(0.0f),
    w(0.0f)
{
}

Vector4::Vector4(const Color& color)
{
    SetColor(color);
}

Vector4::Vector4(const Vector4& v) :
    x(v.x),
    y(v.y),
    z(v.z),
    w(v.w)
{
}

Vector4::Vector4(float vx, float vy, float vz, float vw) :
    x(vx),
    y(vy),
    z(vz),
    w(vw)
{
}

Vector4::Vector4(const Vector2& v2, float vz, float vw) :
    x(v2.x),
    y(v2.y),
    z(vz),
    w(vw)
{
}

Vector4::Vector4(const Vector2& a, const Vector2& b) :
    x(a.x),
    y(a.y),
    z(b.x),
    w(b.y)
{
}

Vector4::Vector4(const Vector3& v3, float vw) :
    x(v3.x),
    y(v3.y),
    z(v3.z),
    w(vw)
{
}

Vector4 Vector4::operator * (const float& v) const
{
    return Vector4(x * v, y * v, z * v, w * v);
}

Vector4 Vector4::operator * (const Vector4& v) const
{
    return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
}

Vector4 Vector4::operator / (const float& v) const
{
    return Vector4(x / v, y / v, z / v, w / v);
}

Vector4 Vector4::operator / (const Vector4& v) const
{
    return Vector4(x / v.x, y / v.y, z / v.z, w / v.w);
}

Vector4 Vector4::operator - (const float& v) const
{
    return Vector4(x - v, y - v, z - v, w - v);
}

Vector4 Vector4::operator - (const Vector4& v) const
{
    return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
}

Vector4 Vector4::operator + (const float& v) const
{
    return Vector4(x + v, y + v, z + v, w + v);
}

Vector4 Vector4::operator + (const Vector4& v) const
{
    return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
}

bool Vector4::operator == (const Vector4& v) const
{
    return (x == v.x && y == v.y && z == v.z && w == v.w);
}

bool Vector4::operator != (const Vector4& v) const
{
    return (x != v.x || y != v.y || z != v.z || w != v.w);
}

Vector4& Vector4::operator += (const Vector4& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

Vector4& Vector4::operator -= (const Vector4& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

Vector4& Vector4::operator *= (const float& v)
{
    x *= v;
    y *= v;
    z *= v;
    w *= v;
    return *this;
}

Vector4& Vector4::operator /= (const float& v)
{
    x /= v;
    y /= v;
    z /= v;
    w /= v;
    return *this;
}

void Vector4::SetColor(const Color& color)
{
    x = (static_cast<float>(color.r) / 255.0f);
    y = (static_cast<float>(color.g) / 255.0f);
    z = (static_cast<float>(color.b) / 255.0f);
    w = (static_cast<float>(color.a) / 255.0f);
}

Color Vector4::GetColor() const
{
	return Co
}

} // namespace math
} // namespace gs2d
