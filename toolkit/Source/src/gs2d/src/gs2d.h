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

#include "gs2dshader.h"

#include <sstream>
#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Platform/FileIOHub.h"

namespace gs2d {

enum GS_ENTITY_ORIGIN
{
	GSEO_DEFAULT = 0,
	GSEO_CENTER = 1,
	GSEO_CENTER_BOTTOM = 2,
	GSEO_CENTER_TOP = 3,
	GSEO_RECT_CENTER = 4,
	GSEO_RECT_CENTER_BOTTOM = 5,
	GSEO_RECT_CENTER_TOP = 6,
};

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

enum GS_BITMAP_FORMAT
{
	GSBF_BMP = 0,
	GSBF_JPG = 1,
	GSBF_PNG,
	GSBF_TGA,
	GSBF_DDS,
	GSBF_HDR,
};

enum GS_RECT_MODE
{
	GSRM_TWO_TRIANGLES = 0,
	GSRM_THREE_TRIANGLES = 1,
	GSRM_FOUR_TRIANGLES,
};

enum GS_TARGET_FORMAT
{
	GSTF_DEFAULT = 0,
	GSTF_ARGB = 1,
	GSTF_NONE = 2
};

enum GS_PIXEL_FORMAT
{
	GSPF_16BIT = 0,
	GSPF_32BIT = 1,
	GSPF_UNKNOWN = 2,
	GSPF_DEFAULT = 3,
};

enum GS_BLEND_MODE
{
	GSBM_ADD = 0,
	GSBM_MODULATE = 1,
};

/// Shows an error, warning or info message to the user
void ShowMessage(str_type::stringstream &stream, const GS_MESSAGE_TYPE type = GSMT_ERROR);

/// Shows an error, warning or info message to the user
void ShowMessage(const str_type::string& str, const GS_MESSAGE_TYPE type = GSMT_ERROR);


/**
 * \brief Abstracts all application related methods
 *
 * This class contains methods that will handle the application
 * taks, such as events and timer.
 */
class Application
{
public:
	enum APP_STATUS
	{
		APP_OK = 0,
		APP_QUIT = 1,
		APP_SKIP = 2
	};

	enum TIME_UNITY
	{
		TU_MILLISECONDS = 0,
		TU_SECONDS = 1,
		TU_MINUTES = 2,
		TU_HOURS = 3
	};

	class ScreenSizeChangeListener
	{
	public:
		virtual void ScreenSizeChanged(const math::Vector2& newScreenSize) = 0;
	};
	typedef boost::shared_ptr<ScreenSizeChangeListener> ScreenSizeChangeListenerPtr;
	typedef boost::weak_ptr<ScreenSizeChangeListener> ScreenSizeChangeListenerWeakPtr;

	/// Presents the back buffer, handle events and clears the screen
	virtual bool ManageLoop() = 0;

	/// Returns the client's current screen size in pixels
	virtual math::Vector2i GetClientScreenSize() const = 0;

	/// Handles application events
	virtual APP_STATUS HandleEvents() = 0;
	virtual float GetFPSRate() const = 0;

	/** \brief Sends a message to the user.
	 * \param text Zero terminated string of the message.
	 * \param type Type of the message.
	 */
	virtual void Message(const str_type::string& text, const GS_MESSAGE_TYPE type = GSMT_ERROR) const = 0;

	/** \brief Returns the current application elapsed time.
	 * \param unity Time unity.
	 */
	virtual unsigned long GetElapsedTime(const TIME_UNITY unity = TU_MILLISECONDS) const = 0;

	/** \brief Returns the current application elapsed time (floating point value).
	 * \param unity Time unity.
	 */
	virtual float GetElapsedTimeF(const TIME_UNITY unity = TU_MILLISECONDS) const = 0;

	/// Resets the application timer
	virtual void ResetTimer() = 0;

	virtual void ForwardCommand(const str_type::string& cmd) = 0;
	virtual str_type::string PullCommands() = 0;

