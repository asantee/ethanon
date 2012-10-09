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

#ifndef GS2D_D3D9VIDEO_H_
#define GS2D_D3D9VIDEO_H_

#include "D3D9VideoInfo.h"
#include <vector>
#include <map>

namespace gs2d {

class D3D9Video : public Video
{
	friend class D3D9Sprite;
	friend class WinInput;

	D3D9Video(
		const unsigned int width,
		const unsigned int height,
		const std::wstring& winTitle,
		const bool windowed,
		const bool sync,
		const Texture::PIXEL_FORMAT pfBB,
		const bool maximizable,
		const Platform::FileIOHubPtr& fileIOHub);

	struct RENDER_TARGET
	{
		RENDER_TARGET();
		RENDER_TARGET(Sprite* sprite, const unsigned int width, const unsigned int height, const Texture::TARGET_FORMAT fmt);
		Sprite* pSprite;
		unsigned int width;
		unsigned int height;
		Texture::TARGET_FORMAT format;
	};

	typedef std::list<RENDER_TARGET> RENDER_TARGET_LIST;

	math::Vector2i m_screenDim;
	math::Vector2i m_windowPos;
	Color m_backgroundColor;
	float m_fpsRate;
	bool m_quit;
	std::vector<BLEND_MODE> m_blendModes;
	IDirect3DDevice9* m_pDevice;
	IDirect3D9* m_pD3D;
	math::Vector2i m_windowedDim;
	math::Vector2i m_topBarSize;

	D3D9VideoInfoPtr m_videoInfo;
	ALPHA_MODE m_alphaMode;

	bool m_rendering;
	bool m_windowed;
	bool m_sync;
	bool m_cursorHidden;
	bool m_clamp;
	bool m_maximizable;
	math::Rect2D m_scissor;
	math::Vector2 m_v2Camera;
	std::wstring m_windowTitle;
	float m_lineWidth;
	float m_depth;
	bool m_roundUpPosition;

	void ForwardCommand(const str_type::string& cmd);
	str_type::string PullCommands();

	std::vector<VIDEO_MODE> m_modes;
	unsigned int m_nVideoModes;
	TEXTUREFILTER_MODE m_textureFilter;
	D3D9CgShaderContextPtr m_shaderContext;
	ShaderPtr m_pCurrentPS,
			  m_pCurrentVS,
			  m_defaultVS,
			  m_rectVS,
			  m_fastVS;

	void ComputeFPSRate();
	void SetDisplayModes(IDirect3D9* pD3D);
	static LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static void RemoveFromTargetList(Sprite *pSprite);
	static void AddToTargetList(const RENDER_TARGET &target);

	boost::weak_ptr<D3D9Video> weak_this;

	Platform::FileIOHubPtr m_fileIOHub;

public:

	~D3D9Video();

	static const DWORD W32_WINDOWED_STYLE;
	static const DWORD W32_FULLSCREEN_STYLE;
	static const float ZNEAR;
	static const float ZFAR;
	static const DWORD ALPHAREF;
	static const unsigned int TEXTURE_CHANNELS;

	static boost::shared_ptr<D3D9Video> Create(
		const unsigned int width,
		const unsigned int height,
		const std::wstring& winTitle,
		const bool windowed,
		const bool sync,
		const Texture::PIXEL_FORMAT pfBB,
		const bool maximizable,
		const Platform::FileIOHubPtr& fileIOHub);

	TexturePtr CreateTextureFromFileInMemory(
		const void* pBuffer,
		const unsigned int bufferLength,
		Color mask,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0);

	TexturePtr LoadTextureFromFile(
		const std::wstring& fileName,
		Color mask,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0);

	TexturePtr CreateRenderTargetTexture(
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT = Texture::TF_DEFAULT);

	SpritePtr CreateSprite(
		GS_BYTE *pBuffer,
		const unsigned int bufferLength,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0);

	SpritePtr CreateSprite(
		const std::wstring& fileName,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0);

	SpritePtr CreateRenderTarget(
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT format = Texture::TF_DEFAULT);

