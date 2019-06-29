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

Vector4::Vector4(const float& v) :
    x(v),
    y(v),
    z(v),
    w(v)
{
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

float Vector4::DP4(const Vector4& a, const Vector4& b)
{
    return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
}

Vector4::Plane Vector4::ComputePlane(const Vector3& pos, const Vector3& normal)
{
    return (Plane(Vector3::Normalize(normal), -((normal.x * pos.x) + (normal.y * pos.y) + (normal.z * pos.z))));
}

float Vector4::PlaneDotVector(const Plane& plane, const Vector3& vec)
{
    return (plane.x * vec.x) + (plane.y * vec.y) + (plane.z * vec.z) + (plane.w);
}

Vector4::Plane Vector4::ComputePlane(const Vector2& pos, const Vector2& normal)
{
    return (Plane(Vector2::Normalize(normal), 0, -((normal.x * pos.x) + (normal.y * pos.y))));
}

float Vector4::PlaneDotVector(const Plane& plane, const Vector2& vec)
{
    return (plane.x * vec.x) + (plane.y * vec.y) + (plane.w);
}

} // namespace math
} // namespace gs2d
