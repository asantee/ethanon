#include "Matrix4x4.h"

#include <math.h>

namespace gs2d {
namespace math {

Matrix4x4::Matrix4x4(const Matrix4x4& m)
{
    mat = m.mat;
}

Matrix4x4::Matrix4x4()
{
    Identity();
}

void Matrix4x4::Identity()
{
    m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
    m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
    m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
    m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}

Matrix4x4::Matrix4x4(
    const float _11, const float _12, const float _13, const float _14,
    const float _21, const float _22, const float _23, const float _24,
    const float _31, const float _32, const float _33, const float _34,
    const float _41, const float _42, const float _43, const float _44)
{
    mat.a11=_11; mat.a12=_12; mat.a13=_13; mat.a14=_14;
    mat.a21=_21; mat.a22=_22; mat.a23=_23; mat.a24=_24;
    mat.a31=_31; mat.a32=_32; mat.a33=_33; mat.a34=_34;
    mat.a41=_41; mat.a42=_42; mat.a43=_43; mat.a44=_44;
}

float Matrix4x4::GetDeterminant()
{
    return
    mat.a14 * mat.a23 * mat.a32 * mat.a41-mat.a13 * mat.a24 * mat.a32 * mat.a41-mat.a14 * mat.a22 * mat.a33 * mat.a41+mat.a12 * mat.a24 * mat.a33 * mat.a41+
    mat.a13 * mat.a22 * mat.a34 * mat.a41-mat.a12 * mat.a23 * mat.a34 * mat.a41-mat.a14 * mat.a23 * mat.a31 * mat.a42+mat.a13 * mat.a24 * mat.a31 * mat.a42+
    mat.a14 * mat.a21 * mat.a33 * mat.a42-mat.a11 * mat.a24 * mat.a33 * mat.a42-mat.a13 * mat.a21 * mat.a34 * mat.a42+mat.a11 * mat.a23 * mat.a34 * mat.a42+
    mat.a14 * mat.a22 * mat.a31 * mat.a43-mat.a12 * mat.a24 * mat.a31 * mat.a43-mat.a14 * mat.a21 * mat.a32 * mat.a43+mat.a11 * mat.a24 * mat.a32 * mat.a43+
    mat.a12 * mat.a21 * mat.a34 * mat.a43-mat.a11 * mat.a22 * mat.a34 * mat.a43-mat.a13 * mat.a22 * mat.a31 * mat.a44+mat.a12 * mat.a23 * mat.a31 * mat.a44+
    mat.a13 * mat.a21 * mat.a32 * mat.a44-mat.a11 * mat.a23 * mat.a32 * mat.a44-mat.a12 * mat.a21 * mat.a33 * mat.a44+mat.a11 * mat.a22 * mat.a33 * mat.a44;
}

bool Matrix4x4::operator == (const Matrix4x4& other) const
{
    return !(
        mat.a11 != other.mat.a11
     || mat.a12 != other.mat.a12
     || mat.a13 != other.mat.a13
     || mat.a14 != other.mat.a14

     || mat.a21 != other.mat.a21
     || mat.a22 != other.mat.a22
     || mat.a23 != other.mat.a23
     || mat.a24 != other.mat.a24

     || mat.a31 != other.mat.a31
     || mat.a32 != other.mat.a32
     || mat.a33 != other.mat.a33
     || mat.a34 != other.mat.a34
        
     || mat.a41 != other.mat.a41
     || mat.a42 != other.mat.a42
     || mat.a43 != other.mat.a43
     || mat.a44 != other.mat.a44);
}

Matrix4x4 Matrix4x4::Scale(const float x, const float y, const float z)
{
    return Matrix4x4(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    );
}

Matrix4x4 Matrix4x4::Translate(const float x, const float y, const float z)
{
    return Matrix4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    );
}

Matrix4x4 Matrix4x4::RotateX(const float theta)
{
    const float sin = sinf(theta), cos = cosf(theta);
    return Matrix4x4(
        1, 0,   0,   0,
        0, cos, sin, 0,
        0,-sin, cos, 0,
        0, 0,   0,   1
    );
}

Matrix4x4 Matrix4x4::RotateY(const float theta)
{
    const float sin = sinf(theta), cos = cosf(theta);
    return Matrix4x4(
        cos, 0,-sin, 0,
        0,   1,   0, 0,
        sin, 0, cos, 0,
        0,   0,   0, 1
    );
}

Matrix4x4 Matrix4x4::RotateZ(const float theta)
{
    const float sin = sinf(theta), cos = cosf(theta);
    return Matrix4x4(
        cos, sin, 0, 0,
        -sin,cos, 0, 0,
        0,   0,   1, 0,
        0,   0,   0, 1
    );
}

Matrix4x4 Matrix4x4::Multiply(const Matrix4x4& m1, const Matrix4x4& m2)
{
    Matrix4x4 r;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            r.m[i][j] = 0;
            for (int k = 0; k < 4; k++)
            {
                r.m[i][j] += m1.m[i][k] * m2.m[k][j];
            }
        }
    }
    return r;
}

Vector3 Matrix4x4::Multiply(const Vector3& v3, const Matrix4x4& mat)
{
    return Vector3(
        v3.x * mat.mat.a11 +
        v3.y * mat.mat.a12 +
        v3.z * mat.mat.a13,

        v3.x * mat.mat.a21 +
        v3.y * mat.mat.a22 +
        v3.z * mat.mat.a23,

        v3.x * mat.mat.a31 +
        v3.y * mat.mat.a32 +
        v3.z * mat.mat.a33);
}

Vector4 Matrix4x4::Multiply(const Vector4& v4, const Matrix4x4& mat)
{
    return Vector4(
        v4.x * mat.mat.a11 +
        v4.y * mat.mat.a12 +
        v4.z * mat.mat.a13 +
        v4.w * mat.mat.a14,

        v4.x * mat.mat.a21 +
        v4.y * mat.mat.a22 +
        v4.z * mat.mat.a23 +
        v4.w * mat.mat.a24,

        v4.x * mat.mat.a31 +
        v4.y * mat.mat.a32 +
        v4.z * mat.mat.a33 +
        v4.w * mat.mat.a34,

        v4.x * mat.mat.a41 +
        v4.y * mat.mat.a42 +
        v4.z * mat.mat.a43 +
        v4.w * mat.mat.a44);
}

Vector2 Matrix4x4::Multiply(const Vector2 &v2, const Matrix4x4 &mat)
{
    return Vector2(
        v2.x * mat.mat.a11 +
        v2.y * mat.mat.a12,

        v2.x * mat.mat.a21 +
        v2.y * mat.mat.a22);
}

void Matrix4x4::Orthogonal(Matrix4x4 &out, const float w, const float h, const float zn, const float zf)
{
    out.Identity();
    out.mat.a11 = 2.0f / w;
    out.mat.a22 = 2.0f / h;
    out.mat.a33 = 1.0f / (zf - zn);
    out.mat.a43 = zn / (zn - zf);
}

Matrix4x4 Matrix4x4::Rotate(const float x, const float y, const float z)
{
    Matrix4x4 r, temp;
    r = RotateX(x);
    temp = RotateY(y);
    r = Multiply(r, temp);
    temp = RotateZ(z);
    return Multiply(r, temp);
}

} // namespace math
} // namespace gs2d