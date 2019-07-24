#ifndef GS2D_GLES2_SHADER_H_
#define GS2D_GLES2_SHADER_H_

#include "../../Shader.h"

#include "GLES2PolygonRenderer.h"
#include "GLES2UniformParameter.h"
#include "GLES2Video.h"

#include "../../../../tsl/hopscotch_map.h"

namespace gs2d {

class GLES2ShaderContext : public ShaderContext
{
public:
	GLES2ShaderContext(GLES2Video *pVideo);
	~GLES2ShaderContext();

	boost::any GetContextPointer();

	static bool CheckForError(const str_type::string& situation);
};

typedef boost::shared_ptr<GLES2ShaderContext> GLES2ShaderContextPtr;

class GLES2Shader : public Shader
{
	std::string m_vsShaderName;
	std::string m_psShaderName;

	std::string m_programName;

	GLuint m_program;
	
	tsl::hopscotch_map<std::string, GLES2UniformParameterPtr> m_parameters;
	
	Platform::FileManagerPtr m_fileManager;

public:
	GLES2Shader(Platform::FileManagerPtr fileManager, GLES2ShaderContextPtr context);
	~GLES2Shader();
	
	GLuint GetProgram() const;

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

	void SetConstant(const str_type::string& name, const math::Vector4 &v);
	void SetConstant(const str_type::string& name, const math::Vector3 &v);
	void SetConstant(const str_type::string& name, const math::Vector2 &v);
	void SetConstant(const str_type::string& name, const float x);
	void SetConstant(const str_type::string& name, const int n);
	void SetConstantArray(const str_type::string& name, unsigned int nElements, const boost::shared_array<const math::Vector2>& v);
	void SetMatrixConstant(const str_type::string& name, const math::Matrix4x4 &matrix);
	void SetTexture(const str_type::string& name, TextureWeakPtr pTexture, const unsigned int index);

	bool SetShader();
};

} // namespace gs2d

#endif
