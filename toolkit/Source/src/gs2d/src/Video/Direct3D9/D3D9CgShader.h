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

#ifndef GS2D_CGD3D9_SHADER_H_
#define GS2D_CGD3D9_SHADER_H_

#include "../../Shader.h"

//#define WIN32_LEAN_AND_MEAN
//#ifdef _DEBUG
//#define D3D_DEBUG_INFO
//#endif

#include <Cg/cg.h>
#include <Cg/cgD3D9.h>
#include <map>

namespace gs2d {

/**
 * \brief Implements an NVIDIA Cg based Direct3D 9 context
 */
class D3D9CgShaderContext : public ShaderContext
{
	friend class D3D9Video;
	CGcontext m_cgContext;
	D3D9CgShaderContext(IDirect3DDevice9 *pDevice);

public:
	~D3D9CgShaderContext();

	boost::any GetContextPointer();

	/// Dummy function (isn't necessary with D3D9)
	bool DisableTextureParams() { return true; }

protected:
	bool CheckForError(const str_type::string& situation);
};

typedef boost::shared_ptr<D3D9CgShaderContext> D3D9CgShaderContextPtr;

/**
 * \brief Implemtns an NVIDIA Cg based Direct3D 9 shader object
 */
class D3D9CgShader : public Shader
{
	CGprogram m_cgProgram;
	CGprofile m_cgLatestProfile;
	std::map<str_type::string, CGparameter> m_mParam;

	bool SetupParameters();
	SHADER_FOCUS m_focus;
	SHADER_PROFILE m_profile;
	str_type::string m_shaderName;
	ShaderContextPtr m_pShaderContext;

	bool CheckForError(const str_type::string &situation, const str_type::string &add);

public:
	D3D9CgShader();
	~D3D9CgShader();

	bool ConstantExist(const str_type::string& name);
	bool SetConstant(const str_type::string& name, const Color& dw);
	bool SetConstant(const str_type::string& name, const math::Vector4 &v);
	bool SetConstant(const str_type::string& name, const math::Vector3 &v);
	bool SetConstant(const str_type::string& name, const math::Vector2 &v);
	bool SetConstant(const str_type::string& name, const float x, const float y, const float z, const float w);
	bool SetConstant(const str_type::string& name, const float x, const float y, const float z);
	bool SetConstant(const str_type::string& name, const float x, const float y);
	bool SetConstant(const str_type::string& name, const float x);
	bool SetConstant(const str_type::string& name, const int n);
	bool SetConstantArray(const str_type::string& name, unsigned int nElements, const boost::shared_array<const math::Vector2>& v);
	bool SetMatrixConstant(const str_type::string& name, const math::Matrix4x4 &matrix);
	bool SetTexture(const str_type::string& name, TextureWeakPtr pTexture);

	bool CompileShader();
	bool SetShader();
	SHADER_FOCUS GetShaderFocus() const;
	SHADER_PROFILE GetShaderProfile() const;

	/// dummy since it's unecessary with D3D9
	void UnbindShader() {}

protected:

	bool LoadShaderFromFile(ShaderContextPtr context, const str_type::string& fileName, const SHADER_FOCUS focus,
		const SHADER_PROFILE profile = SP_HIGHEST, const char *entry = 0);

	bool LoadShaderFromString(ShaderContextPtr context, const str_type::string& shaderName, const std::string& codeAsciiString, const SHADER_FOCUS focus,
		const SHADER_PROFILE profile = SP_HIGHEST, const char *entry = 0);
};

typedef boost::shared_ptr<D3D9CgShader> D3D9CgShaderPtr;

} // namespace gs2d

#endif 