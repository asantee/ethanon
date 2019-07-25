#include "GLCgShader.h"
#include <Cg/cgGL.h>

#include "../../../Application.h"
#include "../../../Enml/Enml.h"

#include "../../../Platform/Platform.h"

#include "../GLTexture.h"
#include "../GLVideo.h"

namespace gs2d {

CGparameter SeekParameter(const std::string& name, CGprogram vs, CGprogram ps)
{
	CGparameter param = cgGetNamedParameter(vs, name.c_str());
	if (param)
	{
		return param;
    }
    else
    {
    	return cgGetNamedParameter(ps, name.c_str());
    }
}

GLCgShader::GLCgShader(GLVideo* video) :
	m_cgVsProfile(CG_PROFILE_UNKNOWN),
    m_cgPsProfile(CG_PROFILE_UNKNOWN),
	m_cgVsProgam(NULL),
    m_cgPsProgam(NULL),
	m_vsShaderName("none"),
	m_psShaderName("none")
{
	m_video = (GLVideo*)(video);
}

GLCgShader::~GLCgShader()
{
	m_video->RemoveRecoverableResource(this);
	DestroyCgProgram(&m_cgVsProgam);
    DestroyCgProgram(&m_cgPsProgam);
}

void GLCgShader::DestroyCgProgram(CGprogram* outProgram)
{
	if (*outProgram)
	{
		cgDestroyProgram(*outProgram);
		*outProgram = NULL;
	}
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
	cgGLUnbindProgram(m_cgVsProfile);
	cgGLDisableProfile(m_cgVsProfile);

    cgGLUnbindProgram(m_cgPsProfile);
    cgGLDisableProfile(m_cgPsProfile);

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
}

bool GLCgShader::SetShader()
{
	{
        cgGLBindProgram(m_cgVsProgam);
        if (CheckForError("cgGLBindProgram", m_vsShaderName))
            return false;

        cgUpdateProgramParameters(m_cgVsProgam);
        if (CheckForError("cgUpdateProgramParameters", m_vsShaderName))
            return false;

        cgGLEnableProfile(m_cgVsProfile);
        if (CheckForError("cgGLEnableProfile", m_vsShaderName))
            return false;
    }
    {
        cgGLBindProgram(m_cgPsProgam);
        if (CheckForError("cgGLBindProgram", m_psShaderName))
            return false;

        cgUpdateProgramParameters(m_cgPsProgam);
        if (CheckForError("cgUpdateProgramParameters", m_psShaderName))
            return false;

        cgGLEnableProfile(m_cgPsProfile);
        if (CheckForError("cgGLEnableProfile", m_psShaderName))
            return false;
    }

	return true;
}

bool GLCgShader::LoadShaderFromFile(
	ShaderContextPtr context,
	const std::string& vsFileName,
	const std::string& vsEntry,
	const std::string& psFileName,
	const std::string& psEntry)
{
	const std::string& vsStr = enml::GetStringFromAnsiFile(vsFileName);
    const std::string& psStr = enml::GetStringFromAnsiFile(psFileName);
	return LoadShaderFromString(context, vsFileName, vsStr, vsEntry, psFileName, psStr, psEntry);
}

bool GLCgShader::LoadShaderFromString(
	ShaderContextPtr context,
    const std::string& vsShaderName,
    const std::string& vsCodeAsciiString,
    const std::string& vsEntry,
    const std::string& psShaderName,
    const std::string& psCodeAsciiString,
    const std::string& psEntry)
{
	m_vsEntry = vsEntry;
	m_psEntry = psEntry;

    m_cgContext = ExtractCgContext(context);

	m_vsShaderCode = vsCodeAsciiString;
	m_vsShaderName = vsShaderName;
	m_cgVsProfile = cgGLGetLatestProfile(CG_GL_VERTEX);

    m_psShaderCode = psCodeAsciiString;
    m_psShaderName = psShaderName;
	m_cgPsProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);

	m_shaderPairName = m_vsShaderName + "-" + m_psShaderName;

	if (CreateCgProgram(&m_cgVsProgam, m_vsShaderCode, m_cgVsProfile, m_vsShaderName, m_vsEntry) &&
		CreateCgProgram(&m_cgPsProgam, m_psShaderCode, m_cgPsProfile, m_psShaderName, m_psEntry))
	{
		m_video->InsertRecoverableResource(this);
	}
	else
	{
		return false;
	}
	return true;
}

bool GLCgShader::CreateCgProgram(
	CGprogram* outProgram,
	const std::string& shaderCode,
	CGprofile profile,
	const std::string& shaderName,
	const std::string& entry)
{
    cgGLSetOptimalOptions(profile);
    if (CheckForError("Shader::LoadShader setting optimal options", shaderName))
        return false;

	DestroyCgProgram(outProgram);

	*outProgram = cgCreateProgram(
		m_cgContext,
		CG_SOURCE,
		shaderCode.c_str(),
		profile,
		entry.empty() ? NULL : entry.c_str(),
		NULL);

	if (CheckForError("Shader::LoadShader creating program from file", shaderName))
		return false;

	// compile shader
	cgGLLoadProgram(*outProgram);
	if (CheckForError("GS_SHADER::CompileShader loading the program", shaderName))
		return false;

	return true;
}

