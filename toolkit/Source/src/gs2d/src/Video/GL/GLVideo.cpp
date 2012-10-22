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

#include "GLVideo.h"

namespace gs2d {

GLVideo::GLVideo() :
	m_filter(Video::TM_UNKNOWN),
	m_textureExtension(GL_TEXTURE_2D),
	m_alphaMode(AM_UNKNOWN),
	m_alphaRef(0.02),
	m_zFar(5.0f),
	m_zNear(0.0f),
	m_backgroundColor(0xFFFFFFFF)
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
	SetBGColor(gs2d::constant::BLACK);

	math::Orthogonal(m_ortho, GetScreenSizeF().x, GetScreenSizeF().y, m_zNear, m_zFar);

	SetZBuffer(false);
	SetZWrite(false);

	Enable2DStates();
	return true;
}

void GLVideo::Enable2DStates()
{
	const math::Vector2i screenSize(GetScreenSize());
	glViewport(0, 0, static_cast<GLint>(screenSize.x), static_cast<GLint>(screenSize.y));
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DITHER);
}

bool GLVideo::SetFilterMode(const TEXTUREFILTER_MODE tfm)
{
	m_filter = tfm;
	if (GetFilterMode() != TM_NEVER)
	{
		glTexParameteri(m_textureExtension, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(m_textureExtension, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(m_textureExtension, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(m_textureExtension, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	return true;
}

Video::TEXTUREFILTER_MODE GLVideo::GetFilterMode() const
{
	return m_filter;
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
	if (enable)
		glDepthMask(GL_TRUE);
	else
		glDepthMask(GL_FALSE);
}

bool GLVideo::GetZWrite() const
{
	GLboolean enabled;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &enabled);
	return (enabled == GL_TRUE) ? true : false;
}




















TexturePtr GLVideo::CreateTextureFromFileInMemory(
	const void *pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	return TexturePtr();
}

TexturePtr GLVideo::LoadTextureFromFile(
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	return TexturePtr();
}

TexturePtr GLVideo::CreateRenderTargetTexture(
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT fmt)
{
	return TexturePtr();
}

SpritePtr GLVideo::CreateSprite(
	GS_BYTE *pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	return SpritePtr();
}

SpritePtr GLVideo::CreateSprite(
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	return SpritePtr();
}

SpritePtr GLVideo::CreateRenderTarget(
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT format)
{
	return SpritePtr();
}

ShaderPtr GLVideo::LoadShaderFromFile(
	const str_type::string& fileName,
	const Shader::SHADER_FOCUS focus,
	const Shader::SHADER_PROFILE profile,
	const char *entry)
{
	return ShaderPtr();
}

ShaderPtr GLVideo::LoadShaderFromString(
	const str_type::string& shaderName,
	const std::string& codeAsciiString,
	const Shader::SHADER_FOCUS focus,
	const Shader::SHADER_PROFILE profile,
	const char *entry)
{
	return ShaderPtr();
}

boost::any GLVideo::GetVideoInfo()
{
	return 0;
}

ShaderPtr GLVideo::GetFontShader()
{
	return ShaderPtr();
}

ShaderPtr GLVideo::GetOptimalVS()
{
	return ShaderPtr();
}

ShaderPtr GLVideo::GetDefaultVS()
{
	return ShaderPtr();
}

ShaderPtr GLVideo::GetVertexShader()
{
	return ShaderPtr();
}

ShaderPtr GLVideo::GetPixelShader()
{
	return ShaderPtr();
}

ShaderContextPtr GLVideo::GetShaderContext()
{
	return ShaderContextPtr();
}

bool GLVideo::SetVertexShader(ShaderPtr pShader)
{
	return false;
}

bool GLVideo::SetPixelShader(ShaderPtr pShader)
{
	return false;
}

Shader::SHADER_PROFILE GLVideo::GetHighestVertexProfile() const
{
	return Shader::SP_NONE;
}

Shader::SHADER_PROFILE GLVideo::GetHighestPixelProfile() const
{
	return Shader::SP_NONE;
}

boost::any GLVideo::GetGraphicContext()
{
	return 0;
}

Video::VIDEO_MODE GLVideo::GetVideoMode(const unsigned int modeIdx) const
{
	return Video::VIDEO_MODE();
}

unsigned int GLVideo::GetVideoModeCount()
{
	return 0;
}

bool GLVideo::ResetVideoMode(
	const VIDEO_MODE& mode,
	const bool toggleFullscreen)
{
	return false;
}

bool GLVideo::ResetVideoMode(
	const unsigned int width,
	const unsigned int height,
	const Texture::PIXEL_FORMAT pfBB,
	const bool toggleFullscreen)
{
	return false;
}

bool GLVideo::SetRenderTarget(SpritePtr pTarget, const unsigned int target)
{
	return false;
}

unsigned int GLVideo::GetMaxRenderTargets() const
{
	return 0;
}

unsigned int GLVideo::GetMaxMultiTextures() const
{
	return 0;
}

bool GLVideo::SetBlendMode(const unsigned int passIdx, const BLEND_MODE mode)
{
	return false;
}

Video::BLEND_MODE GLVideo::GetBlendMode(const unsigned int passIdx) const
{
	return Video::BM_MODULATE;
}

bool GLVideo::UnsetTexture(const unsigned int passIdx)
{
	return false;
}

bool GLVideo::SetClamp(const bool set)
{
	return false;
}

bool GLVideo::GetClamp() const
{
	return false;
}

bool GLVideo::SetSpriteDepth(const float depth)
{
	return false;
}

float GLVideo::GetSpriteDepth() const
{
	return 0.0f;
}

void GLVideo::SetLineWidth(const float width)
{
}

float GLVideo::GetLineWidth() const
{
	return 0.0f;
}

bool GLVideo::SetCameraPos(const math::Vector2 &pos)
{
	return false;
}

bool GLVideo::MoveCamera(const math::Vector2 &dir)
{
	return false;
}

math::Vector2 GLVideo::GetCameraPos() const
{
	return math::Vector2(0.0f, 0.0f);
}

void GLVideo::RoundUpPosition(const bool roundUp)
{
}

bool GLVideo::IsRoundingUpPosition() const
{
	return false;
}

bool GLVideo::SetScissor(const math::Rect2D &rect)
{
	return false;
}

bool GLVideo::SetScissor(const bool &enable)
{
	return false;
}

math::Rect2D GLVideo::GetScissor() const
{
	return math::Rect2D();
}

void GLVideo::UnsetScissor()
{
}

bool GLVideo::DrawLine(const math::Vector2 &p1, const math::Vector2 &p2, const Color& color1, const Color& color2)
{
	return false;
}

bool GLVideo::DrawRectangle(
	const math::Vector2 &v2Pos,
	const math::Vector2 &v2Size,
	const Color& color,
	const float angle,
	const Sprite::ENTITY_ORIGIN origin)
{
	return false;
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
	return false;
}

void GLVideo::SetBGColor(const Color& backgroundColor)
{
}

Color GLVideo::GetBGColor() const
{
	return Color(0xFF00000000);
}

bool GLVideo::BeginSpriteScene(const Color& dwBGColor)
{
	return false;
}

bool GLVideo::EndSpriteScene()
{
	return false;
}

bool GLVideo::BeginTargetScene(const Color& dwBGColor, const bool clear)
{
	return false;
}

bool GLVideo::EndTargetScene()
{
	return false;
}

bool GLVideo::Rendering() const
{
	return false;
}

bool GLVideo::SaveScreenshot(
	const wchar_t *wcsName,
	const Texture::BITMAP_FORMAT fmt,
	math::Rect2D rect)
{
	return false;
}

}
