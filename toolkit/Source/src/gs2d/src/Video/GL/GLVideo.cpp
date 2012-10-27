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

#include "../cgShaderCode.h"

#include "GLTexture.h"

namespace gs2d {

void GLVideo::UnbindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLVideo::GLVideo() :
	m_filter(Video::TM_UNKNOWN),
	m_textureExtension(GL_TEXTURE_2D),
	m_alphaMode(AM_UNKNOWN),
	m_alphaRef(0.02),
	m_zFar(5.0f),
	m_zNear(0.0f),
	m_backgroundColor(gs2d::constant::BLACK),
	m_rendering(false)
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

	m_currentVS = m_defaultVS;

	UpdateViewMatrix();

	UpdateInternalShadersViewData();

	return true;
}

void GLVideo::UpdateInternalShadersViewData()
{
	UpdateShaderViewData(m_defaultVS);
	UpdateShaderViewData(m_rectVS);
	UpdateShaderViewData(m_fastVS);
}

void GLVideo::UpdateViewMatrix()
{
	math::Orthogonal(m_ortho, GetScreenSizeF().x, GetScreenSizeF().y, m_zNear, m_zFar);
}

void GLVideo::UpdateShaderViewData(const ShaderPtr& shader)
{
	shader->SetConstant("screenSize", GetScreenSizeF());
	shader->SetMatrixConstant("viewMatrix", m_ortho);
}

void GLVideo::Enable2DStates()
{
	const math::Vector2i screenSize(GetScreenSize());
	glViewport(0, 0, static_cast<GLint>(screenSize.x), static_cast<GLint>(screenSize.y));
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DITHER);

	glEnable(GL_TEXTURE_2D);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
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

bool GLVideo::BeginSpriteScene(const Color& dwBGColor)
{
	const Color color(dwBGColor != constant::ZERO ? dwBGColor : m_backgroundColor);
	math::Vector4 v;
	v.SetColor(color);
	glClearColor(v.x, v.y, v.z, v.w);
	glClear(GL_COLOR_BUFFER_BIT);
	SetAlphaMode(Video::AM_PIXEL);
	
	m_rendering = true;
	return true;
}

bool GLVideo::EndSpriteScene()
{
	m_rendering = false;
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
	return m_rectRenderer;
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

ShaderPtr GLVideo::GetVertexShader()
{
	return m_currentVS;
}

ShaderContextPtr GLVideo::GetShaderContext()
{
	return m_shaderContext;
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
	UpdateShaderViewData(m_currentVS);
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
	SetPixelShader(ShaderPtr());

	glBindTexture(m_textureExtension, 0);
	GetVertexShader()->SetShader();
	m_rectRenderer.Draw(Sprite::RM_TWO_TRIANGLES);

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

	const math::Vector2 v2Dir = p1 - p2;

	const float length = math::Distance(p1, p2);
	const float angle  = math::RadianToDegree(math::GetAngle(v2Dir));

	DrawRectangle(p1, math::Vector2(GetLineWidth(), length),
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
	return 0;
}

ShaderPtr GLVideo::GetPixelShader()
{
	return ShaderPtr();
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

bool GLVideo::BeginTargetScene(const Color& dwBGColor, const bool clear)
{
	m_rendering = true;
	return false;
}

bool GLVideo::EndTargetScene()
{
	m_rendering = false;
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
