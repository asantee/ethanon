#ifndef GS2D_MATH_H_
#define GS2D_MATH_H_

#include "Vector2.h"
#include "Vector4.h"

#include <math.h>

namespace gs2d {
namespace math {

namespace constant {
const float PI = 3.141592654f;
const float PI2 = PI * 2;
const float PIb = 1.570796327f;
} // namespace constant

struct Rect2D
{
	Rect2D() : pos(0,0), size(0,0)
	{
	}

	Rect2D(const Vector2i &pos, const Vector2i &size)
	{
		this->pos = pos;
		this->size = size;
	}

	Rect2D(const int posX, const int posY, const int sizeX, const int sizeY)
	{
		this->pos.x = posX;
		this->pos.y = posY;
		this->size.x = sizeX;
		this->size.y = sizeY;
	}

	inline bool operator == (const Rect2D& r) const
	{
		return (pos == r.pos && size == r.size);
	}

	inline bool operator != (const Rect2D& r) const
	{
		return !(*this == r);
	}	

	Vector2i pos, size;
};

struct Rect2Df
{
    Rect2Df() : pos(0,0), size(0,0), originalSize(0,0), offset(0,0)
	{
	}

	Rect2Df(const Vector2 &pos, const Vector2 &size)
	{
		this->pos = pos;
		this->size = this->originalSize = size;
	}

    Rect2Df(const Vector2 &pos, const Vector2 &size, const Vector2 &offset, const Vector2 &originalSize)
    {
        this->pos = pos;
        this->size = size;
        this->offset = offset;
        this->originalSize = originalSize;
    }
    
	Rect2Df(const float posX, const float posY, const float sizeX, const float sizeY)
	{
		this->pos.x = posX;
		this->pos.y = posY;
		this->size.x = sizeX;
		this->size.y = sizeY;
	}

	inline bool operator == (const Rect2Df& r) const
	{
		return (pos == r.pos && size == r.size);
	}
	
	inline bool operator != (const Rect2Df& r) const
	{
		return !(*this == r);
	}	

	Vector2 pos, size, offset, originalSize;
};

struct PODMatrix4x4
{
	struct Matrix
	{
		float a11, a12, a13, a14;
		float a21, a22, a23, a24;
		float a31, a32, a33, a34;
		float a41, a42, a43, a44;
	};
	union
	{
		Matrix mat;
		float m[4][4];
		float e[16];
	};
};

struct Matrix4x4 : public PODMatrix4x4
{
	Matrix4x4(const Matrix4x4& m)
	{
		for (unsigned int t = 0; t < 16; t++)
		{
			e[t] = m.e[t];
		}
	}

	Matrix4x4()
	{
		Identity();
	}

	inline void Identity()
	{
		m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
		m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
		m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
		m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
	}

	Matrix4x4(const float _11, const float _12, const float _13, const float _14,
			const float _21, const float _22, const float _23, const float _24,
			const float _31, const float _32, const float _33, const float _34,
			const float _41, const float _42, const float _43, const float _44)
	{
		mat.a11=_11; mat.a12=_12; mat.a13=_13; mat.a14=_14;
		mat.a21=_21; mat.a22=_22; mat.a23=_23; mat.a24=_24;
		mat.a31=_31; mat.a32=_32; mat.a33=_33; mat.a34=_34;
		mat.a41=_41; mat.a42=_42; mat.a43=_43; mat.a44=_44;
	}

