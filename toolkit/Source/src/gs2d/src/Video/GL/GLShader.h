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
		const std::string& psEntry) override;

	bool LoadShaderFromString(
		ShaderContextPtr context,
		const std::string& vsShaderName,
		const std::string& vsCodeAsciiString,
		const std::string& vsEntry,
		const std::string& psShaderName,
		const std::string& psCodeAsciiString,
		const std::string& psEntry) override;

	void SetShader() override;

	void SetConstant(const str_type::string& name, const int n) override;
	void SetConstant(const std::string& name, const float v) override;
	void SetConstant(const std::string& name, const math::Vector2& v) override;
	void SetConstant(const std::string& name, const math::Vector3& v) override;
	void SetConstant(const std::string& name, const math::Vector4& v) override;
	void SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector4* v) override;
	void SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector2* v) override;
	void SetMatrixConstant(const std::string& name, const math::Matrix4x4& matrix) override;
	void SetTexture(const str_type::string& name, TexturePtr pTexture, const unsigned int index) override;
};

} // namespace sprite

#endif
