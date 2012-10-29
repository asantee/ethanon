/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/
 
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

CGprofile GLCgShaderContext::GetLatestVertexProfile() const
{
	return m_latestVertexProfile;
}

CGprofile GLCgShaderContext::GetLatestFragmentProfile() const
{
	return m_latestFragmentProfile;
}

Shader::SHADER_PROFILE GLCgShaderContext::CGProfileToGSProfile(const CGprofile prof)
{
	switch (prof)
	{
	case CG_PROFILE_ARBFP1:
	case CG_PROFILE_ARBVP1:
		return Shader::SP_MODEL_1;
	case CG_PROFILE_FP20:
	case CG_PROFILE_VP20:
		return Shader::SP_MODEL_2;
	case CG_PROFILE_FP30:
	case CG_PROFILE_VP30:
		return Shader::SP_MODEL_3;
	default:
		return Shader::SP_MODEL_3;
	}
	return Shader::SP_NONE;
}

} // namespace gs2d
