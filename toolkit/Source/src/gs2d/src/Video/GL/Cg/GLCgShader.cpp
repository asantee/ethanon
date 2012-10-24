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

namespace gs2d {

Shader::SHADER_PROFILE CGProfileToGSProfile(const CGprofile prof)
{
	switch (prof)
	{
	case CG_PROFILE_ARBFP1:
	case CG_PROFILE_ARBVP1:
		return Shader::SP_MODEL_1;
	case CG_PROFILE_FP20:
	case CG_PROFILE_VP20:
		return Shader::SP_MODEL_2;
	case CG_PROFILE_FP30:
	case CG_PROFILE_VP30:
		return Shader::SP_MODEL_3;
	default:
		return Shader::SP_MODEL_3;
	}
	return Shader::SP_NONE;
}

CGparameter SeekParameter(const std::string& name, std::map<std::string, CGparameter>& params)
{
	std::map<std::string, CGparameter>::iterator iter = params.find(name);
	if (iter != params.end())
		return iter->second;
	return NULL;
}

GLCgShader::GLCgShader() :
	m_cgProfile(CG_PROFILE_UNKNOWN),
	m_cgProgam(NULL),
	m_focus(Shader::SF_NONE),
	m_profile(Shader::SP_NONE),
	m_shaderName("none")
{
}

GLCgShader::~GLCgShader()
{
	cgDestroyProgram(m_cgProgam);
	m_cgProgam = NULL;
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
		bool r = false;
		std::string text = "Situation: ";
		text.append(situation);
		text.append("\nCg error string: ");
		text.append(string);
		text.append("\nAdditional info: ");
		text.append(additionalInfo);
		if (error == CG_COMPILER_ERROR)
		{
			r = true;
			text += "\nCg compiler error: ";
			text += cgGetLastListing(m_cgContext);
		}
		if (error == CG_UNKNOWN_PROFILE_ERROR)
		{
			r = true;
			text += "\nThe current profile is not supported!";
		}
		ShowMessage(text, error == CG_COMPILER_ERROR ? GSMT_ERROR : GSMT_WARNING);
		return true;
	}
	return false;
}

void GLCgShader::UnbindShader()
{
	cgGLUnbindProgram(m_cgProfile);
	if (CheckForError("Shader::UnbindShader", m_shaderName))
		return;
}