	float GetDeterminant()
	{
		return 
		mat.a14 * mat.a23 * mat.a32 * mat.a41-mat.a13 * mat.a24 * mat.a32 * mat.a41-mat.a14 * mat.a22 * mat.a33 * mat.a41+mat.a12 * mat.a24 * mat.a33 * mat.a41+
		mat.a13 * mat.a22 * mat.a34 * mat.a41-mat.a12 * mat.a23 * mat.a34 * mat.a41-mat.a14 * mat.a23 * mat.a31 * mat.a42+mat.a13 * mat.a24 * mat.a31 * mat.a42+
		mat.a14 * mat.a21 * mat.a33 * mat.a42-mat.a11 * mat.a24 * mat.a33 * mat.a42-mat.a13 * mat.a21 * mat.a34 * mat.a42+mat.a11 * mat.a23 * mat.a34 * mat.a42+
		mat.a14 * mat.a22 * mat.a31 * mat.a43-mat.a12 * mat.a24 * mat.a31 * mat.a43-mat.a14 * mat.a21 * mat.a32 * mat.a43+mat.a11 * mat.a24 * mat.a32 * mat.a43+
		mat.a12 * mat.a21 * mat.a34 * mat.a43-mat.a11 * mat.a22 * mat.a34 * mat.a43-mat.a13 * mat.a22 * mat.a31 * mat.a44+mat.a12 * mat.a23 * mat.a31 * mat.a44+
		mat.a13 * mat.a21 * mat.a32 * mat.a44-mat.a11 * mat.a23 * mat.a32 * mat.a44-mat.a12 * mat.a21 * mat.a33 * mat.a44+mat.a11 * mat.a22 * mat.a33 * mat.a44;
	}
	
