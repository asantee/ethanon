/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#include "D3D9CgShader.h"

#include "../../Platform/Platform.h"
#include "../../Application.h"
#include "../../Texture.h"

#include <iostream>
#include <fstream>

namespace gs2d {
using namespace math;


// TODO/TO-DO: move it to Foundation
inline str_type::string getAsciiStringFromFileC(const str_type::char_t* file)
{
	str_type::stringstream ss;
	str_type::ifstream f(file);
	if (f.is_open())
	{
		while(!f.eof())
		{
			str_type::char_t ch = GS_L('\0');
			f.get(ch);
			if (ch != GS_L('\0'))
			{
				ss << ch;
			}
		}
		f.close();
		return ss.str();
	}
	return GS_L("");
}

D3D9CgShaderContext::D3D9CgShaderContext(IDirect3DDevice9 *pDevice)
{
	m_cgContext = 0;

	m_cgContext = cgCreateContext();
	if (CheckForError(GS_L("D3D9CgShaderContext::RegisterShaderHandler creating the context")))
	{
		m_cgContext = 0;
		return;
	}

	cgSetParameterSettingMode(m_cgContext, CG_DEFERRED_PARAMETER_SETTING);
	if (CheckForError(GS_L("D3D9CgShaderContext::RegisterShaderHandler setting parameter mode")))
	{
		m_cgContext = 0;
		return;
	}

	cgD3D9SetDevice(pDevice);
	if (CheckForError(GS_L("D3D9CgShaderContext::RegisterShaderHandler setting Direct3D device")))
	{
		m_cgContext = 0;
		return;
	}

	str_type::stringstream ss;
	ss << GS_L("the NVIDIA Cg language context has been successfuly created");
	ShowMessage(ss, GSMT_INFO);
}

D3D9CgShaderContext::~D3D9CgShaderContext()
{
	cgD3D9SetDevice(NULL);
	cgDestroyContext(m_cgContext);
}

bool D3D9CgShaderContext::CheckForError(const str_type::string& situation)
{
	CGerror error;
	const char* errorString = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		GS2D_CERR << GS_L("Situation: ") << situation << GS_L("\nError: ") << errorString << std::endl;
		return true;
	}
	return false;
}

boost::any D3D9CgShaderContext::GetContextPointer()
{
	return m_cgContext;
}

D3D9CgShader::D3D9CgShader()
{
	m_cgProgram = 0;
	m_focus = SF_NONE;
	m_profile = SP_NONE;
	m_shaderName = GS_L("");
}

inline CGparameter SeekParameter(const str_type::string &name, std::map<str_type::string, CGparameter> &m_mParam)
{
	std::map<str_type::string, CGparameter>::iterator iter = m_mParam.find(name);
	if (iter != m_mParam.end())
		return iter->second;	
	return 0;
}

D3D9CgShader::~D3D9CgShader()
{
	cgDestroyProgram(m_cgProgram);
}

Shader::SHADER_FOCUS D3D9CgShader::GetShaderFocus() const
{
	return m_focus;
}

Shader::SHADER_PROFILE D3D9CgShader::GetShaderProfile() const
{
	return m_profile;
}

