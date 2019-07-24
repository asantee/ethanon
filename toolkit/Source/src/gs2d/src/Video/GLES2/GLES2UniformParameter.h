#ifndef GLES2_UNIFORM_PARAMETER_H_
#define GLES2_UNIFORM_PARAMETER_H_

#include "GLES2Video.h"

#ifdef APPLE_IOS
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>
#endif

#ifdef ANDROID
  #include <GLES2/gl2.h>
  #include <GLES2/gl2ext.h>
#endif

namespace gs2d {

enum PARAMETER_TYPE
{
	PT_FLOAT = 0,
	PT_VECTOR2 = 1,
	PT_VECTOR3,
	PT_VECTOR4,
	PT_VECTOR2_ARRAY,
	PT_M4X4,
	PT_TEXTURE,
	PT_INT
};
	
class GLES2UniformParameter
{
protected:
	str_type::string m_parameterName;
	str_type::string m_programName;
	int m_location;

public:
	GLES2UniformParameter(const str_type::string& programName, const str_type::string& parameterName);
	virtual void SetParameter(const GLuint program);
	virtual PARAMETER_TYPE GetType() const = 0;
};

class GLES2UPVec1 : public GLES2UniformParameter
{
	GLfloat v;
public:
	GLES2UPVec1(const float v1, const str_type::string& programName, const str_type::string& parameterName);
	void SetParameter(const GLuint program);
	PARAMETER_TYPE GetType() const;
};

class GLES2UPInt : public GLES2UniformParameter
{
	GLint i;
public:
	GLES2UPInt(const GLint i1, const str_type::string& programName, const str_type::string& parameterName);
	void SetParameter(const GLuint program);
	PARAMETER_TYPE GetType() const;
};

class GLES2UPVec2 : public GLES2UniformParameter
{
	math::Vector2 v;
public:
	GLES2UPVec2(const math::Vector2& v2, const str_type::string& programName, const str_type::string& parameterName);
	void SetParameter(const GLuint program);
	PARAMETER_TYPE GetType() const;
};

class GLES2UPVec3 : public GLES2UniformParameter
{
	math::Vector3 v;
public:
	GLES2UPVec3(const math::Vector3& v3, const str_type::string& programName, const str_type::string& parameterName);
	void SetParameter(const GLuint program);
	PARAMETER_TYPE GetType() const;
};

class GLES2UPVec4 : public GLES2UniformParameter
{
	math::Vector4 v;
public:
	GLES2UPVec4(const math::Vector4& v4, const str_type::string& programName, const str_type::string& parameterName);
	void SetParameter(const GLuint program);
	PARAMETER_TYPE GetType() const;
};

class GLES2UPMat4x4 : public GLES2UniformParameter
{
	math::Matrix4x4 m;
public:
	GLES2UPMat4x4(const math::Matrix4x4& m4x4, const str_type::string& programName, const str_type::string& parameterName);
	void SetParameter(const GLuint program);
	PARAMETER_TYPE GetType() const;
};

class GLES2UPTexture : public GLES2UniformParameter
{
	GLuint texture;
	GLint i;
public:
	GLES2UPTexture(const GLuint textureId, const GLint index, const str_type::string& programName, const str_type::string& parameterName);
	void SetParameter(const GLuint program);
	PARAMETER_TYPE GetType() const;
};

class GLES2UPVec2Array : public GLES2UniformParameter
{
	boost::shared_array<const math::Vector2> va;
	GLsizei count;
public:
	GLES2UPVec2Array(
		const boost::shared_array<const math::Vector2>& v,
		const unsigned int count,
		const str_type::string& programName,
		const str_type::string& parameterName);
	void SetParameter(const GLuint program);
	PARAMETER_TYPE GetType() const;
};

typedef boost::shared_ptr<GLES2UniformParameter> GLES2UniformParameterPtr;
typedef boost::shared_ptr<GLES2UPVec1> GLES2UPVec1Ptr;
typedef boost::shared_ptr<GLES2UPVec2> GLES2UPVec2Ptr;
typedef boost::shared_ptr<GLES2UPVec3> GLES2UPVec3Ptr;
typedef boost::shared_ptr<GLES2UPVec4> GLES2UPVec4Ptr;
typedef boost::shared_ptr<GLES2UPMat4x4> GLES2UPMat4x4Ptr;
typedef boost::shared_ptr<GLES2UPTexture> GLES2UPTexturePtr;
typedef boost::shared_ptr<GLES2UPVec2Array> GLES2UPVec2ArrayPtr;

} // namespace gs2d

#endif
