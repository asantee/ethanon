#include "GLES2Shader.h"

namespace gs2d {

const int GLES2ShaderContext::INVALID_ATTRIB_LOCATION = -1;
GLuint GLES2ShaderContext::m_currentProgram = 0;
	
GLES2ShaderContext::GLES2ShaderContext(GLES2Video *pVideo) :
		m_logger(Platform::FileLogger::GetLogDirectory() + "GLES2ShaderContext.log.txt"),
		m_rectRenderer(m_logger),
		m_lastProgram(0),
		m_lastPosLocation(INVALID_ATTRIB_LOCATION),
		m_lastTexLocation(INVALID_ATTRIB_LOCATION)
{
}

GLES2ShaderContext::~GLES2ShaderContext()
{
}

boost::any GLES2ShaderContext::GetContextPointer()
{
	return 0;
}

bool GLES2ShaderContext::DisableTextureParams()
{
	return false;
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

void GLES2ShaderContext::GetLocations(int& pos, int& tex) const
{
	if (m_currentProgram != m_lastProgram)
	{
		m_lastProgram = m_currentProgram;
		tsl::hopscotch_map<GLuint, int>::const_iterator iterPos = m_vertexPosLocations.find(m_currentProgram);
		tsl::hopscotch_map<GLuint, int>::const_iterator iterTex = m_texCoordLocations.find(m_currentProgram);
		if (iterPos != m_vertexPosLocations.end() && iterTex != m_texCoordLocations.end())
		{
			m_lastPosLocation = iterPos->second;
			m_lastTexLocation = iterTex->second;
		}
		else
		{
			pos = tex = INVALID_ATTRIB_LOCATION;
			return;
		}
	}
	pos = m_lastPosLocation;
	tex = m_lastTexLocation;
}

bool GLES2ShaderContext::FindLocations(const GLuint program)
{
	m_vertexPosLocations[program] = glGetAttribLocation(program, "vPosition");
	m_texCoordLocations[program] = glGetAttribLocation(program, "vTexCoord");
	return !CheckForError("GLES2ShaderContext::FindLocations - glGetAttribLocation");
}

void GLES2ShaderContext::DrawRect(Video* video, const Sprite::RECT_MODE mode)
{
	int pos, tex;
	GetLocations(pos, tex);
	if (pos != INVALID_ATTRIB_LOCATION && tex != INVALID_ATTRIB_LOCATION)
	{
		video->GetCurrentShader()->SetShader();
		m_rectRenderer.Draw(pos, tex, mode, m_logger);
	}
	else
	{
		m_logger.Log("DrawRect - could not find the current program in the hash", Platform::FileLogger::ERROR);
	}
}

void GLES2ShaderContext::BeginFastDraw(Video* video)
{
	tsl::hopscotch_map<GLuint, int>::const_iterator iterPos = m_vertexPosLocations.find(m_currentProgram);
	tsl::hopscotch_map<GLuint, int>::const_iterator iterTex = m_texCoordLocations.find(m_currentProgram);
	if (iterPos != m_vertexPosLocations.end() && iterTex != m_texCoordLocations.end())
	{
		video->GetCurrentShader()->SetShader();
		m_rectRenderer.BeginFastDraw(iterPos->second, iterTex->second, m_logger);
	}
	else
	{
		m_logger.Log("BeginFastDraw - could not find the current program in the hash", Platform::FileLogger::ERROR);
	}
}

void GLES2ShaderContext::EndFastDraw(Video* video)
{
	m_rectRenderer.EndFastDraw(m_logger);
}

void GLES2ShaderContext::FastDraw(Video* video)
{
	m_rectRenderer.FastDraw(m_logger);
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
