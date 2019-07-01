#ifndef GS2D_GLES2_SHADER_H_
#define GS2D_GLES2_SHADER_H_

#include "../../Shader.h"

#include "GLES2RectRenderer.h"

#include "GLES2Video.h"

#include "../../../../tsl/hopscotch_map.h"

namespace gs2d {

class GLES2ShaderContext : public ShaderContext
{
	GLES2RectRenderer m_rectRenderer;

	Platform::FileLogger m_logger;

	tsl::hopscotch_map<GLuint, int> m_vertexPosLocations;
	tsl::hopscotch_map<GLuint, int> m_texCoordLocations;

	static GLuint m_currentProgram;
	mutable GLuint m_lastProgram;
	mutable int m_lastPosLocation;
	mutable int m_lastTexLocation;

	void GetLocations(int& pos, int& tex) const;
	bool FindLocations(const GLuint program);

	bool CheckForError(const str_type::string& situation);

	static const int INVALID_ATTRIB_LOCATION;

public:
	GLES2ShaderContext(GLES2Video *pVideo);
	~GLES2ShaderContext();

	boost::any GetContextPointer();
	bool DisableTextureParams();

	void DrawRect(Video* video, const Sprite::RECT_MODE mode);

	void BeginFastDraw(Video* video);
	void FastDraw(Video* video);
	void EndFastDraw(Video* video);
};

typedef boost::shared_ptr<GLES2ShaderContext> GLES2ShaderContextPtr;

class GLES2Shader : public Shader
{
public:
	GLES2Shader(Platform::FileManagerPtr fileManager, GLES2ShaderContextPtr context);
	~GLES2Shader();

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

	bool ConstantExist(const str_type::string& name);
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
};

} // namespace gs2d

#endif