bool D3D9CgShader::SetShader()
{
	if (!m_cgProgram)
	{
		ShowMessage(GS_L("D3D9CgShader::SetShader - The shader object or it's context are invalid."));
		return false;
	}
	cgD3D9BindProgram(m_cgProgram);
	if (CheckForError(GS_L("D3D9CgShader::SetShader"), m_shaderName))
		return false;
	cgUpdateProgramParameters(m_cgProgram);
	if (CheckForError(GS_L("D3D9CgShader::SetShader"), m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetConstant(const str_type::string& name, const Color& dw)
{
	Vector4 v;
	v.SetColor(dw);
	return SetConstant(name, v);
}

bool D3D9CgShader::SetConstant(const str_type::string& name, const float x, const float y, const float z, const float w)
{
	return SetConstant(name, Vector4(x,y,z,w));
}

bool D3D9CgShader::SetConstant(const str_type::string& name, const float x, const float y, const float z)
{
	return SetConstant(name, Vector3(x,y,z));
}

bool D3D9CgShader::SetConstant(const str_type::string& name, const float x, const float y)
{
	return SetConstant(name, Vector2(x,y));
}

bool D3D9CgShader::SetConstant(const str_type::string& name, const Vector4 &v)
{
	CGparameter param = SeekParameter(name, m_mParam);

	if (!param)
	{
		str_type::string message = GS_L("D3D9CgShader::Set(*) invalid parameter: ");
		message += name;
		ShowMessage(message);
		return false;
	}

	cgSetParameter4fv(param, (float*)&v);
	if (CheckForError(GS_L("D3D9CgShader::SetConstant4F setting parameter"), m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetConstant(const str_type::string& name, const Vector3 &v)
{
	CGparameter param = SeekParameter(name, m_mParam);

	if (!param)
	{
		str_type::string message = GS_L("D3D9CgShader::Set(*) invalid parameter: ");
		message += name;
		ShowMessage(message);
		return false;
	}

	cgSetParameter3fv(param, (float*)&v);
	if (CheckForError(GS_L("D3D9CgShader::SetConstant3F setting parameter"), m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetConstant(const str_type::string& name, const Vector2 &v)
{
	CGparameter param = SeekParameter(name, m_mParam);

	if (!param)
	{
		str_type::string message = GS_L("D3D9CgShader::Set(*) invalid parameter: ");
		message += name;
		ShowMessage(message);
		return false;
	}

	cgSetParameter2fv(param, (float*)&v);
	if (CheckForError(GS_L("D3D9CgShader::SetConstant2F setting parameter"), m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetConstant(const str_type::string& name, const float value)
{
	CGparameter param = SeekParameter(name, m_mParam);
	
	if (!param)
	{
		str_type::string message = GS_L("D3D9CgShader::Set(*) invalid parameter: ");
		message += name;
		ShowMessage(message);
		return false;
	}

	cgSetParameter1f(param, value);
	if (CheckForError(GS_L("D3D9CgShader::SetConstant1F setting parameter"), m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetConstant(const str_type::string& name, const int n)
{
	CGparameter param = SeekParameter(name, m_mParam);
	
	if (!param)
	{
		str_type::string message = GS_L("D3D9CgShader::Set(*) invalid parameter: ");
		message += name;
		ShowMessage(message);
		return false;
	}

	cgSetParameter1i(param, n);
	if (CheckForError(GS_L("D3D9CgShader::SetConstant1I setting parameter"), m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetMatrixConstant(const str_type::string& name, const Matrix4x4 &matrix)
{
	CGparameter param = SeekParameter(name, m_mParam);

	if (!param)
	{
		str_type::string message = GS_L("D3D9CgShader::Set(*) invalid parameter: ");
		message += name;
		ShowMessage(message);
		return false;
	}

	cgSetMatrixParameterfr(param, (float*)&matrix);
	if (CheckForError(GS_L("D3D9CgShader::SetMatrixConstantF setting parameter"), m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetConstantArray(const str_type::string& name, unsigned int nElements, const boost::shared_array<const math::Vector2>& v)
{
	CGparameter param = SeekParameter(name, m_mParam);

	if (!param)
	{
		str_type::string message = GS_L("D3D9CgShader::Set(*) invalid parameter: ");
		message += name;
		ShowMessage(message);
		return false;
	}

	//cgSetArraySize(param, nElements);
	cgD3D9SetUniformArray(param, 0, (DWORD)nElements, &(v.get()->x));
	if (CheckForError(GS_L("D3D9CgShader::SetConstantArrayF setting parameter"), m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::ConstantExist(const str_type::string& name)
{
	CGparameter param = SeekParameter(name, m_mParam);
	if (!param)
		return false;
	return true;
}

bool D3D9CgShader::SetTexture(const str_type::string& name, TextureWeakPtr pTexture)
{
	TexturePtr texture = pTexture.lock();
	if (m_focus != SF_PIXEL)
	{
		ShowMessage(GS_L("D3D9CgShader::SetTexture - textures can't be set as vertex shader parameter"));
		return false;
	}

	CGparameter param = SeekParameter(name, m_mParam);

	if (!param)
	{
		str_type::string message = GS_L("D3D9CgShader::Set(*) invalid parameter: ");
		message += name;
		ShowMessage(message);
		return false;
	}

	IDirect3DTexture9 *pD3DTexture;
	try
	{
		pD3DTexture = boost::any_cast<IDirect3DTexture9*>(texture->GetTextureObject());
	}
	catch (const boost::bad_any_cast &)
	{
		ShowMessage(GS_L("D3D9CgShader::SetTexture Invalid texture pointer"));
		return false;
	}
	cgD3D9SetTexture(param, pD3DTexture);
	if (CheckForError(GS_L("D3D9CgShader::SetTexture"), m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetupParameters()
{
	CGparameter param = cgGetFirstParameter(m_cgProgram, CG_GLOBAL);
	while (param)
	{
		m_mParam[cgGetParameterName(param)] = param;
		param = cgGetNextParameter(param);
	}
	param = cgGetFirstParameter(m_cgProgram, CG_PROGRAM);
	while (param)
	{
		m_mParam[cgGetParameterName(param)] = param;
		param = cgGetNextParameter(param);
	}

	return true;
}

bool D3D9CgShader::LoadShaderFromFile(ShaderContextPtr context, const str_type::string& fileName, const SHADER_FOCUS focus,
							  const SHADER_PROFILE profile, const char *entry)
{
	const std::string codeAsciiString = getAsciiStringFromFileC(fileName.c_str());
	return LoadShaderFromString(context, fileName, codeAsciiString, focus, profile, entry);
}

bool D3D9CgShader::LoadShaderFromString(ShaderContextPtr context, const str_type::string& shaderName, const std::string& codeAsciiString, const SHADER_FOCUS focus,
							  const SHADER_PROFILE profile, const char *entry)
{
	m_pShaderContext = context;
	m_shaderName = shaderName;

	m_focus = focus;
	m_profile = profile;

	CGprofile cgProfile;
	if (focus == SF_PIXEL)
	{
		m_cgLatestProfile = cgD3D9GetLatestPixelProfile();
		if (CheckForError(GS_L("D3D9CgShader::LoadShader getting latest pixel profile"), m_shaderName))
			return false;
		switch (profile)
		{
		case SP_MODEL_1:
			cgProfile = CG_PROFILE_PS_1_1;
			break;
		case SP_MODEL_2:
			cgProfile = CG_PROFILE_PS_2_0;
			break;
		case SP_MODEL_3:
			cgProfile = CG_PROFILE_PS_3_0;
			break;
		default:
			cgProfile = m_cgLatestProfile;
		};
	}
	else
	{
		m_cgLatestProfile = cgD3D9GetLatestVertexProfile();
		if (CheckForError(GS_L("D3D9CgShader::LoadShader getting latest vertex profile"), m_shaderName))
			return false;
		switch (profile)
		{
		case SP_MODEL_1:
			cgProfile = CG_PROFILE_VS_1_1;
			break;
		case SP_MODEL_2:
			cgProfile = CG_PROFILE_VS_2_0;
			break;
		case SP_MODEL_3:
			cgProfile = CG_PROFILE_VS_3_0;
			break;
		default:
			cgProfile = m_cgLatestProfile;
		};
	}

	if (cgProfile > m_cgLatestProfile)
	{
		cgProfile = m_cgLatestProfile;
		ShowMessage(GS_L("D3D9CgShader::LoadShader the current profile is not supported"));
	}

	const char **profileOpts;
	profileOpts = cgD3D9GetOptimalOptions(cgProfile);
	if (CheckForError(GS_L("D3D9CgShader::LoadShader getting optimal options"), m_shaderName))
		return false;

	CGcontext cgContext;
	try
	{
		cgContext = boost::any_cast<CGcontext>(m_pShaderContext->GetContextPointer());
	}
	catch (const boost::bad_any_cast &)
	{
		ShowMessage(GS_L("D3D9CgShader::LoadShader invalid CGcontext"));
		return false;
	}
	m_cgProgram = cgCreateProgram(
		cgContext, 
		CG_SOURCE,
		codeAsciiString.c_str(),
		cgProfile,
		entry,
		profileOpts
	);

	if (CheckForError(GS_L("D3D9CgShader::LoadShader creating program from file"), m_shaderName))
		return false;

	CompileShader();
	SetupParameters();
	return true;
}

bool D3D9CgShader::CompileShader()
{
	cgD3D9LoadProgram(m_cgProgram, false, 0);
	if (CheckForError(GS_L("D3D9CgShader::CompileShader loading the program"), m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::CheckForError(const str_type::string &situation, const str_type::string &add)
{
	CGerror error;
	const char* str = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR)
	{
		str_type::string text = GS_L("Situation: ");
		text += situation;
		text += GS_L("\nCg error string: ");
		text += str;
		text += GS_L("\nAdditional info: ");
		text += add;
		if (error == CG_COMPILER_ERROR)
		{
			text += GS_L("\nCg compiler error: ");

			CGcontext cgContext;
			try
			{
				cgContext = boost::any_cast<CGcontext>(m_pShaderContext->GetContextPointer());
			}
			catch (const boost::bad_any_cast &)
			{
				ShowMessage(GS_L("D3D9CgShader::CheckForError invalid CGcontext"));
				return false;
			}
			text += cgGetLastListing(cgContext);
		}
		if (error == CG_UNKNOWN_PROFILE_ERROR)
		{
			text += GS_L("\nThe current profile is not supported!");
		}
		ShowMessage(text);
		return true;
	}
	return false;
}

} // namespace gs2d
