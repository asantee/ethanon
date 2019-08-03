#include "GLVideo.h"

#include "../cgShaderCode.h"

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

	// don't reset cg context if it is an opengl context reset
	if (!m_shaderContext)
		m_shaderContext = GLCgShaderContextPtr(new GLCgShaderContext);

	m_defaultShader  = LoadShaderFromString("defaultShaderVS", gs2dglobal::defaultVSCode,    "sprite",    "defaultShaderPS",  gs2dglobal::defaultFragmentShaders, "minimal");
	m_rectShader     = LoadShaderFromString("rectShaderVS",    gs2dglobal::defaultVSCode,    "rectangle", "rectShaderPS",     gs2dglobal::defaultFragmentShaders, "minimal");
	m_fastShader     = LoadShaderFromString("fastShaderVS",    gs2dglobal::fastSimpleVSCode, "fast",      "fastShaderPS",     gs2dglobal::defaultFragmentShaders, "minimal");
	m_modulateShader = LoadShaderFromString("fastShaderVS",    gs2dglobal::defaultVSCode,    "sprite",    "modulateShaderPS", gs2dglobal::defaultFragmentShaders, "modulate");
	m_addShader      = LoadShaderFromString("addShaderVS",     gs2dglobal::defaultVSCode,    "sprite",    "addShaderPS",      gs2dglobal::defaultFragmentShaders, "add");

	m_currentShader = m_defaultShader;

	UpdateInternalShadersViewData(GetScreenSizeF(), false);

	m_rectRenderer = boost::shared_ptr<GLRectRenderer>(new GLRectRenderer());
	return true;
}

void GLVideo::UpdateInternalShadersViewData(const math::Vector2& screenSize, const bool invertY)
{
	UpdateShaderViewData(m_defaultShader, screenSize);
	UpdateShaderViewData(m_rectShader, screenSize);
	UpdateShaderViewData(m_fastShader, screenSize);
}

void GLVideo::UpdateShaderViewData(const ShaderPtr& shader, const math::Vector2& screenSize)
{
	shader->SetConstant("screenSize", screenSize);
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

const GLRectRenderer& GLVideo::GetRectRenderer() const
{
	return *m_rectRenderer.get();
}

ShaderPtr GLVideo::GetDefaultShader()
{
	return m_defaultShader;
}

ShaderPtr GLVideo::GetRectShader()
{
	return m_rectShader;
}

ShaderPtr GLVideo::GetFastShader()
{
	return m_fastShader;
}

ShaderPtr GLVideo::GetModulateShader()
{
	return m_modulateShader;
}

ShaderPtr GLVideo::GetAddShader()
{
	return m_addShader;
}

ShaderPtr GLVideo::GetCurrentShader()
{
	return m_currentShader;
}

ShaderContextPtr GLVideo::GetShaderContext()
{
	return m_shaderContext;
}

bool GLVideo::SetCurrentShader(ShaderPtr shader)
{
	if (!shader)
	{
		m_currentShader = m_defaultShader;
	}
	else
	{
		m_currentShader = shader;
	}

	const math::Vector2 screenSize = GetScreenSizeF();
	UpdateShaderViewData(m_currentShader, screenSize);
	return true;
}

boost::any GLVideo::GetGraphicContext()
{
	return m_shaderContext;
}

boost::any GLVideo::GetVideoInfo()
{
	// no GL context to return
	return 0;
}

} // namespace gs2d
