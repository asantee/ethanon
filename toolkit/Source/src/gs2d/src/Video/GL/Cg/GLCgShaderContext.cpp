#include "GLCgShaderContext.h"
#include "../../../Application.h"

#include <Cg/cgGL.h>

namespace gs2d {

GLCgShaderContext::GLCgShaderContext() :
	m_cgContext(0)
{
	m_cgContext = cgCreateContext();
	CheckForError("GLCgShaderContext creating the context");

	cgSetParameterSettingMode(m_cgContext, CG_DEFERRED_PARAMETER_SETTING);
	CheckForError("GLCgShaderContext setting parameter mode");

	#ifdef DEBUG
		cgGLSetDebugMode(CG_TRUE);
	#else
		cgGLSetDebugMode(CG_FALSE);
	#endif

	m_latestFragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
	m_latestVertexProfile   = cgGLGetLatestProfile(CG_GL_VERTEX);
}

GLCgShaderContext::~GLCgShaderContext()
{
	cgDestroyContext(m_cgContext);
	m_cgContext = 0;
}

bool GLCgShaderContext::CheckForError(const std::string& situation)
{
	CGerror error;
	const char *string = cgGetLastErrorString(&error);

	if (error != CG_NO_ERROR)
	{
		std::stringstream ss;
		ss << "Situation: " << situation << "\nError: " << string;
		ShowMessage(ss, GSMT_ERROR);
		return true;
	}
	return false;
}

boost::any GLCgShaderContext::GetContextPointer()
{
	return m_cgContext;
}

bool GLCgShaderContext::DisableTextureParams()
{
	return false;
}

} // namespace gs2d
