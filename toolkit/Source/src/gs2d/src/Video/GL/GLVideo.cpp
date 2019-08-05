#include "GLVideo.h"

#include "GLTexture.h"

#include <SOIL.h>

#include "../../Platform/Platform.h"

namespace gs2d {

GLVideo::GLVideo() :
	m_textureExtension(GL_TEXTURE_2D),
	m_alphaMode(AM_UNKNOWN),
	m_alphaRef(0.004),
	m_zFar(1.0f),
	m_zNear(0.0f),
	m_backgroundColor(gs2d::constant::BLACK)
{
}

bool GLVideo::StartApplication(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Texture::PIXEL_FORMAT pfBB,
	const bool maximizable)
{
	SetAlphaMode(Video::AM_PIXEL);

	SetZBuffer(false);

	for (unsigned int t = 0; t < 4; t++)
	{
		glActiveTexture(GL_TEXTURE0 + t);
		glTexParameteri(GL_TEXTURE0 + t, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE0 + t, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	Enable2DStates();
	return true;
}

void GLVideo::Enable2DStates()
{
	const math::Vector2 screenSize(GetScreenSizeInPixels());
	glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
	glDisable(GL_CULL_FACE);
	glDisable(GL_DITHER);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
	glDepthRange(0.0f, 1.0f);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
}

bool GLVideo::SetAlphaMode(const ALPHA_MODE mode)
{
	m_alphaMode = mode;
	switch(mode)
	{
	case AM_ADD:
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		break;
	case AM_MODULATE:
		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		break;
	case AM_NONE:
		glDisable(GL_BLEND);
		break;
	case AM_PIXEL:
	default:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	};
	return true;
}

Video::ALPHA_MODE GLVideo::GetAlphaMode() const
{
	return m_alphaMode;
}

void GLVideo::SetZBuffer(const bool enable)
{
	if (enable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

bool GLVideo::GetZBuffer() const
{
	GLboolean enabled;
	glGetBooleanv(GL_DEPTH_TEST, &enabled);
	return (enabled == GL_TRUE) ? true : false;
}

bool GLVideo::BeginRendering(const Color& bgColor)
{
	const Color color(bgColor != math::constant::ZERO_VECTOR4 ? bgColor : m_backgroundColor);
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetAlphaMode(Video::AM_PIXEL);
	
	m_rendering = true;
	return true;
}

bool GLVideo::EndRendering()
{
	m_rendering = false;
	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		ResetVideoMode(static_cast<unsigned int>(GetScreenSize().x), static_cast<unsigned int>(GetScreenSize().y), Texture::PF_DEFAULT);
	}
	return true;
}

void GLVideo::SetBGColor(const Color& backgroundColor)
{
	m_backgroundColor = backgroundColor;
}

Color GLVideo::GetBGColor() const
{
	return m_backgroundColor;
}

ShaderContextPtr GLVideo::GetShaderContext()
{
	return ShaderContextPtr();
}

boost::any GLVideo::GetGraphicContext()
{
	return 0;
}

boost::any GLVideo::GetVideoInfo()
{
	// no GL context to return
	return 0;
}

} // namespace gs2d
