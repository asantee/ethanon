#include "GLShader.h"

#include "GLTexture.h"

#include <iostream>

#define MAX_MESSAGE_SIZE 1024

namespace gs2d {

boost::any GLShaderContext::GetContextPointer()
{
	return 0;
}

GLShader::GLShader(Platform::FileManagerPtr fileManager) :
	m_fileManager(fileManager)
{
}

GLShader::~GLShader()
{
	glDeleteProgram(m_program);
}

bool GLShader::LoadShaderFromFile(
	ShaderContextPtr context,
	const std::string& vsFileName,
	const std::string& vsEntry,
	const std::string& psFileName,
	const std::string& psEntry)
{
	std::string vsStr;
	std::string psStr;

	m_fileManager->GetAnsiFileString(vsFileName, vsStr);
	m_fileManager->GetAnsiFileString(psFileName, psStr);
	return LoadShaderFromString(context, vsFileName, vsStr, vsEntry, psFileName, psStr, psEntry);
}

bool GLShader::LoadShaderFromString(
	ShaderContextPtr context,
    const std::string& vsShaderName,
    const std::string& vsCodeAsciiString,
    const std::string& vsEntry,
    const std::string& psShaderName,
    const std::string& psCodeAsciiString,
    const std::string& psEntry)
{
	const char* vertexShaderCstr   = vsCodeAsciiString.c_str();
	const char* fragmentShaderCstr = psCodeAsciiString.c_str();

	const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCstr, NULL);
	glCompileShader(vertexShader);

	GLint success;
	char infoLog[MAX_MESSAGE_SIZE];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, MAX_MESSAGE_SIZE, NULL, infoLog);
		std::cerr << "glCompileShader vertex: " << std::string(infoLog) << std::endl;
		return false;
	}

	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCstr, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, MAX_MESSAGE_SIZE, NULL, infoLog);
		std::cerr << "glCompileShader fragment: " << std::string(infoLog) << std::endl;
		return false;
	}

	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);

	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_program, MAX_MESSAGE_SIZE, NULL, infoLog);
		std::cerr << "glLinkProgram: " << std::string(infoLog) << std::endl;
		return false;
	}
	
	glDetachShader(m_program, vertexShader);
	glDetachShader(m_program, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	return true;
}

GLint GLShader::FindUniformLocation(const std::string& name)
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
		return location;
	}
}

void GLShader::SetShader()
{
	glUseProgram(m_program);
}

void GLShader::SetConstant(const std::string& name, const int n)
{
	const GLint location = FindUniformLocation(name);
	glUniform1i(location, n);
}

void GLShader::SetConstant(const std::string& name, const float v)
{
	const GLint location = FindUniformLocation(name);
	glUniform1f(location, v);
}

void GLShader::SetConstant(const std::string& name, const math::Vector2& v)
{
	const GLint location = FindUniformLocation(name);
	glUniform2f(location, v.x, v.y);
}

void GLShader::SetConstant(const std::string& name, const math::Vector3& v)
{
	const GLint location = FindUniformLocation(name);
	glUniform3f(location, v.x, v.y, v.z);
}

void GLShader::SetConstant(const std::string& name, const math::Vector4& v)
{
	const GLint location = FindUniformLocation(name);
	glUniform4f(location, v.x, v.y, v.z, v.w);
}

void GLShader::SetMatrixConstant(const std::string& name, const math::Matrix4x4& matrix)
{
	const GLint location = FindUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, &matrix.m[0][0]);
}

void GLShader::SetTexture(const std::string& name, TexturePtr texture, const unsigned int index)
{
	const GLint location = FindUniformLocation(name);
	glUniform1i(location, index);
	glActiveTexture(GL_TEXTURE0 + index);

	const GLTexture* texturePtr = (const GLTexture*)texture.get();
	glBindTexture(GL_TEXTURE_2D, texturePtr->GetTexture());
}

void GLShader::SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector2* v)
{
	const GLint location = FindUniformLocation(name);
	glUniform2fv(location, nElements, static_cast<const GLfloat*>(&v->x));
}

void GLShader::SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector4* v)
{
	const GLint location = FindUniformLocation(name);
	glUniform4fv(location, nElements, static_cast<const GLfloat*>(&v->x));
}

} // namespace sprite