	inline bool operator == (const Matrix4x4& other) const
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

};

inline float GetAngle(const Vector2& v2)
{
	const float r = atan2f(v2.x, v2.y);
	return (r < 0) ? r + (2 * constant::PI) : r;
}

inline float RadianToDegree(const float angle)
{
	return (angle / (constant::PI * 2)) * 360.0f;
}

inline float DegreeToRadian(const float angle)
{
	return angle * (constant::PI / 180.0f);
}

inline float DP2(const Vector2& a, const Vector2& b)
{
	return ((a.x * b.x) + (a.y * b.y));
}

inline float DP3(const Vector3& a, const Vector3& b)
{
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

inline float DP4(const Vector4& a, const Vector4& b)
{
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
}

inline Vector3 cross(const Vector3& v0, const Vector3& v1)
{
	return Vector3((v0.y * v1.z) - (v0.z * v1.y), (v0.z * v1.x) - (v0.x * v1.z), (v0.x * v1.y) - (v0.y * v1.x));
}

inline Vector3 Normalize(const Vector3& v0)
{
	const float a = sqrtf(DP3(v0, v0));
	if (a <= 0.0f)
		return Vector3(0,0,0);
	return Vector3(v0.x / a, v0.y / a, v0.z / a);
}

inline Vector2 Normalize(const Vector2& v0)
{
	const float a = sqrtf(DP2(v0, v0));
	if (a <= 0.0f)
		return Vector2(0,0);
	return Vector2(v0.x / a, v0.y / a);
}

inline float SquaredDistance(const Vector3& P0, const Vector3& P1)
{
	const Vector3 diff = P1 - P0;
	return DP3(diff, diff);
}

inline float SquaredDistance(const Vector2& P0, const Vector2& P1)
{
	const Vector2 diff = P1 - P0;
	return DP2(diff, diff);
}

inline float Distance(const Vector3& P0, const Vector3& P1)
{
	const Vector3 diff = P1 - P0;
	return sqrtf(DP3(diff, diff));
}

inline float Distance(const Vector2& P0, const Vector2& P1)
{
	const Vector2 diff = P1 - P0;
	return sqrtf(DP2(diff, diff));
}

inline Matrix4x4 Scale(const float x, const float y, const float z)
{
	return Matrix4x4(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
	);
}

inline Matrix4x4 Translate(const float x, const float y, const float z)
{
	return Matrix4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1
	);
}

inline Matrix4x4 RotateX(const float theta)
{
	const float sin = sinf(theta), cos = cosf(theta);
	return Matrix4x4(
		1, 0,   0,   0,
		0, cos, sin, 0,
		0,-sin, cos, 0,
		0, 0,   0,   1
	);
}

inline Matrix4x4 RotateY(const float theta)
{
	const float sin = sinf(theta), cos = cosf(theta);
	return Matrix4x4(
		cos, 0,-sin, 0,
		0,   1,   0, 0,
		sin, 0, cos, 0,
		0,   0,   0, 1
	);
}

inline Matrix4x4 RotateZ(const float theta)
{
	const float sin = sinf(theta), cos = cosf(theta);
	return Matrix4x4(
		cos, sin, 0, 0,
		-sin,cos, 0, 0,
		0,   0,   1, 0,
		0,   0,   0, 1
	);
}

inline Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2)
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

inline Vector3 Multiply(const Vector3 &v3, const Matrix4x4 &mat)
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

inline Vector4 Multiply(const Vector4 &v4, const Matrix4x4 &mat)
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

inline Vector2 Multiply(const Vector2 &v2, const Matrix4x4 &mat)
{
	return Vector2(
		v2.x * mat.mat.a11 +
		v2.y * mat.mat.a12,

		v2.x * mat.mat.a21 +
		v2.y * mat.mat.a22);
}

inline void Orthogonal(Matrix4x4 &out, const float w, const float h, const float zn, const float zf)
{
	out.Identity();
	out.mat.a11 = 2.0f / w;
	out.mat.a22 = 2.0f / h;
	out.mat.a33 = 1.0f / (zf - zn);
	out.mat.a43 = zn / (zn - zf);
}

inline Matrix4x4 Rotate(const float x, const float y, const float z)
{
	Matrix4x4 r, temp;
	r = RotateX(x);
	temp = RotateY(y);
	r = Multiply(r, temp);
	temp = RotateZ(z);
	return Multiply(r, temp);
}

template <class T>
inline T Max(const T &a, const T &b)
{
	return (a > b) ? a : b;
}

template <class T>
inline T Min(const T &a, const T &b)
{
	return (a < b) ? a : b;
}

template <class T>
inline T Abs(const T &n)
{
	return (n < 0) ? -n : n;
}

template <class T>
inline T Clamp(const T &n, const T &min, const T &max)
{
	return (n < min) ? min : (n > max) ? max : n;
}

template <class T>
inline T Square(const T &a)
{
	return a*a;
}

template <class T>
inline T Sign(const T &n)
{
	return (n < static_cast<T>(0)) ? static_cast<T>(-1) : (n > static_cast<T>(0)) ? static_cast<T>(1) : static_cast<T>(0);
}

inline Vector2 Vector2Min(const Vector2& a, const Vector2& b)
{
    return Vector2(Min(a.x, b.x), Min(a.y, b.y));
}

inline Vector2 Vector2Max(const Vector2& a, const Vector2& b)
{
    return Vector2(Max(a.x, b.x), Max(a.y, b.y));
}

typedef Vector4 Plane;
inline Plane ComputePlane(const Vector3& pos, const Vector3& normal)
{
	return (Plane(Normalize(normal), -((normal.x * pos.x) + (normal.y * pos.y) + (normal.z * pos.z))));
}

inline Plane ComputePlane(const Vector2& pos, const Vector2& normal)
{
	return (Plane(Normalize(normal), 0, -((normal.x * pos.x) + (normal.y * pos.y))));
}

inline float PlaneDotVector(const Plane& plane, const Vector3& vec)
{
	return (plane.x * vec.x) + (plane.y * vec.y) + (plane.z * vec.z) + (plane.w);
}

inline float PlaneDotVector(const Plane& plane, const Vector2& vec)
{
	return (plane.x * vec.x) + (plane.y * vec.y) + (plane.w);
}

inline Vector4 ConvertToV4(const Color& color)
{
	return Vector4(
		(float)color.r / 255.0f,
		(float)color.g / 255.0f,
		(float)color.b / 255.0f,
		(float)color.a / 255.0f);
}

} // namespace math
} // namespace gs2d

#endif