	/// Posts a quit message
	virtual void Quit() = 0;

	virtual str_type::string GetPlatformName() const = 0;

	virtual Platform::FileIOHubPtr GetFileIOHub() = 0;

	virtual void SetScreenSizeChangeListener(const ScreenSizeChangeListenerPtr& listener);

protected:
	ScreenSizeChangeListenerWeakPtr m_screenSizeChangeListener;
};

typedef boost::shared_ptr<Application> ApplicationPtr;
typedef boost::weak_ptr<Application> ApplicationWeakPtr;

/**
 * \brief Abstracts all window related methods
 *
 * This class contains methods that will handle window
 * tasks, such as positioning and title.
 */
class Window
{
public:
	virtual void EnableQuitShortcuts(const bool enable) = 0;
	virtual bool QuitShortcutsEnabled() = 0;

	virtual bool SetWindowTitle(const str_type::string& title) = 0;
	virtual str_type::string GetWindowTitle() const = 0;

	virtual void EnableMediaPlaying(const bool enable) = 0;

	virtual bool IsWindowed() const = 0;

	virtual math::Vector2i GetScreenSize() const = 0;
	virtual math::Vector2 GetScreenSizeF() const = 0;

	virtual math::Vector2i GetWindowPosition() = 0;
	virtual void SetWindowPosition(const math::Vector2i &v2) = 0;

	virtual math::Vector2i ScreenToWindow(const math::Vector2i &v2Point) const = 0;

	virtual bool WindowVisible() const = 0;
	virtual bool WindowInFocus() const = 0;

	virtual bool HideCursor(const bool hide) = 0;
	virtual bool IsCursorHidden() const = 0;
};

typedef boost::shared_ptr<Window> WindowPtr;
typedef boost::weak_ptr<Window> WindowWeakPtr;

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

	/// Loads the texture from a file in virtual memory (supported formats: .dds, .bmp, .png, .jpg/jpeg).
	virtual TexturePtr CreateTextureFromFileInMemory(const void *pBuffer, const unsigned int bufferLength, GS_COLOR mask,
				 const unsigned int width = 0, const unsigned int height = 0,
				 const unsigned int nMipMaps = 0) = 0;

	/// Loads the texture from a file in a hard disk (supported formats: .dds, .bmp, .png, .jpg/jpeg).
	virtual TexturePtr LoadTextureFromFile(const str_type::string& fileName, GS_COLOR mask,
				 const unsigned int width = 0, const unsigned int height = 0,
				 const unsigned int nMipMaps = 0) = 0;

	/// Creates a texture as render target.
	virtual TexturePtr CreateRenderTargetTexture(const unsigned int width, const unsigned int height, const GS_TARGET_FORMAT fmt) = 0;

	/// Creates a sprite from a texture in virtual memory
	virtual SpritePtr CreateSprite(GS_BYTE *pBuffer, const unsigned int bufferLength,
						GS_COLOR mask = GS_ZERO, const unsigned int width = 0, const unsigned int height = 0) = 0;

	/// Creates a sprite from a texture in a file
	virtual SpritePtr CreateSprite(const str_type::string& fileName, GS_COLOR mask = GS_ZERO, const unsigned int width = 0, const unsigned int height = 0) = 0;

	/// Creates a sprite as render target
	virtual SpritePtr CreateRenderTarget(const unsigned int width, const unsigned int height,
							const GS_TARGET_FORMAT format = GSTF_DEFAULT) = 0;

	/// Create a shader object and load/compile it.
	virtual ShaderPtr LoadShaderFromFile(const str_type::string& fileName, const GS_SHADER_FOCUS focus,
				const GS_SHADER_PROFILE profile = GSSP_HIGHEST, const char *entry = 0) = 0;

