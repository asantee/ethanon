#include "GLES2UniformParameter.h"
#include "GLES2Shader.h"

#include <iostream>

namespace gs2d {

GLES2UniformParameter::GLES2UniformParameter(const str_type::string& programName, const str_type::string& parameterName) :
	m_parameterName(parameterName),
	m_programName(programName),
	m_location(-1)
{
}

void GLES2UniformParameter::SetParameter(const GLuint program)
{
	if (m_location == -1)
	{
		m_location = glGetUniformLocation(program, m_parameterName.c_str());
		if (m_location == -1)
		{
			std::cerr << m_programName << ": parameter " << m_parameterName << " not found" << std::endl;
		}
	}
}

GLES2UPVec1::GLES2UPVec1(const float v1, const str_type::string& programName, const str_type::string& parameterName) :
	GLES2UniformParameter(programName, parameterName),
	v(v1)
{
}

void GLES2UPVec1::SetParameter(const GLuint program)
{
	GLES2UniformParameter::SetParameter(program);
	glUniform1f(m_location, v);
}

PARAMETER_TYPE GLES2UPVec1::GetType() const
{
	return PT_FLOAT;
}

GLES2UPInt::GLES2UPInt(const GLint i1, const str_type::string& programName, const str_type::string& parameterName) :
	GLES2UniformParameter(programName, parameterName),
	i(i1)
{
}

void GLES2UPInt::SetParameter(const GLuint program)
{
	GLES2UniformParameter::SetParameter(program);
	glUniform1i(m_location, i);
}

PARAMETER_TYPE GLES2UPInt::GetType() const
{
	return PT_INT;
}

GLES2UPVec2::GLES2UPVec2(const math::Vector2& v2, const str_type::string& programName, const str_type::string& parameterName) :
	GLES2UniformParameter(programName, parameterName),
	v(v2)
{
}

void GLES2UPVec2::SetParameter(const GLuint program)
{
	GLES2UniformParameter::SetParameter(program);
	glUniform2fv(m_location, 1, &v.x);
}
	
PARAMETER_TYPE GLES2UPVec2::GetType() const
{
	return PT_VECTOR2;
}

GLES2UPVec3::GLES2UPVec3(const math::Vector3& v3, const str_type::string& programName, const str_type::string& parameterName) :
	GLES2UniformParameter(programName, parameterName),
	v(v3)
{
}

void GLES2UPVec3::SetParameter(const GLuint program)
{
	GLES2UniformParameter::SetParameter(program);
	glUniform3fv(m_location, 1, &v.x);
}

PARAMETER_TYPE GLES2UPVec3::GetType() const
{
	return PT_VECTOR3;
}
	
GLES2UPVec4::GLES2UPVec4(const math::Vector4& v4, const str_type::string& programName, const str_type::string& parameterName) :
	GLES2UniformParameter(programName, parameterName),
	v(v4)
{
}

void GLES2UPVec4::SetParameter(const GLuint program)
{
	GLES2UniformParameter::SetParameter(program);
	glUniform4fv(m_location, 1, &v.x);
}

PARAMETER_TYPE GLES2UPVec4::GetType() const
{
	return PT_VECTOR4;
}

GLES2UPMat4x4::GLES2UPMat4x4(const math::Matrix4x4& m4x4, const str_type::string& programName, const str_type::string& parameterName) :
	GLES2UniformParameter(programName, parameterName),
	m(m4x4)
{
}

void GLES2UPMat4x4::SetParameter(const GLuint program)
{
	GLES2UniformParameter::SetParameter(program);
	glUniformMatrix4fv(m_location, 1, GL_FALSE, (GLfloat*)&m.m[0][0]);
}

PARAMETER_TYPE GLES2UPMat4x4::GetType() const
{
	return PT_M4X4;
}

GLES2UPTexture::GLES2UPTexture(
	const GLuint textureId,
	const GLint index,
	const str_type::string& programName,
	const str_type::string& parameterName) :
	GLES2UniformParameter(programName, parameterName),
	i(index),
	texture(textureId)
{
}

void GLES2UPTexture::SetParameter(const GLuint program)
{
	GLES2UniformParameter::SetParameter(program);
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(m_location, i);
}

PARAMETER_TYPE GLES2UPTexture::GetType() const
{
	return PT_TEXTURE;
}
	
GLES2UPVec2Array::GLES2UPVec2Array(
	const boost::shared_array<const gs2d::math::Vector2>& v,
	const unsigned int count,
	const str_type::string& programName,
	const str_type::string& parameterName) :
	GLES2UniformParameter(programName, parameterName),
	count(static_cast<GLsizei>(count)),
	va(v)
{
}

void GLES2UPVec2Array::SetParameter(const GLuint program)
{
	GLES2UniformParameter::SetParameter(program);
	glUniform2fv(m_location, count, &(va.get()->x));
}

PARAMETER_TYPE GLES2UPVec2Array::GetType() const
{
	return PT_VECTOR2_ARRAY;
}

} // namespace gs2d
