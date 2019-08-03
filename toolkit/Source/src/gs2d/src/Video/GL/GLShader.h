#ifndef GS2D_GLSHADER_H_
#define GS2D_GLSHADER_H_

#include "../../Shader.h"
#include "../../Video.h"

#include "../../Platform/FileManager.h"

#include "../../Math/Vector2.h"

#include "GLInclude.h"

namespace gs2d {

class GLShaderContext : ShaderContext
{
public:
	boost::any GetContextPointer();
};

class GLShader : public Shader
{
	GLuint m_program;

	tsl::hopscotch_map<std::string, GLint> m_parameters;

	GLint FindUniformLocation(const std::string& name);
	
	Platform::FileManagerPtr m_fileManager;

public:
	GLShader(Platform::FileManagerPtr fileManager);
	~GLShader();

	bool LoadShaderFromFile(
		ShaderContextPtr context,
		const std::string& vsFileName,
		const std::string& vsEntry,
		const std::string& psFileName,
		const std::string& psEntry);

	bool LoadShaderFromString(
		ShaderContextPtr context,
		const std::string& vsShaderName,
		const std::string& vsCodeAsciiString,
		const std::string& vsEntry,
		const std::string& psShaderName,
		const std::string& psCodeAsciiString,
		const std::string& psEntry);

	void SetShader();

	void SetConstant(const str_type::string& name, const int n);
	void SetConstant(const std::string& name, const float v);
	void SetConstant(const std::string& name, const math::Vector2& v);
	void SetConstant(const std::string& name, const math::Vector3& v);
	void SetConstant(const std::string& name, const math::Vector4& v);
	void SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector4* v);
	void SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector2* v);
	void SetMatrixConstant(const std::string& name, const math::Matrix4x4& matrix);
	void SetTexture(const str_type::string& name, TexturePtr pTexture, const unsigned int index);
};

} // namespace sprite

#endif
