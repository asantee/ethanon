#ifndef GS2D_GLES2_SHADER_H_
#define GS2D_GLES2_SHADER_H_

#include "../../Shader.h"

#include "../../Platform/FileManager.h"

#include <hopscotch_map.h>

#include "GLES2Include.h"

namespace gs2d {

class GLES2Shader : public Shader
{
	std::string m_vsShaderName;
	std::string m_psShaderName;

	std::string m_programName;

	GLuint m_program;
	
	tsl::hopscotch_map<std::string, GLint> m_parameters;

	Platform::FileManagerPtr m_fileManager;

	GLint m_vPositionLocation;
	GLint m_vTexCoordLocation;
	GLint m_vNormalLocation;

	GLint FindUniformLocation(const std::string& name);

public:
	GLES2Shader(Platform::FileManagerPtr fileManager);
	~GLES2Shader();
	
	GLuint GetProgram() const;
	
	GLint GetVPositionLocation() const;
	GLint GetVTexCoordLocation() const;
	GLint GetVNormalLocation() const;

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

	void SetConstant(const std::string& name, const math::Vector4 &v) override;
	void SetConstant(const std::string& name, const math::Vector3 &v) override;
	void SetConstant(const std::string& name, const math::Vector2 &v) override;
	void SetConstant(const std::string& name, const float x) override;
	void SetConstant(const std::string& name, const uint32_t n) override;
	void SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector2* v) override;
	void SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector4* v) override;
	void SetMatrixConstant(const std::string& name, const math::Matrix4x4 &matrix) override;
	void SetTexture(const std::string& name, TexturePtr pTexture, const unsigned int index) override;

	void SetShader() override;
};

typedef boost::shared_ptr<GLES2Shader> GLES2ShaderPtr;

} // namespace gs2d

#endif
