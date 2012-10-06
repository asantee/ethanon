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

#include "gs2dD3D9CgShader.h"
#include "../../Platform/Platform.h"
#include "../../Application.h"
#include "../../Texture.h"

#include <iostream>
#include <fstream>

namespace gs2d {
using namespace math;

const std::wstring cgGetLastErrorStringW(CGerror* error)
{
	return Platform::ConvertAsciiToUnicode( cgGetLastErrorString(error) );
}

const std::wstring cgGetLastListingW(CGcontext context)
{
	return Platform::ConvertAsciiToUnicode( cgGetLastListing(context) );
}

const std::wstring cgGetParameterNameW(CGparameter param)
{
	return Platform::ConvertAsciiToUnicode(cgGetParameterName(param));
}

CGprogram cgCreateProgramW(
	CGcontext ctx,
    CGenum program_type,
	const std::string& program,
    CGprofile profile,
    const char *entry,
    const char **args
)
{
	return cgCreateProgram(
		ctx, 
		program_type,
		program.c_str(),
		profile,
		entry,
		args
	);
}

CGprogram cgCreateProgramFromFileW(
	CGcontext ctx,
    CGenum program_type,
    const std::wstring& program_file,
    CGprofile profile,
    const char *entry,
    const char **args
)
{
	return cgCreateProgramFromFile(
		ctx, 
		program_type,
		Platform::ConvertUnicodeToAscii(program_file.c_str()).c_str(),
		profile,
		entry,
		args
	);
}


D3D9CgShaderContext::D3D9CgShaderContext(IDirect3DDevice9 *pDevice)
{
	m_cgContext = 0;

	m_cgContext = cgCreateContext();
	if (CheckForError(L"D3D9CgShaderContext::RegisterShaderHandler creating the context"))
	{
		m_cgContext = 0;
		return;
	}

	cgSetParameterSettingMode(m_cgContext, CG_DEFERRED_PARAMETER_SETTING);
	if (CheckForError(L"D3D9CgShaderContext::RegisterShaderHandler setting parameter mode"))
	{
		m_cgContext = 0;
		return;
	}

	cgD3D9SetDevice(pDevice);
	if (CheckForError(L"D3D9CgShaderContext::RegisterShaderHandler setting Direct3D device"))
	{
		m_cgContext = 0;
		return;
	}

	std::wstringstream ss;
	ss << L"the NVIDIA Cg language context has been successfuly created";
	ShowMessage(ss, GSMT_INFO);
}

D3D9CgShaderContext::~D3D9CgShaderContext()
{
	cgD3D9SetDevice(NULL);
	cgDestroyContext(m_cgContext);
}

bool D3D9CgShaderContext::CheckForError(const std::wstring& situation)
{
	CGerror error;
	const std::wstring errorString = cgGetLastErrorStringW(&error);
	if (error != CG_NO_ERROR)
	{
		std::wcerr << L"Situation: " << situation << L"\nError: " << errorString << std::endl;
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
	m_focus = GSSF_NONE;
	m_profile = GSSP_NONE;
	m_shaderName = L"";
}

inline CGparameter SeekParameter(const std::wstring &name, std::map<std::wstring, CGparameter> &m_mParam)
{
	std::map<std::wstring, CGparameter>::iterator iter = m_mParam.find(name);
	if (iter != m_mParam.end())
		return iter->second;	
	return 0;
}

D3D9CgShader::~D3D9CgShader()
{
	cgDestroyProgram(m_cgProgram);
}

GS_SHADER_FOCUS D3D9CgShader::GetShaderFocus() const
{
	return m_focus;
}

GS_SHADER_PROFILE D3D9CgShader::GetShaderProfile() const
{
	return m_profile;
}

bool D3D9CgShader::SetShader()
{
	if (!m_cgProgram)
	{
		ShowMessage(L"D3D9CgShader::SetShader - The shader object or it's context are invalid.");
		return false;
	}
	cgD3D9BindProgram(m_cgProgram);
	if (CheckForError(L"D3D9CgShader::SetShader", m_shaderName))
		return false;
	cgUpdateProgramParameters(m_cgProgram);
	if (CheckForError(L"D3D9CgShader::SetShader", m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetConstant(const std::wstring& name, const Color& dw)
{
	Vector4 v;
	v.SetColor(dw);
	return SetConstant(name, v);
}

bool D3D9CgShader::SetConstant(const std::wstring& name, const float x, const float y, const float z, const float w)
{
	return SetConstant(name, Vector4(x,y,z,w));
}

bool D3D9CgShader::SetConstant(const std::wstring& name, const float x, const float y, const float z)
{
	return SetConstant(name, Vector3(x,y,z));
}

bool D3D9CgShader::SetConstant(const std::wstring& name, const float x, const float y)
{
	return SetConstant(name, Vector2(x,y));
}

bool D3D9CgShader::SetConstant(const std::wstring& name, const Vector4 &v)
{
	CGparameter param = SeekParameter(name, m_mParam);

	if (!param)
	{
		std::wstring message = L"D3D9CgShader::Set(*) invalid parameter: ";
		message += name;
		ShowMessage(message);
		return false;
	}

	cgSetParameter4fv(param, (float*)&v);
	if (CheckForError(L"D3D9CgShader::SetConstant4F setting parameter", m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetConstant(const std::wstring& name, const Vector3 &v)
{
	CGparameter param = SeekParameter(name, m_mParam);

	if (!param)
	{
		std::wstring message = L"D3D9CgShader::Set(*) invalid parameter: ";
		message += name;
		ShowMessage(message);
		return false;
	}

	cgSetParameter3fv(param, (float*)&v);
	if (CheckForError(L"D3D9CgShader::SetConstant3F setting parameter", m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetConstant(const std::wstring& name, const Vector2 &v)
{
	CGparameter param = SeekParameter(name, m_mParam);

	if (!param)
	{
		std::wstring message = L"D3D9CgShader::Set(*) invalid parameter: ";
		message += name;
		ShowMessage(message);
		return false;
	}

	cgSetParameter2fv(param, (float*)&v);
	if (CheckForError(L"D3D9CgShader::SetConstant2F setting parameter", m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetConstant(const std::wstring& name, const float value)
{
	CGparameter param = SeekParameter(name, m_mParam);
	
	if (!param)
	{
		std::wstring message = L"D3D9CgShader::Set(*) invalid parameter: ";
		message += name;
		ShowMessage(message);
		return false;
	}

	cgSetParameter1f(param, value);
	if (CheckForError(L"D3D9CgShader::SetConstant1F setting parameter", m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetConstant(const std::wstring& name, const int n)
{
	CGparameter param = SeekParameter(name, m_mParam);
	
	if (!param)
	{
		std::wstring message = L"D3D9CgShader::Set(*) invalid parameter: ";
		message += name;
		ShowMessage(message);
		return false;
	}

	cgSetParameter1i(param, n);
	if (CheckForError(L"D3D9CgShader::SetConstant1I setting parameter", m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetMatrixConstant(const std::wstring& name, const Matrix4x4 &matrix)
{
	CGparameter param = SeekParameter(name, m_mParam);

	if (!param)
	{
		std::wstring message = L"D3D9CgShader::Set(*) invalid parameter: ";
		message += name;
		ShowMessage(message);
		return false;
	}

	cgSetMatrixParameterfr(param, (float*)&matrix);
	if (CheckForError(L"D3D9CgShader::SetMatrixConstantF setting parameter", m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetConstantArray(const std::wstring& name, unsigned int nElements, const boost::shared_array<const math::Vector2>& v)
{
	CGparameter param = SeekParameter(name, m_mParam);

	if (!param)
	{
		std::wstring message = L"D3D9CgShader::Set(*) invalid parameter: ";
		message += name;
		ShowMessage(message);
		return false;
	}

	//cgSetArraySize(param, nElements);
	cgD3D9SetUniformArray(param, 0, (DWORD)nElements, &(v.get()->x));
	if (CheckForError(L"D3D9CgShader::SetConstantArrayF setting parameter", m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::ConstantExist(const std::wstring& name)
{
	CGparameter param = SeekParameter(name, m_mParam);
	if (!param)
		return false;
	return true;
}

bool D3D9CgShader::SetTexture(const std::wstring& name, TextureWeakPtr pTexture)
{
	TexturePtr texture = pTexture.lock();
	if (m_focus != GSSF_PIXEL)
	{
		ShowMessage(L"D3D9CgShader::SetTexture - textures can't be set as vertex shader parameter");
		return false;
	}

	CGparameter param = SeekParameter(name, m_mParam);

	if (!param)
	{
		std::wstring message = L"D3D9CgShader::Set(*) invalid parameter: ";
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
		ShowMessage(L"D3D9CgShader::SetTexture Invalid texture pointer");
		return false;
	}
	cgD3D9SetTexture(param, pD3DTexture);
	if (CheckForError(L"D3D9CgShader::SetTexture", m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::SetupParameters()
{
	CGparameter param = cgGetFirstParameter(m_cgProgram, CG_GLOBAL);
	while (param)
	{
		m_mParam[cgGetParameterNameW(param)] = param;
		param = cgGetNextParameter(param);
	}
	param = cgGetFirstParameter(m_cgProgram, CG_PROGRAM);
	while (param)
	{
		m_mParam[cgGetParameterNameW(param)] = param;
		param = cgGetNextParameter(param);
	}

	return true;
}

// TODO/TO-DO: move it to Foundation
inline std::string getAsciiStringFromFileC(const wchar_t *file)
{
	std::wstringstream ss;
	std::wifstream f(file);
	if (f.is_open())
	{
		while(!f.eof())
		{
			wchar_t ch = L'\0';
			f.get(ch);
			if (ch != L'\0')
			{
				ss << ch;
			}
		}
		f.close();
		return Platform::ConvertUnicodeToAscii(ss.str().c_str());
	}
	return "";
}

// TODO/TO-DO: move it to Foundation
inline std::wstring getStringFromFileC(const wchar_t *file)
{
	std::wstringstream ss;
	std::wifstream f(file);
	if (f.is_open())
	{
		while(!f.eof())
		{
			wchar_t ch = L'\0';
			f.get(ch);
			if (ch != L'\0')
			{
				ss << ch;
			}
		}
		f.close();
		return ss.str();
	}
	return L"";
}

bool D3D9CgShader::LoadShaderFromFile(ShaderContextPtr context, const std::wstring& fileName, const GS_SHADER_FOCUS focus,
							  const GS_SHADER_PROFILE profile, const char *entry)
{
	const std::string codeAsciiString = getAsciiStringFromFileC(fileName.c_str());
	return LoadShaderFromString(context, fileName, codeAsciiString, focus, profile, entry);
}

bool D3D9CgShader::LoadShaderFromString(ShaderContextPtr context, const std::wstring& shaderName, const std::string& codeAsciiString, const GS_SHADER_FOCUS focus,
							  const GS_SHADER_PROFILE profile, const char *entry)
{
	m_pShaderContext = context;
	m_shaderName = shaderName;

	m_focus = focus;
	m_profile = profile;

	CGprofile cgProfile;
	if (focus == GSSF_PIXEL)
	{
		m_cgLatestProfile = cgD3D9GetLatestPixelProfile();
		if (CheckForError(L"D3D9CgShader::LoadShader getting latest pixel profile", m_shaderName))
			return false;
		switch (profile)
		{
		case GSSP_MODEL_1:
			cgProfile = CG_PROFILE_PS_1_1;
			break;
		case GSSP_MODEL_2:
			cgProfile = CG_PROFILE_PS_2_0;
			break;
		case GSSP_MODEL_3:
			cgProfile = CG_PROFILE_PS_3_0;
			break;
		default:
			cgProfile = m_cgLatestProfile;
		};
	}
	else
	{
		m_cgLatestProfile = cgD3D9GetLatestVertexProfile();
		if (CheckForError(L"D3D9CgShader::LoadShader getting latest vertex profile", m_shaderName))
			return false;
		switch (profile)
		{
		case GSSP_MODEL_1:
			cgProfile = CG_PROFILE_VS_1_1;
			break;
		case GSSP_MODEL_2:
			cgProfile = CG_PROFILE_VS_2_0;
			break;
		case GSSP_MODEL_3:
			cgProfile = CG_PROFILE_VS_3_0;
			break;
		default:
			cgProfile = m_cgLatestProfile;
		};
	}

	if (cgProfile > m_cgLatestProfile)
	{
		cgProfile = m_cgLatestProfile;
		ShowMessage(L"D3D9CgShader::LoadShader the current profile is not supported");
	}

	const char **profileOpts;
	profileOpts = cgD3D9GetOptimalOptions(cgProfile);
	if (CheckForError(L"D3D9CgShader::LoadShader getting optimal options", m_shaderName))
		return false;

	CGcontext cgContext;
	try
	{
		cgContext = boost::any_cast<CGcontext>(m_pShaderContext->GetContextPointer());
	}
	catch (const boost::bad_any_cast &)
	{
		ShowMessage(L"D3D9CgShader::LoadShader invalid CGcontext");
		return false;
	}
	m_cgProgram = cgCreateProgramW(
		cgContext, 
		CG_SOURCE,
		codeAsciiString,
		cgProfile,
		entry,
		profileOpts
	);

	if (CheckForError(L"D3D9CgShader::LoadShader creating program from file", m_shaderName))
		return false;

	CompileShader();
	SetupParameters();
	return true;
}

bool D3D9CgShader::CompileShader()
{
	cgD3D9LoadProgram(m_cgProgram, false, 0);
	if (CheckForError(L"D3D9CgShader::CompileShader loading the program", m_shaderName))
		return false;
	return true;
}

bool D3D9CgShader::CheckForError(const std::wstring &situation, const std::wstring &add)
{
	CGerror error;
	const std::wstring str = cgGetLastErrorStringW(&error);
	if (error != CG_NO_ERROR)
	{
		std::wstring text = L"Situation: ";
		text += situation;
		text += L"\nCg error string: ";
		text += str;
		text += L"\nAdditional info: ";
		text += add;
		if (error == CG_COMPILER_ERROR)
		{
			text += L"\nCg compiler error: ";

			CGcontext cgContext;
			try
			{
				cgContext = boost::any_cast<CGcontext>(m_pShaderContext->GetContextPointer());
			}
			catch (const boost::bad_any_cast &)
			{
				ShowMessage(L"D3D9CgShader::CheckForError invalid CGcontext");
				return false;
			}
			text += cgGetLastListingW(cgContext);
		}
		if (error == CG_UNKNOWN_PROFILE_ERROR)
		{
			text += L"\nThe current profile is not supported!";
		}
		ShowMessage(text);
		return true;
	}
	return false;
}

} // namespace gs2d