bool GLCgShader::SetShader()
{
	CGprofile latestProfile;
	if (m_focus == SF_PIXEL)
	{
		latestProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
	}
	else
	{
		latestProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
	}

	if (latestProfile != CG_PROFILE_UNKNOWN)
	{
		cgGLDisableProfile(latestProfile);
		if (CheckForError("cgGLDisableProfile", m_shaderName))
			return false;
	}

	cgGLSetContextOptimalOptions(m_cgContext, m_cgProfile);
	if (CheckForError("cgGLSetContextOptimalOptions", m_shaderName))
		return false;

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
	m_cgContext = ExtractCgContext(context);
	m_shaderName = shaderName;
	m_focus = focus;
	m_profile = profile;

	CGprofile latestProfile;
	if (focus == Shader::SF_PIXEL)
	{
		latestProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
		switch (profile)
		{
		case Shader::SP_MODEL_1:
			m_cgProfile = CG_PROFILE_ARBFP1;
			break;
		case Shader::SP_MODEL_2:
			m_cgProfile = CG_PROFILE_FP20;
			break;
		case Shader::SP_MODEL_3:
			m_cgProfile = CG_PROFILE_FP30;
			break;
		default:
			m_cgProfile = latestProfile;
		};
	}
	else
	{
		latestProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
		switch (profile)
		{
		case Shader::SP_MODEL_1:
			m_cgProfile = CG_PROFILE_ARBVP1;
			break;
		case Shader::SP_MODEL_2:
			m_cgProfile = CG_PROFILE_VP20;
			break;
		case Shader::SP_MODEL_3:
			m_cgProfile = CG_PROFILE_VP30;
			break;
		default:
			m_cgProfile = latestProfile;
		};
	}

	if (profile > CGProfileToGSProfile(latestProfile))
	{
		m_cgProfile = latestProfile;
		ShowMessage("Shader::LoadShader the current profile is not supported", GSMT_ERROR);
	}

	const char **profileOpts;
	profileOpts = cgGLGetOptimalOptions(m_cgProfile);
	if (CheckForError("Shader::LoadShader getting optimal options", m_shaderName))
		return false;

	m_cgProgam = cgCreateProgram(
		m_cgContext,
		CG_SOURCE,
		codeAsciiString.c_str(),
		m_cgProfile,
		entry,
		profileOpts);

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

void GLCgShader::FillParameters(const CGenum domain)
{
	CGparameter param = cgGetFirstParameter(m_cgProgam, domain);
	while (param)
	{
		m_params[cgGetParameterName(param)] = param;
		param = cgGetNextParameter(param);
	}
}

bool GLCgShader::ConstantExist(const str_type::string& name)
{
	return (SeekParameter(name, m_params));
}

bool GLCgShader::SetConstant(const str_type::string& name, const math::Vector4 &v)
{
	CGparameter param = SeekParameter(name, m_params);
	if (!param)
	{
		std::string str = "Shader::Set(*) invalid parameter: "; str.append(name);
		ShowMessage(str, GSMT_ERROR);
		return false;
	}

	cgSetParameter4fv(param, (float*)&v);
	if (CheckForError("Shader::SetConstant4F setting parameter", m_shaderName))
		return false;
	return true;
}

bool GLCgShader::SetConstant(const str_type::string& name, const math::Vector3 &v)
{
	CGparameter param = SeekParameter(name, m_params);
	if (!param)
	{
		std::string str = "Shader::Set(*) invalid parameter: "; str.append(name);
		ShowMessage(str, GSMT_ERROR);
		return false;
	}

	cgSetParameter3fv(param, (float*)&v);
	if (CheckForError("Shader::SetConstant3F setting parameter", m_shaderName))
		return false;
	return true;
}

bool GLCgShader::SetConstant(const str_type::string& name, const math::Vector2 &v)
{
	CGparameter param = SeekParameter(name, m_params);
	if (!param)
	{
		std::string str = "Shader::Set(*) invalid parameter: "; str.append(name);
		ShowMessage(str, GSMT_ERROR);
		return false;
	}

	cgSetParameter2fv(param, (float*)&v);
	if (CheckForError("Shader::SetConstant2F setting parameter", m_shaderName))
		return false;
	return true;
}

bool GLCgShader::SetConstant(const str_type::string& name, const float x)
{
	CGparameter param = SeekParameter(name, m_params);
	if (!param)
	{
		std::string str = "Shader::Set(*) invalid parameter: "; str.append(name);
		ShowMessage(str, GSMT_ERROR);
		return false;
	}

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
	{
		std::string str = "Shader::Set(*) invalid parameter: "; str.append(name);
		ShowMessage(str, GSMT_ERROR);
		return false;
	}

	cgSetParameter1i(param, n);
	if (CheckForError("Shader::SetConstant1I setting parameter", m_shaderName))
		return false;
	return true;
}

bool GLCgShader::SetMatrixConstant(const str_type::string& name, const math::Matrix4x4 &matrix)
{
	CGparameter param = SeekParameter(name, m_params);
	if (!param)
	{
		std::string str = "Shader::Set(*) invalid parameter: "; str.append(name);
		ShowMessage(str, GSMT_ERROR);
		return false;
	}

	cgSetMatrixParameterfr(param, (float*)&matrix);
	if (CheckForError("Shader::SetMatrixConstant setting parameter", m_shaderName))
		return false;
	return true;
}

bool GLCgShader::SetTexture(const str_type::string& name, TextureWeakPtr pTexture)
{
	/*GSSHADER_DATA *pShader = (GSSHADER_DATA*)m_pData;
	if (!pShader || !m_pHandler)
	{
		gsSetErrorMessage("GS_SHADER::SetTexture - The shader object or it's context are invalid.", false, NULL);
		return false;
	}

	if (m_Focus != GSSF_PIXEL)
	{
		gsSetErrorMessage("GS_SHADER::SetTexture - textures can't be set as vertex shader parameter", false, NULL);
		return false;
	}

	CGparameter param = SeekParameter(szName, pShader->mParam);

	if (!param)
	{
		string strMsg = "GS_SHADER::Set(*) invalid parameter: ";
		strMsg += szName;
		gsSetErrorMessage(strMsg.c_str(), false, NULL);
		return false;
	}

	cgGLSetTextureParameter(param, (GLuint)pTexture);
	if (CheckForError("GS_SHADER::SetTexture", m_szShaderName))
		return false;

	cgGLEnableTextureParameter(param);
	if (CheckForError("GS_SHADER::SetTexture", m_szShaderName))
		return false;

	// add this param to the enabled texture parameter list so we can disable it with DisableTetureParams
	g_TextureParams.push_back(param);*/
	return true;
}
















bool GLCgShader::SetConstantArray(const str_type::string& name, unsigned int nElements, const boost::shared_array<const math::Vector2>& v)
{
	#warning TODO
	return false;
}

} // namespace gs2d
