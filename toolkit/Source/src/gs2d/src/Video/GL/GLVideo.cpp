/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/
 
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

#include "GLVideo.h"

#include "../cgShaderCode.h"

#include "GLTexture.h"

#include <SOIL.h>

#include "../../Platform/Platform.h"

namespace gs2d {

void GLVideo::UnbindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLVideo::GLVideo() :
	m_filter(Video::TM_UNKNOWN),
	m_textureExtension(GL_TEXTURE_2D),
	m_alphaMode(AM_UNKNOWN),
	m_alphaRef(0.004),
	m_zFar(1.0f),
	m_zNear(0.0f),
	m_backgroundColor(gs2d::constant::BLACK),
	m_rendering(false),
	m_clamp(true),
	m_blendMode(BM_MODULATE),
	m_scissor(math::Vector2i(0, 0), math::Vector2i(0, 0))
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
	SetFilterMode(Video::TM_ALWAYS);
	SetAlphaMode(Video::AM_PIXEL);

	SetZBuffer(false);
	SetZWrite(false);
	SetClamp(true);
	SetBlendMode(1, Video::BM_MODULATE);

	Enable2DStates();

	// don't reset cg context if it is an opengl context reset
	if (!m_shaderContext)
		m_shaderContext = GLCgShaderContextPtr(new GLCgShaderContext);

	if (!m_defaultVS)
		m_defaultVS = LoadShaderFromString("defaultShader", gs2dglobal::defaultVSCode, Shader::SF_VERTEX, Shader::SP_MODEL_2, "sprite");

	if (!m_rectVS)
		m_rectVS = LoadShaderFromString("rectShader", gs2dglobal::defaultVSCode, Shader::SF_VERTEX, Shader::SP_MODEL_2, "rectangle");

	if (!m_fastVS)
		m_fastVS = LoadShaderFromString("fastShader", gs2dglobal::fastSimpleVSCode, Shader::SF_VERTEX, Shader::SP_MODEL_2, "fast");

	if (!m_defaultModulatePS)
		m_defaultModulatePS = LoadShaderFromString("modulate", gs2dglobal::defaultFragmentShaders, Shader::SF_PIXEL, Shader::SP_MODEL_1, "modulate");

	if (!m_defaultAddPS)
		m_defaultAddPS = LoadShaderFromString("add", gs2dglobal::defaultFragmentShaders, Shader::SF_PIXEL, Shader::SP_MODEL_1, "add");

	if (!m_defaultPS)
		m_defaultPS = LoadShaderFromString("minimal", gs2dglobal::defaultFragmentShaders, Shader::SF_PIXEL, Shader::SP_MODEL_1, "minimal");

	m_currentVS = m_defaultVS;
    m_currentPS = m_defaultPS;

	UpdateInternalShadersViewData(GetScreenSizeF(), false);

	m_rectRenderer = boost::shared_ptr<GLRectRenderer>(new GLRectRenderer());
	return true;
}

void GLVideo::UpdateInternalShadersViewData(const math::Vector2& screenSize, const bool invertY)
{
	UpdateViewMatrix(screenSize, m_ortho, m_zNear, m_zFar, invertY);

	UpdateShaderViewData(m_defaultVS, screenSize, m_ortho);
	UpdateShaderViewData(m_rectVS, screenSize, m_ortho);
	UpdateShaderViewData(m_fastVS, screenSize, m_ortho);
}

void GLVideo::UpdateViewMatrix(const math::Vector2& screenSize, math::Matrix4x4& ortho, const float znear, const float zfar, const bool invertY)
{
	math::Orthogonal(ortho, screenSize.x, screenSize.y * ((invertY) ? -1.0f : 1.0f), znear, zfar);
}

void GLVideo::UpdateShaderViewData(const ShaderPtr& shader, const math::Vector2& screenSize, const math::Matrix4x4& ortho)
{
	shader->SetConstant("screenSize", screenSize);
	shader->SetMatrixConstant("viewMatrix", ortho);
}

