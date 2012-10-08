/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#include "GLES2Shader.h"

#include "../../Video.h"
#include <sstream>
#include <boost/functional/hash/hash.hpp>

namespace gs2d {

GLenum GetGLShaderType(const GS_SHADER_FOCUS sf)
{
	switch(sf)
	{
	case GSSF_PIXEL:
		return GL_FRAGMENT_SHADER;
	case GSSF_VERTEX:
		return GL_VERTEX_SHADER;
	default:
		return GL_VERTEX_SHADER; // TODO return an invalid type code
	}
}

GLenum GetEquivalentTexturePass(const GLint pass)
{
	switch (pass)
	{
	case 0:
		return GL_TEXTURE0;
	case 1:
		return GL_TEXTURE1;
	case 2:
		return GL_TEXTURE2;
	case 3:
		return GL_TEXTURE3;
	case 4:
		return GL_TEXTURE4;
	case 5:
		return GL_TEXTURE5;
	case 6:
		return GL_TEXTURE6;
	case 7:
		return GL_TEXTURE7;
	case 8:
		return GL_TEXTURE8;
	default:
		return GL_TEXTURE1;
	}
}

const int GLES2ShaderContext::INVALID_ATTRIB_LOCATION = -1;
GLuint GLES2ShaderContext::m_currentProgram = 0;
	
GLES2ShaderContext::GLES2ShaderContext(GLES2Video *pVideo) :
		m_logger(Platform::FileLogger::GetLogDirectory() + "GLES2ShaderContext.log.txt"),
		m_rectRenderer(m_logger),
		m_spriteDepthValue(0.0f),
		m_lastProgram(0),
		m_lastPosLocation(INVALID_ATTRIB_LOCATION),
		m_lastTexLocation(INVALID_ATTRIB_LOCATION)
{
	GLES2UniformParameter::ClearParams();
}

GLES2ShaderContext::~GLES2ShaderContext()
{
	// since android doesn't tell us when it wants to unload resources, it must be this way
	#ifndef ANDROID
	for (std::map<std::size_t, GLuint>::iterator iter = m_programs.begin();
		iter != m_programs.end(); iter++) 
	{
		glDeleteProgram(iter->second);
	}
	#endif
}

boost::any GLES2ShaderContext::GetContextPointer()
{
	return 0; // OpenGL ES has no context object
}

bool GLES2ShaderContext::DisableTextureParams()
{
	// TODO
	return false;
}

void GLES2ShaderContext::Log(const str_type::string& str, const Platform::FileLogger::TYPE& type) const
{
	m_logger.Log(str, type);
}

bool GLES2ShaderContext::SetSpriteDepth(const float depth)
{
	m_spriteDepthValue = depth;
	return true;
}

float GLES2ShaderContext::GetSpriteDepth() const
{
	return m_spriteDepthValue;
}

size_t GLES2ShaderContext::ComputeProgramHashValue(GLES2ShaderPtr vertex, GLES2ShaderPtr pixel) const
{
	std::size_t seed = 0;
	boost::hash_combine(seed, vertex->GetShader());
	boost::hash_combine(seed, pixel->GetShader());
	return seed;
}

bool GLES2ShaderContext::FindLocations(const GLuint program)
{
	m_vertexPosLocations[program] = glGetAttribLocation(program, "vPosition");
	m_texCoordLocations[program] = glGetAttribLocation(program, "vTexCoord");
	return !CheckForError("GLES2ShaderContext::FindLocations - glGetAttribLocation");
}

void GLES2ShaderContext::GetLocations(int& pos, int& tex) const
{
	if (m_currentProgram != m_lastProgram)
	{
		m_lastProgram = m_currentProgram;
		std::map<GLuint, int>::const_iterator iterPos = m_vertexPosLocations.find(m_currentProgram);
		std::map<GLuint, int>::const_iterator iterTex = m_texCoordLocations.find(m_currentProgram);
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

void GLES2ShaderContext::DrawRect(const Sprite::RECT_MODE mode)
{
	CreateProgram();
	int pos, tex;
	GetLocations(pos, tex);
	if (pos != INVALID_ATTRIB_LOCATION && tex != INVALID_ATTRIB_LOCATION)
	{
		SetUniformParametersFromCurrentProgram(m_currentVS);
		SetUniformParametersFromCurrentProgram(m_currentPS);
		m_rectRenderer.Draw(pos, tex, mode, m_logger);
	}
	else
	{
		m_logger.Log("DrawRect - could not find the current program in the hash", Platform::FileLogger::ERROR);
	}
}

void GLES2ShaderContext::BeginFastDraw()
{
	CreateProgram();
	std::map<GLuint, int>::const_iterator iterPos = m_vertexPosLocations.find(m_currentProgram);
	std::map<GLuint, int>::const_iterator iterTex = m_texCoordLocations.find(m_currentProgram);
	if (iterPos != m_vertexPosLocations.end() && iterTex != m_texCoordLocations.end())
	{
		m_rectRenderer.BeginFastDraw(iterPos->second, iterTex->second, m_logger);
	}
	else
	{
		m_logger.Log("BeginFastDraw - could not find the current program in the hash", Platform::FileLogger::ERROR);
	}
}

void GLES2ShaderContext::EndFastDraw()
{
	m_rectRenderer.EndFastDraw(m_logger);
}

void GLES2ShaderContext::FastDraw()
{
	SetUniformParametersFromCurrentProgram(m_currentPS);
	SetUniformParametersFromCurrentProgram(m_currentVS);
	m_rectRenderer.FastDraw(m_logger);
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

void GLES2ShaderContext::SetShader(GLES2ShaderPtr shader)
{
	switch (shader->GetShaderFocus())
	{
	case GSSF_VERTEX:
		m_currentVS = shader;
		break;
	case GSSF_PIXEL:
		m_currentPS = shader;
		break;
	default:
		break; // prevents "unhandled value" warnings
	}
}

void GLES2ShaderContext::CreateProgram()
{
	GLuint currentProgram = 0;
	{
		const std::size_t programHash = ComputeProgramHashValue(m_currentVS, m_currentPS);

		std::map<std::size_t, GLuint>::iterator iter = m_programs.find(programHash);
		if (iter == m_programs.end())
		{
			std::stringstream ss;
			ss  << "#" << programHash << " - " << m_currentVS->GetShaderName() << " @ " << m_currentPS->GetShaderName();
			const str_type::string programName = ss.str(); 

			const GLuint program = CreateProgramFromCurrentShaders();
			if (program)
			{
				m_programs[programHash] = program;
				currentProgram = program;
				m_logger.Log(programName + " successfully created", Platform::FileLogger::INFO);
				if (!FindLocations(currentProgram))
				{
					m_logger.Log(m_currentVS->GetShaderName() + m_currentPS->GetShaderName() + " does not have necessary attribs", Platform::FileLogger::ERROR);
				}
			}
			else
			{
				m_logger.Log(programName + " failed", Platform::FileLogger::ERROR);
			}
		}
		else
		{
			currentProgram = iter->second;
		}

		if (m_currentProgram != currentProgram)
		{
			m_currentProgram = currentProgram;
			glUseProgram(currentProgram);
			CheckForError("glUseProgram");
		}
	}
}

void GLES2ShaderContext::SetShader(GLES2ShaderPtr vs, GLES2ShaderPtr ps, const math::Matrix4x4 &ortho, const math::Vector2& screenSize)
{
	// Optimization: don't swap shaders if it's the same
	if (m_currentVS == vs && m_currentPS == ps)
	{
		return;
	}

	SetShader(vs);
	SetShader(ps);
	
	m_ortho = ortho;
	m_screenSize = screenSize;
	if (ps && vs)
	{
		ResetViewConstants(m_ortho, m_screenSize);
	}
}

void GLES2ShaderContext::ResetViewConstants(const math::Matrix4x4 &ortho, const math::Vector2& screenSize)
{
	m_currentVS->SetConstant("screenSize", screenSize);
	m_currentVS->SetMatrixConstant("viewMatrix", ortho);
}

void GLES2ShaderContext::SetUniformParametersFromCurrentProgram(GLES2ShaderPtr shader) const
{
	const std::map<std::size_t, GLES2UniformParameterPtr>& parameters = shader->GetParameters();
	for (std::map<std::size_t, GLES2UniformParameterPtr>::const_iterator iter =
			parameters.begin(); iter != parameters.end(); iter++)
	{
		GLES2UniformParameter* param = iter->second.get();
		param->SetParameter(m_currentProgram, param->GetName(), m_logger);
	}
}

GLuint GLES2ShaderContext::CreateProgramFromCurrentShaders()
{
	std::stringstream ss;
	ss << m_currentVS->GetShaderName() << "(" << m_currentVS->GetShader() << ") / " <<  m_currentPS->GetShaderName()
	<< " (" << m_currentPS->GetShader() << ")";
	 const str_type::string programName = ss.str();

	const GLuint program = glCreateProgram();
	if (program)
	{
		glAttachShader(program, m_currentVS->GetShader());
		CheckForError(programName + " glAttachShader (vertex)");
		
		glAttachShader(program, m_currentPS->GetShader());
		CheckForError(programName + " glAttachShader (pixel)");

		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE)
		{
			m_logger.Log(programName + ": link failed", Platform::FileLogger::ERROR);
			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength)
			{
				char* buf = new char [bufLength];
				if (buf)
				{
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					m_logger.Log(str_type::string("could not link program: ") + programName + buf, Platform::FileLogger::ERROR);
					delete [] buf;
				}
			}
			glDeleteProgram(program);
			return 0;
		}
	}
	return program;
}

GLES2ShaderPtr GLES2ShaderContext::GetCurrentVS()
{
	return m_currentVS;
}

GLES2ShaderPtr GLES2ShaderContext::GetCurrentPS()
{
	return m_currentPS;
}

GLES2Shader::GLES2Shader(Platform::FileManagerPtr fileManager, GLES2ShaderContextPtr context) :
	DIFFUSE_TEXTURE_NAME("diffuse"),
	m_zip(fileManager),
	m_context(context),
	m_shader(0),
	m_shaderFocus(GSSF_NONE),
	m_texturePassCounter(0)
{
	m_texturePasses[DIFFUSE_TEXTURE_NAME] = m_texturePassCounter++;
}

GLES2Shader::~GLES2Shader()
{
	// since android doesn't tell us when it unloads resources, it must be this way
	#ifndef ANDROID
	if (m_shader != 0)
	{
		glDeleteShader(m_shader);
	}
	#endif
}

GLuint GLES2Shader::GetShader()
{
	return m_shader;
}

const std::map<std::size_t, GLES2UniformParameterPtr>& GLES2Shader::GetParameters() const
{
	return m_parameters;
}

str_type::string GLES2Shader::GetShaderName()
{
	return m_shaderName;
}

bool GLES2Shader::LoadShaderFromFile(ShaderContextPtr context, const str_type::string& fileName,
		const GS_SHADER_FOCUS focus, const GS_SHADER_PROFILE profile, const char *entry)
{
	str_type::string content;
	if (!m_zip->GetAnsiFileString(fileName, content))
	{
		m_context->Log(str_type::string("couldn't open ") + fileName, Platform::FileLogger::ERROR);
		return false;
	}
	return LoadShaderFromString(context, fileName, content, focus, profile, 0);
}

bool GLES2Shader::LoadShaderFromString(ShaderContextPtr context, const str_type::string& shaderName,
		const std::string& codeAsciiString, const GS_SHADER_FOCUS focus,
		const GS_SHADER_PROFILE profile, const char *entry)
{
	if (m_shader != 0)
		return false;

	// pVideo->Message(codeAsciiString, GSMT_INFO);
	const GLuint shader = glCreateShader(GetGLShaderType(focus));
	if (shader)
	{
		if (m_shaderName == "")
			m_shaderName = shaderName;

		m_shaderFocus = focus;
		m_logger = boost::shared_ptr<Platform::FileLogger>(
				new Platform::FileLogger(Platform::FileLogger::GetLogDirectory() + Platform::GetFileName(shaderName) + ".log.txt"));
		m_shader = shader;
		const char *pSource = codeAsciiString.c_str();
		glShaderSource(shader, 1, &pSource, NULL);

		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			m_context->Log(str_type::string("couldn't compile ") + Platform::GetFileName(shaderName), Platform::FileLogger::ERROR);
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

			if (infoLen)
			{
				char* buf = new char [infoLen];
				if (buf)
				{
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					std::stringstream ss;
					ss << "Error: could not compile shader " << shaderName << " ("
							<< buf << ")";
					m_logger->Log(ss.str(), Platform::FileLogger::ERROR);
					m_context->Log(ss.str(), Platform::FileLogger::ERROR);
					delete [] buf;
				}
				glDeleteShader(shader);
				m_shader = 0;
				return false;
			}
		}
		else
		{
			m_context->Log(shaderName + " successfully created", Platform::FileLogger::INFO);
			// TODO glReleaseShaderCompiler() ?
		}
	}
	else
	{
		std::stringstream ss;
		ss << "Error: could not create shader " << shaderName;
		m_logger->Log(ss.str(), Platform::FileLogger::ERROR);
	}
	return true;
}

bool GLES2Shader::ConstantExist(const str_type::string& name)
{
	// TODO
	return false;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const Color& dw)
{
	math::Vector4 v;
	v.SetColor(dw);
	return SetConstant(name, v);
}

inline void SetParameter(
	const std::size_t nameHash,
	const GLES2UniformParameterPtr& param,
	std::map<std::size_t, GLES2UniformParameterPtr>& parameters)
{
	std::map<std::size_t, GLES2UniformParameterPtr>::iterator iter = parameters.find(nameHash);
	if (iter != parameters.end())
	{
		param->SetLocations(iter->second->GetLocations());
		iter->second = param;
	}
	else
	{
		parameters[nameHash] = param;
	}
}

inline void SetParameter(const str_type::string& name, const GLES2UniformParameterPtr& param,
						 std::map<std::size_t, GLES2UniformParameterPtr>& parameters)
{
	SetParameter(fastHash(name), param, parameters);
}

bool GLES2Shader::SetConstant(const std::size_t nameHash, const str_type::string& name, const math::Vector4 &v)
{
	SetParameter(nameHash, GLES2UPVec4Ptr(new GLES2UPVec4(v, name)), m_parameters);
	return true;
}

bool GLES2Shader::SetConstant(const std::size_t nameHash, const str_type::string& name, const math::Vector3 &v)
{
	SetParameter(nameHash, GLES2UPVec3Ptr(new GLES2UPVec3(v, name)), m_parameters);
	return true;
}

bool GLES2Shader::SetConstant(const std::size_t nameHash, const str_type::string& name, const math::Vector2 &v)
{
	SetParameter(nameHash, GLES2UPVec2Ptr(new GLES2UPVec2(v, name)), m_parameters);
	return true;
}

bool GLES2Shader::SetConstant(const std::size_t nameHash, const str_type::string& name, const float x)
{
	SetParameter(nameHash, GLES2UPVec1Ptr(new GLES2UPVec1(x, name)), m_parameters);
	return true;
}

bool GLES2Shader::SetMatrixConstant(const std::size_t nameHash, const str_type::string& name, const math::Matrix4x4 &matrix)
{
	SetParameter(nameHash, GLES2UPMat4x4Ptr(new GLES2UPMat4x4(matrix, name)), m_parameters);
	return true;
}

bool GLES2Shader::SetConstantArray(const std::size_t nameHash, const str_type::string& name, unsigned int nElements,
								   const boost::shared_array<const math::Vector2>& v)
{
	SetParameter(nameHash, GLES2UPVec2ArrayPtr(new GLES2UPVec2Array(v, nElements, name)), m_parameters);
	return true;
}

bool GLES2Shader::SetConstant(const std::size_t nameHash, const str_type::string& name, const Color& dw)
{
	math::Vector4 v;
	v.SetColor(dw);
	return SetConstant(nameHash, name, v);
}

bool GLES2Shader::SetConstant(const str_type::string& name, const math::Vector4 &v)
{
	//m_parameters[name] = GLES2UPVec4Ptr(new GLES2UPVec4(v));
	SetParameter(name, GLES2UPVec4Ptr(new GLES2UPVec4(v, name)), m_parameters);
	return true;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const math::Vector3 &v)
{
	//m_parameters[name] = GLES2UPVec3Ptr(new GLES2UPVec3(v));
	SetParameter(name, GLES2UPVec3Ptr(new GLES2UPVec3(v, name)), m_parameters);
	return true;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const math::Vector2 &v)
{
	//m_parameters[name] = GLES2UPVec2Ptr(new GLES2UPVec2(v));
	SetParameter(name, GLES2UPVec2Ptr(new GLES2UPVec2(v, name)), m_parameters);
	return true;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const float x, const float y, const float z, const float w)
{
	return SetConstant(name, math::Vector4(x,y,z,w));
}

bool GLES2Shader::SetConstant(const str_type::string& name, const float x, const float y, const float z)
{
	return SetConstant(name, math::Vector3(x,y,z));
}

bool GLES2Shader::SetConstant(const str_type::string& name, const float x, const float y)
{
	return SetConstant(name, math::Vector2(x,y));
}

bool GLES2Shader::SetConstant(const str_type::string& name, const float x)
{
	//m_parameters[name] = GLES2UPVec1Ptr(new GLES2UPVec1(x));
	SetParameter(name, GLES2UPVec1Ptr(new GLES2UPVec1(x, name)), m_parameters);
	return true;
}

bool GLES2Shader::SetConstant(const str_type::string& name, const int n)
{
	// TODO
	return false;
}

bool GLES2Shader::SetConstantArray(const str_type::string& name, unsigned int nElements, const boost::shared_array<const math::Vector2>& v)
{
	SetParameter(name, GLES2UPVec2ArrayPtr(new GLES2UPVec2Array(v, nElements, name)), m_parameters);
	return true;
}

bool GLES2Shader::SetMatrixConstant(const str_type::string& name, const math::Matrix4x4 &matrix)
{
	//m_parameters[name] = GLES2UPMat4x4Ptr(new GLES2UPMat4x4(matrix));
	SetParameter(name, GLES2UPMat4x4Ptr(new GLES2UPMat4x4(matrix, name)), m_parameters);
	return true;
}

bool GLES2Shader::SetTexture(const str_type::string& name, TextureWeakPtr pTexture)
{
	std::map<str_type::string, GLint>::iterator iter = m_texturePasses.find(name);
	if (iter == m_texturePasses.end())
	{
		m_texturePasses[name] = m_texturePassCounter++;
		iter = m_texturePasses.find(name);
	}

	const GLint pass = iter->second;

	GLES2Texture* tex = static_cast<GLES2Texture*>(pTexture.lock().get());
	if (tex)
	{
		//m_parameters[name] = GLES2UPTexturePtr(new GLES2UPTexture(GetEquivalentTexturePass(pass), tex->GetTextureID(), pass));
		SetParameter(name, GLES2UPTexturePtr(new GLES2UPTexture(GetEquivalentTexturePass(pass), tex->GetTextureID(), pass, name)), m_parameters);
		/*if (name == DIFFUSE_TEXTURE_NAME)
		{
			m_parameters[name] = GLES2UPTexturePtr(new GLES2UPTexture(GL_TEXTURE0, tex->GetTextureID(), 0));
		}
		else
		{
			m_parameters[name] = GLES2UPTexturePtr(new GLES2UPTexture(GL_TEXTURE1, tex->GetTextureID(), 1));
		}*/
		return true;
	}
	else
	{
		return false;
	}
}

bool GLES2Shader::SetShader()
{
	return false;
}

GS_SHADER_FOCUS GLES2Shader::GetShaderFocus() const
{
	return m_shaderFocus;
}

GS_SHADER_PROFILE GLES2Shader::GetShaderProfile() const
{
	return GSSP_NONE;
}

void GLES2Shader::UnbindShader()
{
	// TODO
}

} //namespace gs2d