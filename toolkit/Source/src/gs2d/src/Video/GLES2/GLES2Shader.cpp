#include "GLES2Shader.h"

namespace gs2d {

GLES2ShaderContext::GLES2ShaderContext(GLES2Video *pVideo)
{
}

GLES2ShaderContext::~GLES2ShaderContext()
{
}

boost::any GLES2ShaderContext::GetContextPointer()
{
	return 0;
}

bool GLES2ShaderContext::CheckForError(const str_type::string& situation)
{
	bool r = false;
	for (GLint error = glGetError(); error; error = glGetError())
	{
		std::cerr << "ERROR: after " << situation << ". Error code " << error;
		r = true;
	}
	return r;
}

GLES2Shader::GLES2Shader(Platform::FileManagerPtr fileManager, GLES2ShaderContextPtr context) :
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
	GLES2ShaderContext::CheckForError(vsShaderName + " glAttachShader (vertex)");

	glAttachShader(m_program, ps);
	GLES2ShaderContext::CheckForError(psShaderName + " glAttachShader (fragment)");

	glLinkProgram(m_program);

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
	
	glDeleteShader(vs);
	glDeleteShader(ps);

	return true;
}

void GLES2Shader::SetConstant(const str_type::string& name, const math::Vector4 &v)
{
	m_parameters[name] = GLES2UniformParameterPtr(new GLES2UPVec4(v, m_programName, name));
}

void GLES2Shader::SetConstant(const str_type::string& name, const math::Vector3 &v)
{
	m_parameters[name] = GLES2UniformParameterPtr(new GLES2UPVec3(v, m_programName, name));
}

void GLES2Shader::SetConstant(const str_type::string& name, const math::Vector2 &v)
{
	m_parameters[name] = GLES2UniformParameterPtr(new GLES2UPVec2(v, m_programName, name));
}

void GLES2Shader::SetConstant(const str_type::string& name, const float x)
{
	m_parameters[name] = GLES2UniformParameterPtr(new GLES2UPVec1(x, m_programName, name));
}

void GLES2Shader::SetConstant(const str_type::string& name, const int n)
{
	m_parameters[name] = GLES2UniformParameterPtr(new GLES2UPInt(n, m_programName, name));
}

void GLES2Shader::SetConstantArray(const str_type::string& name, unsigned int nElements, const boost::shared_array<const math::Vector2>& v)
{
	m_parameters[name] = GLES2UniformParameterPtr(new GLES2UPVec2Array(v, nElements, m_programName, name));
}

void GLES2Shader::SetMatrixConstant(const str_type::string& name, const math::Matrix4x4 &matrix)
{
	m_parameters[name] = GLES2UniformParameterPtr(new GLES2UPMat4x4(matrix, m_programName, name));
}

void GLES2Shader::SetTexture(const str_type::string& name, TextureWeakPtr pTexture, const unsigned int index)
{
	GLES2Texture* tex = static_cast<GLES2Texture*>(pTexture.lock().get());
	if (tex)
	{
		m_parameters[name] = GLES2UniformParameterPtr(new GLES2UPTexture(tex->GetTextureID(), index, m_programName, name));
	}
}

bool GLES2Shader::SetShader()
{
	glUseProgram(m_program);
	for (tsl::hopscotch_map<std::string, GLES2UniformParameterPtr>::iterator iter = m_parameters.begin();
		iter != m_parameters.end(); ++iter)
	{
		iter.value()->SetParameter(m_program);
	}
	return true;
}

} //namespace gs2d
