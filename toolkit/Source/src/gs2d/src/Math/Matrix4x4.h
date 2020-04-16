#ifndef GS2D_MATRIX4X4_H_
#define GS2D_MATRIX4X4_H_

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace gs2d {
namespace math {

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
	Matrix4x4(const Matrix4x4& m);
	Matrix4x4();
	void Identity();
	Matrix4x4(
		const float _11, const float _12, const float _13, const float _14,
		const float _21, const float _22, const float _23, const float _24,
		const float _31, const float _32, const float _33, const float _34,
		const float _41, const float _42, const float _43, const float _44);
	float GetDeterminant();
	bool operator == (const Matrix4x4& other) const;

	static Matrix4x4 Scale(const float x, const float y, const float z);
	static Matrix4x4 Translate(const float x, const float y, const float z);
	static Matrix4x4 RotateX(const float theta);
	static Matrix4x4 RotateY(const float theta);
	static Matrix4x4 RotateZ(const float theta);
	static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
	static Vector3 Multiply(const Vector3& v3, const Matrix4x4& mat);
	static Vector4 Multiply(const Vector4& v4, const Matrix4x4& mat);
	static Vector2 Multiply(const Vector2 &v2, const Matrix4x4 &mat);
	static void Orthogonal(Matrix4x4& outM, const float w, const float h, const float zn, const float zf);
	static Matrix4x4 Rotate(const float x, const float y, const float z);
};

} // namespace math
} // namespace gs2d

#endif