	/// Create a shader object and load/compile it.
	virtual ShaderPtr LoadShaderFromString(const str_type::string& shaderName, const std::string& codeAsciiString, const GS_SHADER_FOCUS focus,
				const GS_SHADER_PROFILE profile = GSSP_HIGHEST, const char *entry = 0) = 0;

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
	virtual bool ResetVideoMode(const VIDEO_MODE& mode, const bool toggleFullscreen = false) = 0;
	virtual bool ResetVideoMode(const unsigned int width, const unsigned int height, const GS_PIXEL_FORMAT pfBB, const bool toggleFullscreen = false) = 0;

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
	virtual unsigned int FindClosestCarretPosition(const str_type::string& font, const str_type::string &text, const math::Vector2 &textPos, const math::Vector2 &reference) = 0;
	virtual bool DrawBitmapText(const math::Vector2 &v2Pos, const str_type::string& text, const str_type::string& font, const GS_COLOR& color, const float scale = 1.0f) = 0;
	virtual bool DrawLine(const math::Vector2 &p1, const math::Vector2 &p2, const GS_COLOR& color1, const GS_COLOR& color2) = 0;
	virtual bool DrawRectangle(const math::Vector2 &v2Pos, const math::Vector2 &v2Size,
						const GS_COLOR& color, const float angle = 0.0f, const GS_ENTITY_ORIGIN origin = GSEO_DEFAULT) = 0;
	virtual bool DrawRectangle(const math::Vector2 &v2Pos, const math::Vector2 &v2Size,
						const GS_COLOR& color0, const GS_COLOR& color1, const GS_COLOR& color2, const GS_COLOR& color3,
						const float angle = 0.0f, const GS_ENTITY_ORIGIN origin = GSEO_DEFAULT) = 0;

	virtual void SetBGColor(const GS_COLOR& backgroundColor) = 0;
	virtual GS_COLOR GetBGColor() const = 0;

	virtual bool BeginSpriteScene(const GS_COLOR& dwBGColor = GS_ZERO) = 0;
	virtual bool EndSpriteScene() = 0;
	virtual bool BeginTargetScene(const GS_COLOR& dwBGColor = GS_ZERO, const bool clear = true) = 0;
	virtual bool EndTargetScene() = 0;

	virtual bool SetAlphaMode(const GS_ALPHA_MODE mode) = 0;
	virtual GS_ALPHA_MODE GetAlphaMode() const = 0;

	virtual bool SetFilterMode(const GS_TEXTUREFILTER_MODE tfm) = 0;
	virtual GS_TEXTUREFILTER_MODE GetFilterMode() const = 0;

	virtual bool Rendering() const = 0;

	virtual bool SaveScreenshot(const wchar_t *wcsName, const GS_BITMAP_FORMAT fmt = GSBF_BMP,
								math::Rect2D rect = math::Rect2D(0,0,0,0)) = 0;

private:
	virtual bool StartApplication(const unsigned int width, const unsigned int height,
			const str_type::string& winTitle, const bool windowed,
			const bool sync, const GS_PIXEL_FORMAT pfBB = GSPF_UNKNOWN,
			const bool maximizable = false) = 0;
};

typedef boost::shared_ptr<Video> VideoPtr;
typedef boost::shared_ptr<const Video> VideoConstPtr;
typedef boost::weak_ptr<Video> VideoWeakPtr;

/**
 * \brief Stores a texture buffer to be used as source or target
 */
class Texture
{
public:
	struct PROFILE
	{
		PROFILE()
		{
			mask = 0xFF000000;
			width = height = nMipMaps = originalWidth = originalHeight = 0;
		}
		GS_COLOR mask;
		unsigned int width, height, nMipMaps,
			originalWidth, originalHeight;
	};

	enum TYPE
	{
		TT_STATIC = 0,
		TT_RENDER_TARGET = 1,
		TT_NONE = 2,
	};

	static const unsigned int FULL_MIPMAP_CHAIN = 0x00FFFFFF;

