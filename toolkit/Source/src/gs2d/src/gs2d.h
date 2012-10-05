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

#ifndef GS2D_H_
#define GS2D_H_

#include <sstream>
#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "gs2dapplication.h"
#include "gs2dshader.h"
#include "gs2dsprite.h"
#include "gs2dwindow.h"

namespace gs2d {

enum GS_ALPHA_MODE
{
	GSAM_PIXEL = 0,
	GSAM_ADD = 1,
	GSAM_ALPHA_TEST = 2,
	GSAM_NONE = 3,
	GSAM_MODULATE = 4
};

enum GS_TEXTUREFILTER_MODE
{
	GSTM_NEVER = 0,
	GSTM_IFNEEDED = 1,
	GSTM_ALWAYS = 2
};

enum GS_BLEND_MODE
{
	GSBM_ADD = 0,
	GSBM_MODULATE = 1,
};

/**
 * \brief Abstracts all video device operations
 *
 * This class works as a renderer. Sending all data to
 * the video device and telling it how to render it.
 * It also instantiates textures, sprites and shaders.
 */
class Video :
	public Application,
	public Window
{
public:
	struct VIDEO_MODE
	{
		bool operator == (const VIDEO_MODE &other) const
		{
			return (width == other.width && height == other.height && pf == other.pf);
		}
		bool operator < (const VIDEO_MODE &other) const
		{
			if (pf < other.pf)
			{
				return true;
			}
			else if (pf == other.pf
					&& width*height < other.width*other.height)
			{
				return true;
			}
			return false;
		}
		unsigned int width, height;
		GS_PIXEL_FORMAT pf;
		GS_DWORD idx;
	};

	typedef std::list<VIDEO_MODE> VIDEO_MODE_LIST;

	/// Loads the texture from a file in virtual memory
	virtual TexturePtr CreateTextureFromFileInMemory(
		const void *pBuffer,
		const unsigned int bufferLength,
		Color mask,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0) = 0;

	/// Loads the texture from a file in a hard disk
	virtual TexturePtr LoadTextureFromFile(
		const str_type::string& fileName,
		Color mask,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0) = 0;

	/// Creates a texture as render target.
	virtual TexturePtr CreateRenderTargetTexture(
		const unsigned int width,
		const unsigned int height,
		const GS_TARGET_FORMAT fmt) = 0;

	/// Creates a sprite from a texture in virtual memory
	virtual SpritePtr CreateSprite(
		GS_BYTE *pBuffer,
		const unsigned int bufferLength,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0) = 0;

	/// Creates a sprite from a texture in a file
	virtual SpritePtr CreateSprite(
		const str_type::string& fileName,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0) = 0;

	/// Creates a sprite as render target
	virtual SpritePtr CreateRenderTarget(
		const unsigned int width,
		const unsigned int height,
		const GS_TARGET_FORMAT format = GSTF_DEFAULT) = 0;

	/// Create a shader object and load/compile it.
	virtual ShaderPtr LoadShaderFromFile(
		const str_type::string& fileName,
		const GS_SHADER_FOCUS focus,
		const GS_SHADER_PROFILE profile = GSSP_HIGHEST,
		const char *entry = 0) = 0;

	/// Create a shader object and load/compile it.
	virtual ShaderPtr LoadShaderFromString(
		const str_type::string& shaderName,
		const std::string& codeAsciiString,
		const GS_SHADER_FOCUS focus,
		const GS_SHADER_PROFILE profile = GSSP_HIGHEST,
		const char *entry = 0) = 0;

	virtual boost::any GetVideoInfo() = 0;

	virtual ShaderPtr GetFontShader() = 0;
	virtual ShaderPtr GetOptimalVS() = 0;
	virtual ShaderPtr GetDefaultVS() = 0;
	virtual ShaderPtr GetVertexShader() = 0;
	virtual ShaderPtr GetPixelShader() = 0;
	virtual ShaderContextPtr GetShaderContext() = 0;
	virtual bool SetVertexShader(ShaderPtr pShader) = 0;
	virtual bool SetPixelShader(ShaderPtr pShader) = 0;
	virtual GS_SHADER_PROFILE GetHighestVertexProfile() const = 0;
	virtual GS_SHADER_PROFILE GetHighestPixelProfile() const = 0;

	virtual boost::any GetGraphicContext() = 0;

	virtual VIDEO_MODE GetVideoMode(const unsigned int modeIdx) const = 0;
	virtual unsigned int GetVideoModeCount() = 0;

	virtual bool ResetVideoMode(
		const VIDEO_MODE& mode,
		const bool toggleFullscreen = false) = 0;

	virtual bool ResetVideoMode(
		const unsigned int width,
		const unsigned int height,
		const GS_PIXEL_FORMAT pfBB,
		const bool toggleFullscreen = false) = 0;

	virtual bool SetRenderTarget(SpritePtr pTarget, const unsigned int target = 0) = 0;
	virtual unsigned int GetMaxRenderTargets() const = 0;
	virtual unsigned int GetMaxMultiTextures() const = 0;
	virtual bool SetBlendMode(const unsigned int passIdx, const GS_BLEND_MODE mode) = 0;
	virtual GS_BLEND_MODE GetBlendMode(const unsigned int passIdx) const = 0;
	virtual bool UnsetTexture(const unsigned int passIdx) = 0;

	virtual void SetZBuffer(const bool enable) = 0;
	virtual bool GetZBuffer() const = 0;

	virtual void SetZWrite(const bool enable) = 0;
	virtual bool GetZWrite() const = 0;

	virtual bool SetClamp(const bool set) = 0;
	virtual bool GetClamp() const = 0;

	virtual bool SetSpriteDepth(const float depth) = 0;
	virtual float GetSpriteDepth() const = 0;

	virtual void SetLineWidth(const float width) = 0;
	virtual float GetLineWidth() const = 0;

	virtual bool SetCameraPos(const math::Vector2 &pos) = 0;
	virtual bool MoveCamera(const math::Vector2 &dir) = 0;
	virtual math::Vector2 GetCameraPos() const = 0;

	virtual void RoundUpPosition(const bool roundUp) = 0;
	virtual bool IsRoundingUpPosition() const = 0;

	virtual bool SetScissor(const math::Rect2D &rect) = 0;
	virtual bool SetScissor(const bool &enable) = 0;
	virtual math::Rect2D GetScissor() const = 0;
	virtual void UnsetScissor() = 0;

	virtual math::Vector2 ComputeCarretPosition(const str_type::string& font, const str_type::string& text, const unsigned int pos) = 0;
	virtual math::Vector2 ComputeTextBoxSize(const str_type::string& font, const str_type::string& text) = 0;

	virtual unsigned int FindClosestCarretPosition(
		const str_type::string& font,
		const str_type::string &text,
		const math::Vector2 &textPos,
		const math::Vector2 &reference) = 0;

	virtual bool DrawBitmapText(
		const math::Vector2 &v2Pos,
		const str_type::string& text,
		const str_type::string& font,
		const Color& color,
		const float scale = 1.0f) = 0;

	virtual bool DrawLine(const math::Vector2 &p1, const math::Vector2 &p2, const Color& color1, const Color& color2) = 0;

	virtual bool DrawRectangle(
		const math::Vector2 &v2Pos,
		const math::Vector2 &v2Size,
		const Color& color,
		const float angle = 0.0f,
		const GS_ENTITY_ORIGIN origin = GSEO_DEFAULT) = 0;

	virtual bool DrawRectangle(
		const math::Vector2 &v2Pos,
		const math::Vector2 &v2Size,
		const Color& color0,
		const Color& color1,
		const Color& color2,
		const Color& color3,
		const float angle = 0.0f,
		const GS_ENTITY_ORIGIN origin = GSEO_DEFAULT) = 0;

	virtual void SetBGColor(const Color& backgroundColor) = 0;
	virtual Color GetBGColor() const = 0;

	virtual bool BeginSpriteScene(const Color& dwBGColor = constant::ZERO) = 0;
	virtual bool EndSpriteScene() = 0;
	virtual bool BeginTargetScene(const Color& dwBGColor = constant::ZERO, const bool clear = true) = 0;
	virtual bool EndTargetScene() = 0;

	virtual bool SetAlphaMode(const GS_ALPHA_MODE mode) = 0;
	virtual GS_ALPHA_MODE GetAlphaMode() const = 0;

	virtual bool SetFilterMode(const GS_TEXTUREFILTER_MODE tfm) = 0;
	virtual GS_TEXTUREFILTER_MODE GetFilterMode() const = 0;

	virtual bool Rendering() const = 0;

	virtual bool SaveScreenshot(
		const wchar_t *wcsName,
		const GS_BITMAP_FORMAT fmt = GSBF_BMP,
		math::Rect2D rect = math::Rect2D(0,0,0,0)) = 0;

private:
	virtual bool StartApplication(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const bool windowed,
		const bool sync,
		const GS_PIXEL_FORMAT pfBB = GSPF_UNKNOWN,
		const bool maximizable = false) = 0;
};

/**
 * \brief Stores and renders bitmap fonts
 *
 * This class parses text fnt files generated with the AngelCode's bitmap
 * font generator and renders it. The parser code was originally written by Promit
 * and posted in this thread: http://www.gamedev.net/community/forums/topic.asp?topic_id=330742
 */
class BitmapFont
{
	struct CHAR_DESCRIPTOR
	{
		//clean 16 bytes
		float x, y;
		float width, height;
		float xOffset, yOffset;
		float xAdvance;
		int page;

