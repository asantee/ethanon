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
	CGprogram m_cgVsProgam;
    CGprogram m_cgPsProgam;

	CGprofile m_cgVsProfile;
    CGprofile m_cgPsProfile;

	std::string m_vsShaderName;
	std::string m_vsShaderCode;

	std::string m_psShaderName;
	std::string m_psShaderCode;

    std::string m_shaderPairName;

	std::string m_vsEntry;
    std::string m_psEntry;

	bool CheckForError(const std::string& situation, const std::string& additionalInfo);
	void Recover();

	GLVideo* m_video;
	GLCgShaderContextPtr m_shaderContext;
	CGcontext m_cgContext;

	CGcontext ExtractCgContext(ShaderContextPtr context);

	std::list<CGparameter> m_enabledTextures;

	bool CreateCgProgram(
		CGprogram* outProgram,
        const std::string& shaderCode,
        CGprofile profile,
        const std::string& shaderName,
        const std::string& entry);

	void DestroyCgProgram(CGprogram* outProgram);

	void DisableIfEnabled(CGparameter param);

public:
	GLCgShader(GLVideo* video);
	~GLCgShader();

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
	void UnbindShader();
	void DisableTextures();
};

typedef boost::shared_ptr<GLCgShader> GLCgShaderPtr;

}

#endif
