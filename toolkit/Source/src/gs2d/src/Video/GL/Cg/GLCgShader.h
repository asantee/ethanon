#ifndef GS2D_GLCGSHADER_H_
#define GS2D_GLCGSHADER_H_

#include "../../../Shader.h"
#include "GLCgShaderContext.h"

#include <Cg/cg.h>

#include <map>
#include <list>

#include "../../../Utilities/RecoverableResource.h"

namespace gs2d {

class GLVideo;

class GLCgShader : public Shader, RecoverableResource
{
	std::map<std::string, CGparameter> m_params;
	CGprogram m_cgProgam;
	CGprofile m_cgProfile;

	SHADER_FOCUS m_focus;
	
	std::string m_shaderName;
	std::string m_shaderCode;
	std::string m_entry;

	bool CheckForError(const std::string& situation, const std::string& additionalInfo);
	void Recover();

	GLVideo* m_video;
	GLCgShaderContextPtr m_shaderContext;
	CGcontext m_cgContext;

	CGcontext ExtractCgContext(ShaderContextPtr context);
	void FillParameters(const CGenum domain);

	std::list<CGparameter> m_enabledTextures;

	bool CreateCgProgram();
	void DestroyCgProgram();

	void DisableIfEnabled(CGparameter param);

public:
	GLCgShader(GLVideo* video);
	~GLCgShader();

	bool LoadShaderFromFile(
		ShaderContextPtr context,
		const str_type::string& fileName,
		const SHADER_FOCUS focus,
		const char *entry = 0);

	bool LoadShaderFromString(
		ShaderContextPtr context,
		const str_type::string& shaderName,
		const std::string& codeAsciiString,
		const SHADER_FOCUS focus,
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
	void UnbindShader();
	void DisableTextures();
};

typedef boost::shared_ptr<GLCgShader> GLCgShaderPtr;

}

#endif