void GLCgShader::Recover()
{
	CreateCgProgram(&m_cgVsProgam, m_vsShaderCode, m_cgVsProfile, m_vsShaderName, m_vsEntry);
	CreateCgProgram(&m_cgPsProgam, m_psShaderCode, m_cgPsProfile, m_psShaderName, m_psEntry);

	ShowMessage("Shader recovered: " + Platform::GetFileName(m_shaderPairName), GSMT_INFO);
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

void GLCgShader::SetConstant(const str_type::string& name, const math::Vector4 &v)
{
	CGparameter param = SeekParameter(name, m_cgVsProgam, m_cgPsProgam);
	if (!param)
	{
		ShowInvalidParameterWarning(m_shaderPairName, name);
		return;
  	}

	cgGLSetParameter4fv(param, (float*)&v);
	if (CheckForError("Shader::SetConstant4F setting parameter", m_shaderPairName))
		return;
}

void GLCgShader::SetConstant(const str_type::string& name, const math::Vector3 &v)
{
	CGparameter param = SeekParameter(name, m_cgVsProgam, m_cgPsProgam);
	if (!param)
	{
		ShowInvalidParameterWarning(m_shaderPairName, name);
  		return;
    }

	cgGLSetParameter3fv(param, (float*)&v);
	if (CheckForError("Shader::SetConstant3F setting parameter", m_shaderPairName))
		return;
}

void GLCgShader::SetConstant(const str_type::string& name, const math::Vector2 &v)
{
	CGparameter param = SeekParameter(name, m_cgVsProgam, m_cgPsProgam);
	if (!param)
	{
		ShowInvalidParameterWarning(m_shaderPairName, name);
		return;
  	}

	cgGLSetParameter2fv(param, (float*)&v);
	if (CheckForError("Shader::SetConstant2F setting parameter", m_shaderPairName))
		return;
}

void GLCgShader::SetConstant(const str_type::string& name, const float x)
{
	CGparameter param = SeekParameter(name, m_cgVsProgam, m_cgPsProgam);
	if (!param)
	{
		ShowInvalidParameterWarning(m_shaderPairName, name);
  		return;
    }

	cgGLSetParameter1f(param, x);
	if (CheckForError("Shader::SetConstant1F setting parameter", m_shaderPairName))
		return;
}

void GLCgShader::SetConstant(const str_type::string& name, const int n)
{
	CGparameter param = SeekParameter(name, m_cgVsProgam, m_cgPsProgam);
	if (!param)
    {
        ShowInvalidParameterWarning(m_shaderPairName, name);
          return;
    }

	cgSetParameter1i(param, n);
	if (CheckForError("Shader::SetConstant1I setting parameter", m_shaderPairName))
		return;
}

void GLCgShader::SetMatrixConstant(const str_type::string& name, const math::Matrix4x4 &matrix)
{
	CGparameter param = SeekParameter(name, m_cgVsProgam, m_cgPsProgam);
	if (!param)
    {
        ShowInvalidParameterWarning(m_shaderPairName, name);
          return;
    }

	cgSetMatrixParameterfr(param, (float*)&matrix);
	if (CheckForError("Shader::SetMatrixConstant setting parameter", m_shaderPairName))
		return;
}

void GLCgShader::SetConstantArray(const str_type::string& name, unsigned int nElements, const boost::shared_array<const math::Vector2>& v)
{
	CGparameter param = SeekParameter(name, m_cgVsProgam, m_cgPsProgam);
	if (!param)
    {
        ShowInvalidParameterWarning(m_shaderPairName, name);
          return;
    }

	cgGLSetParameterArray2f(param, 0, (long)nElements, (float*)v.get());
	if (CheckForError("Shader::SetConstantArrayF setting parameter", m_shaderPairName))
		return;
}

void GLCgShader::SetTexture(const str_type::string& name, TextureWeakPtr pTexture, const unsigned int index)
{
	CGparameter param = SeekParameter(name, m_cgVsProgam, m_cgPsProgam);

	if (!param)
    {
        ShowInvalidParameterWarning(m_shaderPairName, name);
          return;
    }

	DisableIfEnabled(param);

	const GLTexture* textureObj = (GLTexture*)(pTexture.lock().get());
	
	cgGLSetTextureParameter(param, textureObj->GetTextureInfo().m_texture);
	if (CheckForError("Shader::SetTexture", m_shaderPairName))
		return;

	cgGLEnableTextureParameter(param);
	if (CheckForError("Shader::SetTexture", m_shaderPairName))
		return;

	// add this param to the enabled texture parameter list so we can disable it with DisableTetureParams
	m_enabledTextures.push_back(param);
}

} // namespace gs2d
