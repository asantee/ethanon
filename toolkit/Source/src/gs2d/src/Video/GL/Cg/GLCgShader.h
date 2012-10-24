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

#ifndef GS2D_GLCGSHADER_H_
#define GS2D_GLCGSHADER_H_

#include "../../../Shader.h"
#include "GLCGShaderContext.h"

#include <Cg/cg.h>

#include <map>

namespace gs2d {

class GLCgShader : public Shader
{
	std::map<std::string, CGparameter> m_params;
	CGprogram m_cgProgam;
	CGprofile m_cgProfile;

	SHADER_FOCUS m_focus;
	SHADER_PROFILE m_profile;
	
	std::string m_shaderName;

	bool CheckForError(const std::string& situation, const std::string& additionalInfo);

	GLCgShaderContextPtr m_shaderContext;
	CGcontext m_cgContext;

	CGcontext ExtractCgContext(ShaderContextPtr context);
	void FillParameters(const CGenum domain);

public:
	GLCgShader();
	~GLCgShader();

	bool LoadShaderFromFile(
		ShaderContextPtr context,
		const str_type::string& fileName,
		const SHADER_FOCUS focus,
		const SHADER_PROFILE profile = SP_HIGHEST,
		const char *entry = 0);

	bool LoadShaderFromString(
		ShaderContextPtr context,
		const str_type::string& shaderName,
		const std::string& codeAsciiString,
		const SHADER_FOCUS focus,
		const SHADER_PROFILE profile = SP_HIGHEST,
		const char *entry = 0);

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

	bool SetShader();
	SHADER_FOCUS GetShaderFocus() const;
	SHADER_PROFILE GetShaderProfile() const;
	void UnbindShader();
};

}

#endif