	ShaderPtr LoadShaderFromFile(
		const std::wstring& fileName,
		const Shader::SHADER_FOCUS focus,
		const Shader::SHADER_PROFILE profile = Shader::SP_HIGHEST,
		const char *entry = 0);

	ShaderPtr LoadShaderFromString(
		const std::wstring& shaderName,
		const std::string& codeAsciiString,
		const Shader::SHADER_FOCUS focus,
		const Shader::SHADER_PROFILE profile = Shader::SP_HIGHEST,
		const char *entry = 0);

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

	boost::any GetVideoInfo();

	VIDEO_MODE GetVideoMode(const unsigned int modeIdx) const;
	unsigned int GetVideoModeCount();
	bool ResetVideoMode(const VIDEO_MODE& mode, const bool toggleFullscreen = false);

	bool ResetVideoMode(
		const unsigned int width,
		const unsigned int height,
		const Texture::PIXEL_FORMAT pfBB,
		const bool toggleFullscreen = false);

	bool SetRenderTarget(SpritePtr pTarget, const unsigned int target = 0);
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

	bool DrawLine(const math::Vector2 &p1,
		const math::Vector2 &p2,
		const Color& color1,
		const Color& color2);

	bool DrawRectangle(
		const math::Vector2& v2Pos,
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

	bool BeginSpriteScene(const Color& bgColor = constant::ZERO);
	bool EndSpriteScene();
	bool BeginTargetScene(const Color& bgColor = constant::ZERO, const bool clear = true);
	bool EndTargetScene();

	bool SetAlphaMode(const ALPHA_MODE mode);
	ALPHA_MODE GetAlphaMode() const;

	bool SetFilterMode(const TEXTUREFILTER_MODE tfm);
	TEXTUREFILTER_MODE GetFilterMode() const;

	bool Rendering() const;

	bool SaveScreenshot(const wchar_t *wcsName, const Texture::BITMAP_FORMAT fmt = Texture::BF_BMP,
						math::Rect2D rect = math::Rect2D(0,0,0,0));

	boost::any GetGraphicContext();

	// Application method implementations:
	bool ManageLoop();
	math::Vector2i GetClientScreenSize() const;
	APP_STATUS HandleEvents();
	float GetFPSRate() const;
	void Message(const std::wstring& text, const GS_MESSAGE_TYPE type = GSMT_ERROR) const;
	unsigned long GetElapsedTime(const TIME_UNITY unity = TU_MILLISECONDS) const;
	float GetElapsedTimeF(const TIME_UNITY unity = TU_MILLISECONDS) const;
	void ResetTimer();

	// Window method implementations:
	void EnableQuitShortcuts(const bool enable);
	bool QuitShortcutsEnabled();
	bool SetWindowTitle(const std::wstring& wcsTitle);
	std::wstring GetWindowTitle() const;
	void EnableMediaPlaying(const bool enable);
	bool IsWindowed() const;
	math::Vector2i GetScreenSize() const;
	math::Vector2 GetScreenSizeF() const;
	math::Vector2i GetWindowPosition();
	void SetWindowPosition(const math::Vector2i &v2);
	math::Vector2i ScreenToWindow(const math::Vector2i &v2Point) const;
	bool WindowVisible() const;
	bool WindowInFocus() const;
	bool HideCursor(const bool hide);
	bool IsCursorHidden() const;
	void Quit();

	str_type::string GetPlatformName() const;

	Platform::FileIOHubPtr GetFileIOHub();

private:

	bool StartApplication(
		const unsigned int width,
		const unsigned int height,
		const std::wstring& winTitle,
		const bool windowed,
		const bool sync,
		const Texture::PIXEL_FORMAT pfBB,
		const bool maximizable);

	bool BeginScene(const Color& bgColor = constant::ZERO, const bool clear = true);
	bool EndScene(const bool swap = true);

	static RENDER_TARGET_LIST m_targets;

	// these members must be static because their content must be shared with the Input object
	static SHORT m_wheelDelta;
	static bool m_inputFocus;
	static TCHAR m_currentChar;
};

} // namespace gs2d

#endif
