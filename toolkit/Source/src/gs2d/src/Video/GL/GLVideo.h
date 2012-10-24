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

#ifndef GS2D_GL_VIDEO_H_
#define GS2D_GL_VIDEO_H_

#include "../../Video.h"

#include "GLInclude.h"
#include "GLRectRenderer.h"
#include "Cg/GLCGShaderContext.h"

namespace gs2d {

class GLVideo : public virtual Video
{
	TEXTUREFILTER_MODE m_filter;
	ALPHA_MODE m_alphaMode;
	GLenum m_textureExtension;
	const double m_alphaRef;
	const float m_zNear, m_zFar;
	math::Matrix4x4 m_ortho;
	Color m_backgroundColor;
	bool m_rendering;

	GLCgShaderContextPtr m_shaderContext;
	GLRectRenderer m_rectRenderer;

	void Enable2DStates();

protected:
	bool StartApplication(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const bool windowed,
		const bool sync,
		const Texture::PIXEL_FORMAT pfBB = Texture::PF_UNKNOWN,
		const bool maximizable = false);

public:
	GLVideo();

	TexturePtr CreateTextureFromFileInMemory(
		const void *pBuffer,
		const unsigned int bufferLength,
		Color mask,
		const unsigned int width,
		const unsigned int height,
		const unsigned int nMipMaps);
	
	TexturePtr LoadTextureFromFile(
		const str_type::string& fileName,
		Color mask,
		const unsigned int width,
		const unsigned int height,
		const unsigned int nMipMaps);
	
	TexturePtr CreateRenderTargetTexture(
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT fmt);
	
	SpritePtr CreateSprite(
		GS_BYTE *pBuffer,
		const unsigned int bufferLength,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0);
	
	SpritePtr CreateSprite(
		const str_type::string& fileName,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0);
	
	SpritePtr CreateRenderTarget(
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT format = Texture::TF_DEFAULT);

	ShaderPtr LoadShaderFromFile(
		const str_type::string& fileName,
		const Shader::SHADER_FOCUS focus,
		const Shader::SHADER_PROFILE profile = Shader::SP_HIGHEST,
		const char *entry = 0);
	
	ShaderPtr LoadShaderFromString(
		const str_type::string& shaderName,
		const std::string& codeAsciiString,
		const Shader::SHADER_FOCUS focus,
		const Shader::SHADER_PROFILE profile = Shader::SP_HIGHEST,
		const char *entry = 0);
	
	boost::any GetVideoInfo();
	
	ShaderPtr GetFontShader();
	ShaderPtr GetOptimalVS();
	ShaderPtr GetDefaultVS();
	ShaderPtr GetVertexShader();
	ShaderPtr GetPixelShader();
	ShaderContextPtr GetShaderContext();
	bool SetVertexShader(ShaderPtr pShader);
	bool SetPixelShader(ShaderPtr pShader);
	Shader::SHADER_PROFILE GetHighestVertexProfile() const;
	Shader::SHADER_PROFILE GetHighestPixelProfile() const;
	
	boost::any GetGraphicContext();
	
	VIDEO_MODE GetVideoMode(const unsigned int modeIdx) const;
	unsigned int GetVideoModeCount();
	
	bool ResetVideoMode(
		const VIDEO_MODE& mode,
		const bool toggleFullscreen = false);
	
	bool ResetVideoMode(
		const unsigned int width,
		const unsigned int height,
		const Texture::PIXEL_FORMAT pfBB,
		const bool toggleFullscreen = false);
	
	bool SetRenderTarget(SpritePtr pTarget, const unsigned int target);
	unsigned int GetMaxRenderTargets() const;
	unsigned int GetMaxMultiTextures() const;
	bool SetBlendMode(const unsigned int passIdx, const BLEND_MODE mode);
	BLEND_MODE GetBlendMode(const unsigned int passIdx) const;
	bool UnsetTexture(const unsigned int passIdx);
	
	void SetZBuffer(const bool enable);
	bool GetZBuffer() const;
	
	void SetZWrite(const bool enable);
	bool GetZWrite() const;
	
	bool SetClamp(const bool set);
	bool GetClamp() const;
	
	bool SetSpriteDepth(const float depth);
	float GetSpriteDepth() const;
	
	void SetLineWidth(const float width);
	float GetLineWidth() const;
	
	bool SetCameraPos(const math::Vector2 &pos);
	bool MoveCamera(const math::Vector2 &dir);
	math::Vector2 GetCameraPos() const;
	
	void RoundUpPosition(const bool roundUp);
	bool IsRoundingUpPosition() const;
	
	bool SetScissor(const math::Rect2D &rect);
	bool SetScissor(const bool &enable);
	math::Rect2D GetScissor() const;
	void UnsetScissor();
	
	bool DrawLine(const math::Vector2 &p1, const math::Vector2 &p2, const Color& color1, const Color& color2);
	
	bool DrawRectangle(
		const math::Vector2 &v2Pos,
		const math::Vector2 &v2Size,
		const Color& color,
		const float angle = 0.0f,
		const Sprite::ENTITY_ORIGIN origin = Sprite::EO_DEFAULT);
	
	bool DrawRectangle(
		const math::Vector2 &v2Pos,
		const math::Vector2 &v2Size,
		const Color& color0,
		const Color& color1,
		const Color& color2,
		const Color& color3,
		const float angle = 0.0f,
		const Sprite::ENTITY_ORIGIN origin = Sprite::EO_DEFAULT);
	
	void SetBGColor(const Color& backgroundColor);
	Color GetBGColor() const;
	
	bool BeginSpriteScene(const Color& dwBGColor = constant::ZERO);
	bool EndSpriteScene();
	bool BeginTargetScene(const Color& dwBGColor = constant::ZERO, const bool clear = true);
	bool EndTargetScene();
	
	bool SetAlphaMode(const ALPHA_MODE mode);
	ALPHA_MODE GetAlphaMode() const;
	
	bool SetFilterMode(const TEXTUREFILTER_MODE tfm);
	TEXTUREFILTER_MODE GetFilterMode() const;
	
	bool Rendering() const;
	
	bool SaveScreenshot(
		const wchar_t *wcsName,
		const Texture::BITMAP_FORMAT fmt = Texture::BF_BMP,
		math::Rect2D rect = math::Rect2D(0,0,0,0));

	const GLRectRenderer& GetRectRenderer() const;
};

}

#endif