void GLVideo::Enable2DStates()
{
	const math::Vector2i screenSize(GetScreenSize());
	glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
	glDisable(GL_LIGHTING);
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

static void SetChannelClamp(const bool set)
{
	if (set)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
}

bool GLVideo::SetClamp(const bool set)
{
	m_clamp = set;
	glActiveTexture(GL_TEXTURE0);
	SetChannelClamp(set);
	return true;
}

bool GLVideo::GetClamp() const
{
	return m_clamp;
}

static void SetChannelFilterMode(const Video::TEXTUREFILTER_MODE tfm, const GLenum extension)
{
	if (tfm != Video::TM_NEVER)
	{
		glTexParameteri(extension, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(extension, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(extension, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(extension, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

bool GLVideo::SetFilterMode(const TEXTUREFILTER_MODE tfm)
{
	m_filter = tfm;
	glActiveTexture(GL_TEXTURE0);
	SetChannelFilterMode(tfm, m_textureExtension);
	return true;
}

Video::TEXTUREFILTER_MODE GLVideo::GetFilterMode() const
{
	return m_filter;
}

unsigned int GLVideo::GetMaxMultiTextures() const
{
	GLint units;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &units);
	return static_cast<GLint>(units);
}

bool GLVideo::UnsetTexture(const unsigned int passIdx)
{
	switch (passIdx)
	{
	case 0:
		glActiveTexture(GL_TEXTURE0);
		break;
	case 2:
		glActiveTexture(GL_TEXTURE2);
		break;
	case 3:
		glActiveTexture(GL_TEXTURE3);
		break;
	case 1:
	default:
		glActiveTexture(GL_TEXTURE1);
		break;
	}
	glBindTexture(m_textureExtension, 0);
	return true;
}

bool GLVideo::SetAlphaMode(const ALPHA_MODE mode)
{
	m_alphaMode = mode;

	switch(mode)
	{
	case AM_PIXEL:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glAlphaFunc(GL_GREATER, (GLclampf)m_alphaRef);
		glEnable(GL_ALPHA_TEST);
		break;
	case AM_ADD:
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glAlphaFunc(GL_GREATER, (GLclampf)m_alphaRef);
		glEnable(GL_ALPHA_TEST);
		break;
	case AM_ALPHA_TEST:
		glAlphaFunc(GL_GREATER, (GLclampf)m_alphaRef);
		glEnable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
	break;
	case AM_MODULATE:
		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		glAlphaFunc(GL_GREATER, (GLclampf)m_alphaRef);
		glEnable(GL_ALPHA_TEST);
	break;
	case AM_NONE:
	default:
		m_alphaMode = AM_NONE;
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
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

void GLVideo::SetZWrite(const bool enable)
{
	// dummy for OpenGL
}

bool GLVideo::GetZWrite() const
{
	return true;
}

bool GLVideo::BeginSpriteScene(const Color& dwBGColor)
{
	const Color color(dwBGColor != constant::ZERO ? dwBGColor : m_backgroundColor);
	math::Vector4 v;
	v.SetColor(color);
	glClearColor(v.x, v.y, v.z, v.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetAlphaMode(Video::AM_PIXEL);
	
	m_rendering = true;
	return true;
}

bool GLVideo::EndSpriteScene()
{
	m_rendering = false;
	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		ResetVideoMode(static_cast<unsigned int>(GetScreenSize().x), static_cast<unsigned int>(GetScreenSize().y), Texture::PF_DEFAULT);
	}
	return true;
}

bool GLVideo::Rendering() const
{
	return m_rendering;
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

ShaderPtr GLVideo::GetFontShader()
{
	return m_fastVS;
}

ShaderPtr GLVideo::GetOptimalVS()
{
	return m_defaultVS;
}

ShaderPtr GLVideo::GetDefaultVS()
{
	return m_defaultVS;
}

ShaderPtr GLVideo::GetDefaultPS()
{
	return m_defaultPS;
}

ShaderPtr GLVideo::GetDefaultModulatePS()
{
	return m_defaultModulatePS;
}

ShaderPtr GLVideo::GetDefaultAddPS()
{
	return m_defaultAddPS;
}

ShaderContextPtr GLVideo::GetShaderContext()
{
	return m_shaderContext;
}

ShaderPtr GLVideo::GetVertexShader()
{
	return m_currentVS;
}

bool GLVideo::SetVertexShader(ShaderPtr pShader)
{
	if (!pShader)
	{
		if (m_currentVS != m_defaultVS)
			m_currentVS->UnbindShader();
		m_currentVS = m_defaultVS;
	}
	else
	{
		if (pShader->GetShaderFocus() != Shader::SF_VERTEX)
		{
			ShowMessage("The shader set is not a vertex program", GSMT_ERROR);
			return false;
		}
		else
		{
			m_currentVS->UnbindShader();
			m_currentVS = pShader;
		}
	}

	const math::Vector2 screenSize = GetScreenSizeF();
	UpdateViewMatrix(screenSize, m_ortho, m_zNear, m_zFar, (m_currentTarget.lock()) ? true : false);
	UpdateShaderViewData(m_currentVS, screenSize, m_ortho);
	return true;
}

ShaderPtr GLVideo::GetPixelShader()
{
	return m_currentPS;
}

bool GLVideo::SetPixelShader(ShaderPtr pShader)
{
	if (pShader)
	{
		if (pShader->GetShaderFocus() != Shader::SF_PIXEL)
		{
			ShowMessage("The shader set is not a fragment program", GSMT_ERROR);
			return false;
		}
	}
    else
    {
        pShader = m_defaultPS;
    }

	m_currentPS = pShader;
	return true;
}

bool GLVideo::DrawRectangle(
	const math::Vector2 &v2Pos,
	const math::Vector2 &v2Size,
	const Color& color,
	const float angle,
	const Sprite::ENTITY_ORIGIN origin)
{
	return DrawRectangle(v2Pos, v2Size, color, color, color, color, angle, origin);
}

bool GLVideo::DrawRectangle(
	const math::Vector2 &v2Pos,
	const math::Vector2 &v2Size,
	const Color& color0,
	const Color& color1,
	const Color& color2,
	const Color& color3,
	const float angle,
	const Sprite::ENTITY_ORIGIN origin)
{
	if (v2Size == math::Vector2(0,0))
	{
		return true;
	}

	// TODO/TO-DO this is diplicated code: fix it
	math::Vector2 v2Center;
	switch (origin)
	{
	case Sprite::EO_CENTER:
	case Sprite::EO_RECT_CENTER:
		v2Center.x = v2Size.x / 2.0f;
		v2Center.y = v2Size.y / 2.0f;
		break;
	case Sprite::EO_RECT_CENTER_BOTTOM:
	case Sprite::EO_CENTER_BOTTOM:
		v2Center.x = v2Size.x / 2.0f;
		v2Center.y = v2Size.y;
		break;
	case Sprite::EO_RECT_CENTER_TOP:
	case Sprite::EO_CENTER_TOP:
		v2Center.x = v2Size.x / 2.0f;
		v2Center.y = 0.0f;
		break;
	case Sprite::EO_DEFAULT:
	default:
		v2Center.x = 0.0f;
		v2Center.y = 0.0f;
		break;
	};

	math::Matrix4x4 mRot;
	if (angle != 0.0f)
		mRot = math::RotateZ(math::DegreeToRadian(angle));
	m_rectVS->SetMatrixConstant("rotationMatrix", mRot);
	m_rectVS->SetConstant("size", v2Size);
	m_rectVS->SetConstant("entityPos", v2Pos);
	m_rectVS->SetConstant("center", v2Center);
	m_rectVS->SetConstant("color0", color0);
	m_rectVS->SetConstant("color1", color1);
	m_rectVS->SetConstant("color2", color2);
	m_rectVS->SetConstant("color3", color3);

	ShaderPtr prevVertexShader = GetVertexShader(), prevPixelShader = GetPixelShader();

	SetVertexShader(m_rectVS);
	m_shaderContext->DisableFragmentShader();

	UnsetTexture(0);
	UnsetTexture(1);
	GetVertexShader()->SetShader();
	m_rectRenderer->Draw(Sprite::RM_TWO_TRIANGLES);

	SetPixelShader(prevPixelShader);
	SetVertexShader(prevVertexShader);
	return true;
}

bool GLVideo::DrawLine(const math::Vector2 &p1, const math::Vector2 &p2, const Color& color1, const Color& color2)
{
	if (GetLineWidth() <= 0.0f)
		return true;
	if (p1 == p2)
		return true;

	static const math::Vector2 offsetFix(0.5f, 0.5f);
	const math::Vector2 a(p1 + offsetFix), b(p2 + offsetFix);
	const math::Vector2 v2Dir = a - b;

	const float length = v2Dir.Length() + 0.5f;
	const float angle  = math::RadianToDegree(math::GetAngle(v2Dir));

	DrawRectangle(a, math::Vector2(GetLineWidth(), length),
				  color2, color2, color1, color1,
				  angle, Sprite::EO_CENTER_BOTTOM);
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

unsigned int GLVideo::GetMaxRenderTargets() const
{
	return 1;
}

bool GLVideo::SetBlendMode(const unsigned int passIdx, const BLEND_MODE mode)
{
	m_blendMode = mode;
	switch (passIdx)
	{
	case 0:
		glActiveTexture(GL_TEXTURE0);
		break;
	default:
		glActiveTexture(GL_TEXTURE1);
		break;
	}
	const GLfloat v = (mode == BM_MODULATE) ? GL_MODULATE : GL_ADD;
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, v);
	return true;
}

Video::BLEND_MODE GLVideo::GetBlendMode(const unsigned int passIdx) const
{
	return m_blendMode;
}

bool GLVideo::SetScissor(const bool& enable)
{
	if (enable)
	{
		glEnable(GL_SCISSOR_TEST);
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}
	return true;
}

bool GLVideo::SetScissor(const math::Rect2D& rect)
{
	SetScissor(true);
	GLint posY;
	TexturePtr target = m_currentTarget.lock();
	if (target)
	{
		posY = static_cast<GLint>(rect.pos.y);
	}
	else
	{
		posY = static_cast<GLint>(GetScreenSize().y) - static_cast<GLint>(rect.pos.y + rect.size.y);
	}

	if (m_scissor != rect)
	{
		m_scissor = rect;
		glScissor(static_cast<GLint>(rect.pos.x), posY,
				  static_cast<GLsizei>(rect.size.x), static_cast<GLsizei>(rect.size.y));
	}
	return true;
}

math::Rect2D GLVideo::GetScissor() const
{
	return m_scissor;
}

void GLVideo::UnsetScissor()
{
	SetScissor(false);
}

Shader::SHADER_PROFILE GLVideo::GetHighestVertexProfile() const
{
	return GLCgShaderContext::CGProfileToGSProfile(m_shaderContext->GetLatestVertexProfile());
}

Shader::SHADER_PROFILE GLVideo::GetHighestPixelProfile() const
{
	return GLCgShaderContext::CGProfileToGSProfile(m_shaderContext->GetLatestFragmentProfile());
}

bool GLVideo::SetRenderTarget(SpritePtr pTarget, const unsigned int target)
{
	if (!pTarget)
	{
		m_currentTarget.reset();
		UnbindFrameBuffer();
	}
	else
	{
		if (pTarget->GetType() == Sprite::T_TARGET)
		{
			m_currentTarget = pTarget->GetTexture();
		}
		else
		{
			Message(GS_L("The current sprite has no render target texture"), GSMT_ERROR);
		}
	}
	return true;
}

math::Vector2 GLVideo::GetCurrentTargetSize() const
{
	TexturePtr target = m_currentTarget.lock();
	if (target)
	{
		return math::Vector2(static_cast<float>(target->GetProfile().width), static_cast<float>(target->GetProfile().height));
	}
	else
	{
		return GetScreenSizeF();
	}
}

bool GLVideo::BeginTargetScene(const Color& dwBGColor, const bool clear)
{
	// explicit static cast for better performance
	TexturePtr texturePtr = m_currentTarget.lock();
	if (!texturePtr)
	{
		Message(GS_L("There's no render target"), GSMT_ERROR);
	}
	Texture *pTexture = texturePtr.get(); // safety compile-time error checking
	GLTexture *pGLTexture = static_cast<GLTexture*>(pTexture); // safer direct cast
	const GLuint target = pGLTexture->GetTextureInfo().m_frameBuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, target);

	CheckFrameBufferStatus(target, pGLTexture->GetTextureInfo().m_texture, false);

	if (clear)
	{
		math::Vector4 color;
		color.SetColor(dwBGColor);
		glClearColor(color.x, color.y, color.z, color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	UpdateInternalShadersViewData(GetScreenSizeF(), true);
	m_rendering = true;
	return true;
}

bool GLVideo::EndTargetScene()
{
	SetRenderTarget(SpritePtr(), 0);
	m_rendering = false;
	UnbindFrameBuffer();

	UpdateInternalShadersViewData(GetScreenSizeF(), false);

	return true;
}

bool GLVideo::SaveScreenshot(
	const str_type::char_t* name,
	const Texture::BITMAP_FORMAT fmt,
	math::Rect2D rect)
{
	str_type::string fileName = name;
	const str_type::string ext = ".tga";

	// fix file name format
	if (!Platform::IsExtensionRight(fileName, ext))
		fileName.append(ext);

	// recognize default value for full screen shot
	if (rect.size.x <= 0 || rect.size.y <= 0)
	{
		rect.pos = math::Vector2i(0,0);
		rect.size = GetScreenSize();
	}

	const int result = SOIL_save_screenshot(
		fileName.c_str(),
		SOIL_SAVE_TYPE_TGA,
		rect.pos.x,
		rect.pos.y,
		rect.size.x,
		rect.size.y);

	return (result != 0);
}

} // namespace gs2d
