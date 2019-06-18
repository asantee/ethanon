#include "Vector3.h"

#include <math.h>

namespace gs2d {
namespace math {

Vector3::Vector3()
{
    x = y = z = 0.0f;
}

Vector3::Vector3(const Vector3& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
}

Vector3::Vector3(const float& vx, const float& vy, const float& vz)
{
    x = vx; y = vy; z = vz;
}

Vector3::Vector3(const Vector2& v2, const float& vz)
{
    x = v2.x; y = v2.y; z = vz;
}

float Vector3::Length() const
{
    return sqrtf(x * x + y * y + z * z);
}

float Vector3::SquaredLength() const
{
    return (x * x + y * y + z * z);
}

Vector3 Vector3::operator * (const float& v) const
{
    return Vector3(x * v, y * v, z * v);
}

Vector3 Vector3::operator * (const Vector3& v) const
{
    return Vector3(x * v.x, y * v.y, z * v.z);
}

Vector3 Vector3::operator / (const float& v) const
{
    return Vector3(x / v, y / v, z / v);
}

Vector3 Vector3::operator / (const Vector3& v) const
{
    return Vector3(x / v.x, y / v.y, z / v.z);
}

Vector3 Vector3::operator - (const float& v) const
{
    return Vector3(x - v, y - v, z - v);
}

Vector3 Vector3::operator - (const Vector3& v) const
{
    return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3 Vector3::operator + (const float& v) const
{
    return Vector3(x + v, y + v, z + v);
}

Vector3 Vector3::operator + (const Vector3& v) const
{
    return Vector3(x + v.x, y + v.y, z + v.z);
}

bool Vector3::operator == (const Vector3& v) const
{
    return (x == v.x && y == v.y && z == v.z);
}

bool Vector3::operator != (const Vector3& v) const
{
    return (x != v.x || y != v.y || z != v.z);
}

Vector3& Vector3::operator += (const Vector3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vector3& Vector3::operator -= (const Vector3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vector3& Vector3::operator *= (const float& v)
{
    x *= v;
    y *= v;
    z *= v;
    return *this;
}

Vector3& Vector3::operator /= (const float& v)
{
    x /= v;
    y /= v;
    z /= v;
    return *this;
}

} // namespace math
} // namespace gs2d
