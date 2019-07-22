#include "GLES2Shader.h"

namespace gs2d {

GLES2ShaderContext::GLES2ShaderContext(GLES2Video *pVideo) :
	m_logger(Platform::FileLogger::GetLogDirectory() + "GLES2ShaderContext.log.txt")
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
		std::stringstream ss;
		ss << "ERROR: after " << situation << ". Error code " << error;
		m_logger.Log(ss.str(), Platform::FileLogger::ERROR);
		r = true;
	}
	return r;
}

GLES2Shader::GLES2Shader(Platform::FileManagerPtr fileManager, GLES2ShaderContextPtr context)
{
}

GLES2Shader::~GLES2Shader()
{
}

bool GLES2Shader::LoadShaderFromFile(
	ShaderContextPtr context,
	const std::string& vsFileName,
	const std::string& vsEntry,
	const std::string& psFileName,
	const std::string& psEntry)
{
	return false;
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
	return false;
}

bool GLES2Shader::ConstantExist(const str_type::string& name)
{
	return false;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const math::Vector4 &v)
{
	return false;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const math::Vector3 &v)
{
	return false;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const math::Vector2 &v)
{
	return false;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const float x, const float y, const float z, const float w)
{
	return false;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const float x, const float y, const float z)
{
	return false;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const float x, const float y)
{
	return false;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const float x)
{
	return false;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const int n)
{
	return false;
}

bool GLES2Shader::SetConstantArray(const str_type::string& name, unsigned int nElements, const boost::shared_array<const math::Vector2>& v)
{
	return false;
}

bool GLES2Shader::SetMatrixConstant(const str_type::string& name, const math::Matrix4x4 &matrix)
{
	return false;
}

bool GLES2Shader::SetTexture(const str_type::string& name, TextureWeakPtr pTexture)
{
	return false;
}

bool GLES2Shader::SetShader()
{
	return false;
}

} //namespace gs2d