	virtual bool SetTexture(const unsigned int passIdx = 0) = 0;
	virtual PROFILE GetProfile() const = 0;
	virtual TYPE GetTextureType() const = 0;
	virtual boost::any GetTextureObject() = 0;
	virtual math::Vector2 GetBitmapSize() const = 0;

	virtual bool CreateRenderTarget(
		VideoWeakPtr video, const unsigned int width, const unsigned int height, const GS_TARGET_FORMAT fmt
	) = 0;
	virtual bool LoadTexture(
		VideoWeakPtr video,
		const str_type::string& fileName, GS_COLOR mask,
			const unsigned int width = 0, const unsigned int height = 0,
			const unsigned int nMipMaps = 0
	) = 0;
	virtual bool LoadTexture(
		VideoWeakPtr video,
		const void * pBuffer, GS_COLOR mask,
			const unsigned int width, const unsigned int height,
			const unsigned int nMipMaps,
			const unsigned int bufferLength
	) = 0;
};

/**
 * \brief Draws and manages sprites
 *
 * This class encapsulates all the math and API calls behind
 * sprite drawing. It stores a bitmap buffer and is capable of rendering
 * it as a sprite as many times as necessary and at different places
 * and shapes.
 */
class Sprite
{
public:
	enum TYPE
	{
		T_NOT_LOADED = 0,
		T_BITMAP = 1,
		T_TARGET = 2,
		T_RELOAD = 3,
	};

	virtual bool LoadSprite(VideoWeakPtr video, GS_BYTE *pBuffer, const unsigned int bufferLength,
						GS_COLOR mask = GS_ZERO, const unsigned int width = 0, const unsigned int height = 0) = 0;
	virtual bool LoadSprite(VideoWeakPtr video, const str_type::string& fileName, GS_COLOR mask = GS_ZERO,
					const unsigned int width = 0, const unsigned int height = 0) = 0;
	virtual bool CreateRenderTarget(VideoWeakPtr video, const unsigned int width, const unsigned int height,
							const GS_TARGET_FORMAT format = GSTF_DEFAULT) = 0;

	virtual bool Draw(const math::Vector2 &v2Pos,
					const GS_COLOR& color = GS_WHITE,
					const float angle = 0.0f,
					const math::Vector2 &v2Scale = math::Vector2(1.0f,1.0f)) = 0;
	virtual bool DrawShaped(const math::Vector2 &v2Pos, const math::Vector2 &v2Size,
						  const GS_COLOR& color0, const GS_COLOR& color1,
						  const GS_COLOR& color2, const GS_COLOR& color3,
						  const float angle = 0.0f) = 0;
	virtual bool Stretch(const math::Vector2 &a, const math::Vector2 &b, const float width,
					   const GS_COLOR& color0 = GS_WHITE, const GS_COLOR& color1 = GS_WHITE) = 0;
	virtual bool SaveBitmap(const wchar_t *wcsName, const GS_BITMAP_FORMAT fmt, math::Rect2D *pRect = 0) = 0;

	virtual bool DrawShapedFast(const math::Vector2 &v2Pos, const math::Vector2 &v2Size, const GS_COLOR& color) = 0;
	virtual bool DrawOptimal(const math::Vector2 &v2Pos, const GS_COLOR& color = GS_WHITE, const float angle = 0.0f,
					 const math::Vector2 &v2Size = math::Vector2(-1,-1)) = 0;
	virtual void BeginFastRendering() = 0;
	virtual void EndFastRendering() = 0;

	virtual TextureWeakPtr GetTexture() = 0;
	virtual void SetOrigin(const GS_ENTITY_ORIGIN origin) = 0;
	virtual void SetOrigin(const math::Vector2 &v2Custom) = 0;
	virtual math::Vector2 GetOrigin() const = 0;

