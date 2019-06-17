#ifndef GS2D_GLCGSHADER_CONTEXT_H_
#define GS2D_GLCGSHADER_CONTEXT_H_

#include <Shader.h>

#include <Cg/cg.h>

namespace gs2d {

class GLCgShaderContext : public ShaderContext
{
	CGcontext m_cgContext;
	CGprofile m_latestFragmentProfile, m_latestVertexProfile;

	bool CheckForError(const std::string& situation);

public:
	GLCgShaderContext();
	~GLCgShaderContext();

	boost::any GetContextPointer();
	bool DisableTextureParams();
};

typedef boost::shared_ptr<GLCgShaderContext> GLCgShaderContextPtr;

} // namespace gs2d

#endif