		CHAR_DESCRIPTOR() : x(0), y(0), width(0), height(0), xOffset(0), yOffset(0),
			xAdvance(0), page(0)
		{ }
	};

	struct CHARSET
	{
		CHARSET() : paddingDown(0), paddingUp(0), paddingLeft(0), paddingRight(0) {}
		float lineHeight;
		float base;
		float width, height;
		int pages;
		CHAR_DESCRIPTOR chars[65536];
		std::vector<str_type::string> textureNames;
		float paddingUp, paddingRight, paddingDown, paddingLeft;
	} m_charSet;

	bool ParseFNTString(const str_type::string& str);

	std::vector<SpritePtr> m_bitmaps;

public:
	bool IsLoaded() const;
	BitmapFont(
		VideoWeakPtr video,
		const str_type::string& fileName,
		const str_type::string& str);

	math::Vector2 DrawBitmapText(
		const math::Vector2 &pos,
		const str_type::string& text,
		const Color& color,
		const float scale = 1.0f);

	math::Vector2 ComputeTextBoxSize(const str_type::string &text);
	math::Vector2 ComputeCarretPosition(const str_type::string &text, const unsigned int pos);

	unsigned int FindClosestCarretPosition(
		const str_type::string &text,
		const math::Vector2 &textPos,
		const math::Vector2 &reference);
};

typedef boost::shared_ptr<BitmapFont> BitmapFontPtr;

/// When called once per frame, returns the last frame time delta
GS2D_API unsigned long ComputeElapsedTime(ApplicationPtr app);
GS2D_API float ComputeElapsedTimeF(ApplicationPtr app);

/// Instantiate a Video object (must be defined in the API specific code)
GS2D_API VideoPtr CreateVideo(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Platform::FileIOHubPtr& fileIOHub,
	const GS_PIXEL_FORMAT pfBB = GSPF_UNKNOWN,
	const bool maximizable = false);

#if defined(ANDROID) || defined(APPLE_IOS)
/// Instantiate a Video object (must be defined in the API specific code)
GS2D_API VideoPtr CreateVideo(const unsigned int width, const unsigned int height, const Platform::FileIOHubPtr& fileIOHub);
#endif

} // namespace gs2d

#endif
