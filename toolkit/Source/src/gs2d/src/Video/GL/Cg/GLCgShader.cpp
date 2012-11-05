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

#include "GLCgShader.h"
#include <Cg/cgGL.h>

#include "../../../Application.h"
#include "../../../Enml/Enml.h"

#include "../../../Platform/Platform.h"

#include "../GLTexture.h"
#include "../GLVideo.h"

namespace gs2d {

CGparameter SeekParameter(const std::string& name, std::map<std::string, CGparameter>& params)
{
	std::map<std::string, CGparameter>::iterator iter = params.find(name);
	if (iter != params.end())
		return iter->second;
	return NULL;
}

GLCgShader::GLCgShader(GLVideo* video) :
	m_cgProfile(CG_PROFILE_UNKNOWN),
	m_cgProgam(NULL),
	m_focus(Shader::SF_NONE),
	m_profile(Shader::SP_NONE),
	m_shaderName("none")
{
	m_video = (GLVideo*)(video);
}

GLCgShader::~GLCgShader()
{
	m_video->RemoveRecoverableResource(this);
	DestroyCgProgram();
}

void GLCgShader::DestroyCgProgram()
{
	if (m_cgProgam)
	{
		cgDestroyProgram(m_cgProgam);
		m_cgProgam = NULL;
	}
}

Shader::SHADER_FOCUS GLCgShader::GetShaderFocus() const
{
	return m_focus;
}

Shader::SHADER_PROFILE GLCgShader::GetShaderProfile() const
{
	return m_profile;
}

CGcontext GLCgShader::ExtractCgContext(ShaderContextPtr context)
{
	return boost::any_cast<CGcontext>(context->GetContextPointer());
}

bool GLCgShader::CheckForError(const std::string& situation, const std::string& additionalInfo)
{
	CGerror error;
	const char *string = cgGetLastErrorString(&error);

	if (error != CG_NO_ERROR)
	{
		std::string text = "Situation: ";
		text.append(situation);
		text.append("\nCg error string: ");
		text.append(string);
		text.append("\nAdditional info: ");
		text.append(additionalInfo);
		if (error == CG_COMPILER_ERROR)
		{
			text += "\nCg compiler error: ";
			text += cgGetLastListing(m_cgContext);
		}
		if (error == CG_UNKNOWN_PROFILE_ERROR)
		{
			text += "\nThe current profile is not supported!";
		}
		ShowMessage(text, error == CG_COMPILER_ERROR ? GSMT_ERROR : GSMT_WARNING);
		return true;
	}
	else
	{
		return false;
	}
}

void GLCgShader::UnbindShader()
{
	cgGLUnbindProgram(m_cgProfile);
	cgGLDisableProfile(m_cgProfile);
	if (m_focus == Shader::SF_PIXEL)
		DisableTextures();
}

void GLCgShader::DisableIfEnabled(CGparameter param)
{
	for (std::list<CGparameter>::iterator iter = m_enabledTextures.begin(); iter != m_enabledTextures.end();)
	{
		if ((*iter) == param)
		{
			cgGLDisableTextureParameter((*iter));
			iter = m_enabledTextures.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void GLCgShader::DisableTextures()
{
	for (std::list<CGparameter>::iterator iter = m_enabledTextures.begin(); iter != m_enabledTextures.end(); ++iter)
	{
		cgGLDisableTextureParameter((*iter));
	}
	m_enabledTextures.clear();
	m_video->UnsetTexture(0);
	m_video->UnsetTexture(1);
	m_video->UnsetTexture(2);
	m_video->UnsetTexture(4);
}

bool GLCgShader::SetShader()
{
	cgGLBindProgram(m_cgProgam);
	if (CheckForError("cgGLBindProgram", m_shaderName))
		return false;

	cgUpdateProgramParameters(m_cgProgam);
	if (CheckForError("cgUpdateProgramParameters", m_shaderName))
		return false;

	cgGLEnableProfile(m_cgProfile);
	if (CheckForError("cgGLEnableProfile", m_shaderName))
		return false;

	return true;
}

bool GLCgShader::LoadShaderFromFile(
	ShaderContextPtr context,
	const str_type::string& fileName,
	const SHADER_FOCUS focus,
	const SHADER_PROFILE profile,
	const char *entry)
{
	const std::string& str = enml::GetStringFromAnsiFile(fileName);
	return LoadShaderFromString(context, fileName, str, focus, profile, entry);
}

bool GLCgShader::LoadShaderFromString(
	ShaderContextPtr context,
	const str_type::string& shaderName,
	const std::string& codeAsciiString,
	const SHADER_FOCUS focus,
	const SHADER_PROFILE profile,
	const char *entry)
{
	if (entry)
		m_entry = entry;
	m_shaderCode = codeAsciiString;
	m_cgContext = ExtractCgContext(context);
	m_shaderName = shaderName;
	m_focus = focus;
	m_profile = profile;

	if (focus == Shader::SF_PIXEL)
	{
		//CGprofile latestProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
		m_cgProfile = CG_PROFILE_ARBFP1;
	}
	else
	{
		#ifdef MACOSX
			m_cgProfile = CG_PROFILE_ARBVP1;
		#else
			m_cgProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
		#endif
	}

	cgGLSetOptimalOptions(m_cgProfile);
	if (CheckForError("Shader::LoadShader setting optimal options", m_shaderName))
		return false;

	if (CreateCgProgram())
	{
		m_video->InsertRecoverableResource(this);
	}
	else
	{
		return false;
	}
	return true;
}

bool GLCgShader::CreateCgProgram()
{
	DestroyCgProgram();
	m_cgProgam = cgCreateProgram(
		m_cgContext,
		CG_SOURCE,
		m_shaderCode.c_str(),
		m_cgProfile,
		m_entry.empty() ? NULL : m_entry.c_str(),
		NULL);

	if (CheckForError("Shader::LoadShader creating program from file", m_shaderName))
		return false;

	// compile shader
	cgGLLoadProgram(m_cgProgam);
	if (CheckForError("GS_SHADER::CompileShader loading the program", m_shaderName))
		return false;

	FillParameters(CG_GLOBAL);
	FillParameters(CG_PROGRAM);
	return true;
}

void GLCgShader::Recover()
{
	CreateCgProgram();
	ShowMessage("Shader recovered: " + Platform::GetFileName(m_shaderName), GSMT_INFO);
}

void GLCgShader::FillParameters(const CGenum domain)
{
	CGparameter param = cgGetFirstParameter(m_cgProgam, domain);
	while (param)
	{
		const char* name = cgGetParameterName(param);
		m_params[name] = param;
		param = cgGetNextParameter(param);
	}
}

bool GLCgShader::ConstantExist(const str_type::string& name)
{
	return (SeekParameter(name, m_params));
}

static bool ShowInvalidParameterWarning(const str_type::string& shaderName, const str_type::string& name)
{
	std::string str = "Shader - invalid parameter set to ";
	str.append(shaderName);
	str.append(": ");
	str.append(name);
	ShowMessage(str, GSMT_WARNING);
	return false;
}

bool GLCgShader::SetConstant(const str_type::string& name, const math::Vector4 &v)
{
	CGparameter param = SeekParameter(name, m_params);
	if (!param)
		return ShowInvalidParameterWarning(m_shaderName, name);

	cgSetParameter4fv(param, (float*)&v);
	if (CheckForError("Shader::SetConstant4F setting parameter", m_shaderName))
		return false;
	return true;
}

bool GLCgShader::SetConstant(const str_type::string& name, const math::Vector3 &v)
{
	CGparameter param = SeekParameter(name, m_params);
	if (!param)
		return ShowInvalidParameterWarning(m_shaderName, name);

	cgSetParameter3fv(param, (float*)&v);
	if (CheckForError("Shader::SetConstant3F setting parameter", m_shaderName))
		return false;
	return true;
}

bool GLCgShader::SetConstant(const str_type::string& name, const math::Vector2 &v)
{
	CGparameter param = SeekParameter(name, m_params);
	if (!param)
		return ShowInvalidParameterWarning(m_shaderName, name);

	cgSetParameter2fv(param, (float*)&v);
	if (CheckForError("Shader::SetConstant2F setting parameter", m_shaderName))
		return false;
	return true;
}

bool GLCgShader::SetConstant(const str_type::string& name, const float x)
{
	CGparameter param = SeekParameter(name, m_params);
	if (!param)
		return ShowInvalidParameterWarning(m_shaderName, name);

	cgSetParameter1f(param, x);
	if (CheckForError("Shader::SetConstant1F setting parameter", m_shaderName))
		return false;
	return true;
}

bool GLCgShader::SetConstant(const str_type::string& name, const Color& dw)
{
	math::Vector4 v;
	v.SetColor(dw);
	return SetConstant(name, v);
}

bool GLCgShader::SetConstant(const str_type::string& name, const float x, const float y, const float z, const float w)
{
	return SetConstant(name, math::Vector4(x, y, z, w));
}

bool GLCgShader::SetConstant(const str_type::string& name, const float x, const float y, const float z)
{
	return SetConstant(name, math::Vector3(x, y, z));
}

bool GLCgShader::SetConstant(const str_type::string& name, const float x, const float y)
{
	return SetConstant(name, math::Vector2(x, y));
}

bool GLCgShader::SetConstant(const str_type::string& name, const int n)
{
	CGparameter param = SeekParameter(name, m_params);
	if (!param)
		return ShowInvalidParameterWarning(m_shaderName, name);

	cgSetParameter1i(param, n);
	if (CheckForError("Shader::SetConstant1I setting parameter", m_shaderName))
		return false;
	return true;
}

bool GLCgShader::SetMatrixConstant(const str_type::string& name, const math::Matrix4x4 &matrix)
{
	CGparameter param = SeekParameter(name, m_params);
	if (!param)
		return ShowInvalidParameterWarning(m_shaderName, name);

	cgSetMatrixParameterfr(param, (float*)&matrix);
	if (CheckForError("Shader::SetMatrixConstant setting parameter", m_shaderName))
		return false;
	return true;
}

bool GLCgShader::SetConstantArray(const str_type::string& name, unsigned int nElements, const boost::shared_array<const math::Vector2>& v)
{
	CGparameter param = SeekParameter(name, m_params);
	if (!param)
		return ShowInvalidParameterWarning(m_shaderName, name);

	cgGLSetParameterArray2f(param, 0, (long)nElements, (float*)v.get());
	if (CheckForError("Shader::SetConstantArrayF setting parameter", m_shaderName))
		return false;
	return true;
}

bool GLCgShader::SetTexture(const str_type::string& name, TextureWeakPtr pTexture)
{
	if (m_focus != Shader::SF_PIXEL)
	{
		ShowMessage("Shader::SetTexture - textures can't be set as vertex shader parameter", GSMT_ERROR);
		return false;
	}

	CGparameter param = SeekParameter(name, m_params);

	if (!param)
		return ShowInvalidParameterWarning(m_shaderName, name);

	DisableIfEnabled(param);

	const GLTexture* textureObj = (GLTexture*)(pTexture.lock().get());
	
	cgGLSetTextureParameter(param, textureObj->GetTextureInfo().m_texture);
	if (CheckForError("Shader::SetTexture", m_shaderName))
		return false;

	cgGLEnableTextureParameter(param);
	if (CheckForError("Shader::SetTexture", m_shaderName))
		return false;

	// add this param to the enabled texture parameter list so we can disable it with DisableTetureParams
	m_enabledTextures.push_back(param);
	return true;
}

} // namespace gs2d
