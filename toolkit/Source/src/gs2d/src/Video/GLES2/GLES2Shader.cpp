#include "GLES2Shader.h"

#include "GLES2Texture.h"

#include <iostream>

namespace gs2d {

static bool CheckForGLError(const str_type::string& situation)
{
	bool r = false;
	for (GLint error = glGetError(); error; error = glGetError())
	{
		std::cerr << "ERROR: after " << situation << ". Error code " << error;
		r = true;
	}
	return r;
}

GLES2Shader::GLES2Shader(Platform::FileManagerPtr fileManager) :
	m_program(0),
	m_fileManager(fileManager)
{
}

GLES2Shader::~GLES2Shader()
{
	if (m_program)
	{
		glDeleteProgram(m_program);
	}
}

GLuint GLES2Shader::GetProgram() const
{
	return m_program;
}

bool GLES2Shader::LoadShaderFromFile(
	ShaderContextPtr context,
	const std::string& vsFileName,
	const std::string& vsEntry,
	const std::string& psFileName,
	const std::string& psEntry)
{
	str_type::string vsCode, psCode;
	if (!m_fileManager->GetAnsiFileString(vsFileName, vsCode))
	{
		std::cerr << "Couldn't open " << vsFileName << std::endl;
		return false;
	}
	if (!m_fileManager->GetAnsiFileString(psFileName, psCode))
	{
		std::cerr << "Couldn't open " << psFileName << std::endl;
		return false;
	}
	return LoadShaderFromString(context, vsFileName, vsCode, vsEntry, psFileName, psCode, psEntry);
}

GLuint compileShader(const GLenum type, const std::string& shaderName, const std::string& codeString)
{
	const GLuint r = glCreateShader(type);
	const char *source = codeString.c_str();
	glShaderSource(r, 1, &source, NULL);
	glCompileShader(r);

	GLint compiled = 0;
	glGetShaderiv(r, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		std::cerr << "Couldn't compile " << shaderName;
		GLint infoLen = 0;
		glGetShaderiv(r, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen)
		{
			char* buf = new char [infoLen];
			if (buf)
			{
				glGetShaderInfoLog(r, infoLen, NULL, buf);
				std::cerr << ". Error: " << buf << std::endl;
				delete [] buf;
			}
			glDeleteShader(r);
			return 0;
		}
	}
	return r;
}

bool GLES2Shader::LoadShaderFromString(
	ShaderContextPtr context,
	const std::string& vsShaderName,
	const std::string& vsCodeAsciiString,
	const std::string& vsEntry,
	const std::string& psShaderName,
	const std::string& psCodeAsciiString,
	const std::string& psEntry)
{
	m_vsShaderName = vsShaderName;
	m_psShaderName = psShaderName;

	m_programName = vsShaderName + "/" + psShaderName;

	const GLuint vs = compileShader(GL_VERTEX_SHADER, vsShaderName, vsCodeAsciiString);
	const GLuint ps = compileShader(GL_FRAGMENT_SHADER, psShaderName, psCodeAsciiString);

	m_program = glCreateProgram();
	glAttachShader(m_program, vs);
	CheckForGLError(vsShaderName + " glAttachShader (vertex)");

	glAttachShader(m_program, ps);
	CheckForGLError(psShaderName + " glAttachShader (fragment)");

	glLinkProgram(m_program);
	CheckForGLError(psShaderName + " glLinkProgram");

	GLint linkStatus = GL_FALSE;
	glGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		std::cerr << m_programName << ": link failed" << std::endl;

		GLint bufLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &bufLength);
		if (bufLength)
		{
			char* buf = new char [bufLength];
			if (buf)
			{
				glGetProgramInfoLog(m_program, bufLength, NULL, buf);
				std::cerr << m_programName << " could not link program: " << buf << std::endl;
				delete [] buf;
			}
		}
		glDeleteProgram(m_program);
		return false;
	}
	
	m_vPositionLocation = glGetAttribLocation(m_program, "vPosition");
	m_vTexCoordLocation = glGetAttribLocation(m_program, "vTexCoord");
	m_vNormalLocation = glGetAttribLocation(m_program, "vNormal");
	
	glDeleteShader(vs);
	glDeleteShader(ps);

	return true;
}

GLint GLES2Shader::GetVPositionLocation() const
{
	return m_vPositionLocation;
}

GLint GLES2Shader::GetVTexCoordLocation() const
{
	return m_vTexCoordLocation;
}

GLint GLES2Shader::GetVNormalLocation() const
{
	return m_vNormalLocation;
}

GLint GLES2Shader::FindUniformLocation(const std::string& name)
{
	tsl::hopscotch_map<std::string, GLint>::iterator iter = m_parameters.find(name);
	if (iter != m_parameters.end())
	{
		return iter->second;
	}
	else
	{
		const GLint location = glGetUniformLocation(m_program, name.c_str());
		if (location >= 0)
		{
			m_parameters.insert(std::pair<std::string, GLint>(name, location));
		}
		else
		{
			std::cerr << "Error: couldn't find uniform parameter " << name << std::endl;
		}
		return location;
	}
}

void GLES2Shader::SetConstant(const str_type::string& name, const math::Vector4 &v)
{
	const GLint location = FindUniformLocation(name);
	glUniform4f(location, v.x, v.y, v.z, v.w);
}

void GLES2Shader::SetConstant(const str_type::string& name, const math::Vector3 &v)
{
	const GLint location = FindUniformLocation(name);
	glUniform3f(location, v.x, v.y, v.z);
}

void GLES2Shader::SetConstant(const str_type::string& name, const math::Vector2 &v)
{
	const GLint location = FindUniformLocation(name);
	glUniform2f(location, v.x, v.y);
}

void GLES2Shader::SetConstant(const str_type::string& name, const float x)
{
	const GLint location = FindUniformLocation(name);
	glUniform1f(location, x);
}

void GLES2Shader::SetConstant(const str_type::string& name, const int n)
{
	const GLint location = FindUniformLocation(name);
	glUniform1i(location, n);
}

void GLES2Shader::SetConstantArray(const str_type::string& name, unsigned int nElements, const math::Vector2* v)
{
	const GLint location = FindUniformLocation(name);
	glUniform2fv(location, nElements, &(v[0].x));
}

void GLES2Shader::SetConstantArray(const str_type::string& name, unsigned int nElements, const math::Vector4* v)
{
	const GLint location = FindUniformLocation(name);
	glUniform4fv(location, nElements, &(v[0].x));
}

void GLES2Shader::SetMatrixConstant(const str_type::string& name, const math::Matrix4x4 &matrix)
{
	const GLint location = FindUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat*)&matrix.m[0][0]);
}

void GLES2Shader::SetTexture(const str_type::string& name, TexturePtr pTexture, const unsigned int index)
{
	GLES2Texture* tex = static_cast<GLES2Texture*>(pTexture.get());
	if (tex)
	{
		const GLint location = FindUniformLocation(name);
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, tex->GetTexture());
		glUniform1i(location, index);
	}
}

void GLES2Shader::SetShader()
{
	glUseProgram(m_program);
}

} //namespace gs2d
