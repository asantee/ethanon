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

#ifndef GS2D_GLES2_SHADER_H_
#define GS2D_GLES2_SHADER_H_

#include "../../Shader.h"
#include "../../Platform/Platform.h"
#include "../../Platform/FileLogger.h"

#include "GLES2RectRenderer.h"
#include "GLES2Texture.h"
#include "GLES2UniformParameter.h"

#include <map>

namespace gs2d {

class GLES2Shader;
typedef boost::shared_ptr<GLES2Shader> GLES2ShaderPtr;

inline std::size_t fastHash(const str_type::string &str)
{
	static const std::size_t seed = 0;
	std::size_t hash = seed;
	const char* s = str.c_str();
	while (*s)
	{
		hash = hash * 101  +  *s++;
	}
	return hash;
}

class GLES2ShaderContext : public ShaderContext
{
public:
	friend class GLES2UniformParameter;
	GLES2ShaderContext(GLES2Video *pVideo);
	~GLES2ShaderContext();

	boost::any GetContextPointer();
	bool DisableTextureParams();

	void SetShader(GLES2ShaderPtr vs, GLES2ShaderPtr ps, const math::Matrix4x4 &ortho, const math::Vector2& screenSize);
	void DrawRect(const Sprite::RECT_MODE mode);

	void BeginFastDraw();
	void FastDraw();
	void EndFastDraw();

	void Log(const str_type::string& str, const Platform::FileLogger::TYPE& type) const;

	void ResetViewConstants(const math::Matrix4x4 &ortho, const math::Vector2& screenSize);

	GLES2ShaderPtr GetCurrentVS();
	GLES2ShaderPtr GetCurrentPS();

	bool SetSpriteDepth(const float depth);
	float GetSpriteDepth() const;
	
	static const int INVALID_ATTRIB_LOCATION;

protected:
	GLES2ShaderPtr m_currentVS;
	GLES2ShaderPtr m_currentPS;
	std::map<std::size_t, GLuint> m_programs;
	std::map<GLuint, int> m_vertexPosLocations;
	std::map<GLuint, int> m_texCoordLocations;
	bool CheckForError(const str_type::string& situation);
	GLES2RectRenderer m_rectRenderer;

private:

	void CreateProgram();

	mutable GLuint m_lastProgram;
	mutable int m_lastPosLocation;
	mutable int m_lastTexLocation;
	void GetLocations(int& pos, int& tex) const;

	math::Matrix4x4 m_ortho;
	math::Vector2 m_screenSize;

	static GLuint m_currentProgram;
	Platform::FileLogger m_logger;

	float m_spriteDepthValue;
	
	void SetShader(GLES2ShaderPtr shader);
	void SetUniformParametersFromCurrentProgram(GLES2ShaderPtr shader) const;
	GLuint CreateProgramFromCurrentShaders();
	size_t ComputeProgramHashValue(GLES2ShaderPtr vertex, GLES2ShaderPtr pixel) const;
	bool FindLocations(const GLuint program);
};

typedef boost::shared_ptr<GLES2ShaderContext> GLES2ShaderContextPtr;

class GLES2Shader : public Shader
{
private:
	GLuint m_shader;
	boost::shared_ptr<Platform::FileLogger> m_logger;
	Platform::FileManagerPtr m_zip;

	GLES2ShaderContextPtr m_context;
	str_type::string m_shaderName;
	Shader::SHADER_FOCUS m_shaderFocus;
	std::map<std::size_t, GLES2UniformParameterPtr> m_parameters;
	const str_type::string DIFFUSE_TEXTURE_NAME;
	
	std::map<str_type::string, GLint> m_texturePasses;
	GLint m_texturePassCounter;

public:

	const std::map<std::size_t, GLES2UniformParameterPtr>& GetParameters() const;
	GLuint GetShader();
	str_type::string GetShaderName();
	
	GLES2Shader(Platform::FileManagerPtr fileManager, GLES2ShaderContextPtr context);
	~GLES2Shader();

	bool LoadShaderFromFile(
		ShaderContextPtr context,
		const str_type::string& fileName,
		const Shader::SHADER_FOCUS focus,
		const Shader::SHADER_PROFILE profile = Shader::SP_HIGHEST,
		const char *entry = 0);

	bool LoadShaderFromString(
		ShaderContextPtr context,
		const str_type::string& shaderName,
		const std::string& codeAsciiString,
		const Shader::SHADER_FOCUS focus,
		const Shader::SHADER_PROFILE profile = Shader::SP_HIGHEST,
		const char *entry = 0);

	bool ConstantExist(const str_type::string& name);

	bool SetConstant(const std::size_t nameHash, const str_type::string& name, const math::Vector4 &v);
	bool SetConstant(const std::size_t nameHash, const str_type::string& name, const math::Vector3 &v);
	bool SetConstant(const std::size_t nameHash, const str_type::string& name, const math::Vector2 &v);
	bool SetConstant(const std::size_t nameHash, const str_type::string& name, const float x);
	bool SetMatrixConstant(const std::size_t nameHash, const str_type::string& name, const math::Matrix4x4 &matrix);
	bool SetConstant(const std::size_t nameHash, const str_type::string& name, const Color& dw);
	bool SetConstantArray(const std::size_t nameHash, const str_type::string& name, unsigned int nElements,
						  const boost::shared_array<const math::Vector2>& v);

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
	Shader::SHADER_FOCUS GetShaderFocus() const;
	Shader::SHADER_PROFILE GetShaderProfile() const;
	void UnbindShader();
};

} // namespace gs2d

#endif