	virtual bool SetupSpriteRects(const unsigned int columns, const unsigned int rows) = 0;
	virtual bool SetRect(const unsigned int column, const unsigned int row) = 0;
	virtual bool SetRect(const unsigned int rect) = 0;
	virtual void SetRect(const math::Rect2Df &rect) = 0;
	virtual void UnsetRect() = 0;
	virtual unsigned int GetNumRects() const = 0;
	virtual math::Rect2Df GetRect() const = 0;
	virtual math::Rect2Df GetRect(const unsigned int rect) const = 0;
	virtual unsigned int GetRectIndex() const = 0;

	virtual Texture::PROFILE GetProfile() const = 0;
	virtual math::Vector2i GetBitmapSize() const = 0;
	virtual math::Vector2 GetBitmapSizeF() const = 0;

	virtual math::Vector2 GetFrameSize() const = 0;

	virtual unsigned int GetNumRows() const = 0;
	virtual unsigned int GetNumColumns() const = 0;

	virtual void FlipX(const bool flip) = 0;
	virtual void FlipY(const bool flip) = 0;
	virtual void FlipX() = 0;
	virtual void FlipY() = 0;
	virtual bool GetFlipX() const = 0;
	virtual bool GetFlipY() const = 0;

	virtual void SetScroll(const math::Vector2 &v2Scroll) = 0;
	virtual math::Vector2 GetScroll() const = 0;

	virtual void SetMultiply(const math::Vector2 &v2Multiply) = 0;
	virtual math::Vector2 GetMultiply() const = 0;

	virtual TYPE GetType() const = 0;
	virtual boost::any GetTextureObject() = 0;

	virtual void GenerateBackup() = 0;
	virtual bool SetAsTexture(const unsigned int passIdx) = 0;
	virtual void SetRectMode(const GS_RECT_MODE mode) = 0;
	virtual GS_RECT_MODE GetRectMode() const = 0;

	virtual void SetSpriteDensityValue(const float value) = 0;
	virtual float GetSpriteDensityValue() const = 0;

	/// Used on API's that must handle lost devices
	virtual void OnLostDevice() = 0;

	/// Used on API's that must handle lost devices
	virtual void RecoverFromBackup() = 0;
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

		CHAR_DESCRIPTOR() : x( 0 ), y( 0 ), width( 0 ), height( 0 ), xOffset( 0 ), yOffset( 0 ),
			xAdvance( 0 ), page( 0 )
		{ }
	};

	struct CHARSET
	{
		CHARSET() : paddingDown( 0 ), paddingUp( 0 ), paddingLeft( 0 ), paddingRight( 0 ) {	}
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
	BitmapFont(VideoWeakPtr video, const str_type::string& fileName, const str_type::string& str);
	math::Vector2 DrawBitmapText(const math::Vector2 &pos, const str_type::string& text, const GS_COLOR& color, const float scale = 1.0f);
	math::Vector2 ComputeTextBoxSize(const str_type::string &text);
	math::Vector2 ComputeCarretPosition(const str_type::string &text, const unsigned int pos);
	unsigned int FindClosestCarretPosition(const str_type::string &text, const math::Vector2 &textPos, const math::Vector2 &reference);
};

typedef boost::shared_ptr<BitmapFont> BitmapFontPtr;

/// When called once per frame, returns the last frame time delta
GS2D_API unsigned long ComputeElapsedTime(ApplicationPtr app);
GS2D_API float ComputeElapsedTimeF(ApplicationPtr app);

/// Instantiate a Video object (must be defined in the API specific code)
GS2D_API VideoPtr CreateVideo(const unsigned int width, const unsigned int height,
				const str_type::string& winTitle, const bool windowed, const bool sync,
				const Platform::FileIOHubPtr& fileIOHub, const GS_PIXEL_FORMAT pfBB = GSPF_UNKNOWN, const bool maximizable = false);

#if defined(ANDROID) || defined(APPLE_IOS)
/// Instantiate a Video object (must be defined in the API specific code)
GS2D_API VideoPtr CreateVideo(const unsigned int width, const unsigned int height, const Platform::FileIOHubPtr& fileIOHub);
#endif

} // namespace gs2d

#endif