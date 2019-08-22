#ifndef GS2D_GL_SDL_VIDEO_H_
#define GS2D_GL_SDL_VIDEO_H_

#include "../../Video.h"
#include <SDL2/SDL.h>

namespace gs2d {

class GLSDLVideo : public Video
{
	friend class SDLInput;

	boost::weak_ptr<GLSDLVideo> weak_this;

	static float m_mouseWheel;
	static str_type::string m_lastCharInput;

    Platform::FileIOHubPtr m_fileIOHub;

	SDL_Window* m_window;
	SDL_GLContext m_glcontext;

    math::Vector2 m_screenSize;
    bool m_maximizable, m_sync, m_quit;
    bool m_windowHasFocus, m_windowIsVisible;
    std::vector<VIDEO_MODE> m_videoModes;
    float m_fpsRate;
    double m_startTime;
    ALPHA_MODE m_alphaMode;
    const double m_alphaRef;
    const float m_zNear, m_zFar;
    Color m_backgroundColor;

    void Enable2DStates();
    void ReadDisplayModes();
	Uint32 AssembleFlags(const bool windowed, const bool maximizable, const bool sync);

public:
	static boost::shared_ptr<GLSDLVideo> Create(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const bool windowed,
		const bool sync,
		const Platform::FileIOHubPtr& fileIOHub,
		const Texture::PIXEL_FORMAT pfBB,
		const bool maximizable);

	GLSDLVideo(
		Platform::FileIOHubPtr fileIOHub,
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const bool windowed,
		const bool sync,
		const bool maximizable);
	
	~GLSDLVideo();

    bool ResetVideoMode(
        const unsigned int width,
        const unsigned int height,
        const Texture::PIXEL_FORMAT pfBB,
        const bool toggleFullscreen,
        const bool forceWindowResize);

    bool StartApplication(
        const unsigned int width,
        const unsigned int height,
        const str_type::string& winTitle,
        const bool windowed,
        const bool sync,
        const Texture::PIXEL_FORMAT pfBB = Texture::PF_UNKNOWN,
        const bool maximizable = false) override;

	TexturePtr CreateTextureFromFileInMemory(
		const void *pBuffer,
		const unsigned int bufferLength,
		const unsigned int nMipMaps) override;
	
	TexturePtr LoadTextureFromFile(
		const str_type::string& fileName,
		const unsigned int nMipMaps) override;

	ShaderPtr LoadShaderFromFile(
	    const std::string& vsFileName,
        const std::string& vsEntry,
        const std::string& psFileName,
        const std::string& psEntry) override;
	
	ShaderPtr LoadShaderFromString(
		const std::string& vsShaderName,
        const std::string& vsCodeAsciiString,
        const std::string& vsEntry,
        const std::string& psShaderName,
        const std::string& psCodeAsciiString,
        const std::string& psEntry) override;

	bool BeginRendering(const Color& bgColor = math::constant::ZERO_VECTOR4) override;
	bool EndRendering() override;
	
	bool ResetVideoMode(
		const VIDEO_MODE& mode,
		const bool toggleFullscreen = false) override;

	bool ResetVideoMode(
		const unsigned int width,
		const unsigned int height,
		const Texture::PIXEL_FORMAT pfBB,
		const bool toggleFullscreen = false) override;
	
	math::Vector2 CatchBestScreenResolution() const;

	bool SetAlphaMode(const ALPHA_MODE mode) override;
	ALPHA_MODE GetAlphaMode() const override;

	void SetZBuffer(const bool enable) override;
	bool GetZBuffer() const override;
	
	void SetBGColor(const Color& backgroundColor) override;
	Color GetBGColor() const override;

	// Application implementations
	math::Vector2i GetClientScreenSize() const override;

	APP_STATUS HandleEvents() override;
	float GetFPSRate() const override;
	
	void Message(const str_type::string& text, const GS_MESSAGE_TYPE type = GSMT_ERROR) const override;

	unsigned long GetElapsedTime(const TIME_UNITY unity = TU_MILLISECONDS) const override;

	float GetElapsedTimeF(const TIME_UNITY unity = TU_MILLISECONDS) const override;

	void ForwardCommand(const str_type::string& cmd) override;
	str_type::string PullCommands() override;

	void Quit() override;

	str_type::string GetPlatformName() const;

	Platform::FileIOHubPtr GetFileIOHub() override;
	
	// Window implementations
	void EnableQuitShortcuts(const bool enable) override;
	bool QuitShortcutsEnabled() override;

	bool SetWindowTitle(const str_type::string& title) override;
	str_type::string GetWindowTitle() const override;

	bool IsWindowed() const override;

	math::Vector2i GetScreenSize() const override;
	math::Vector2 GetScreenSizeF() const override;
	math::Vector2 GetScreenSizeInPixels() const override;

	math::Vector2i GetWindowPosition() override;
	void SetWindowPosition(const math::Vector2i &v2) override;

	math::Vector2i ScreenToWindow(const math::Vector2i &v2Point) const override;

	bool WindowVisible() const override;
	bool WindowInFocus() const override;
	
	bool HideCursor(const bool hide) override;
	bool IsCursorHidden() const override;

	VIDEO_MODE GetVideoMode(const unsigned int modeIdx) const override;
	unsigned int GetVideoModeCount() const override;

	bool SyncEnabled() const;
	bool IsMaximizable() const;
	double GetElapsedTimeD(const TIME_UNITY unity) const;
	void ComputeFPSRate();};
} // namespace gs2d

#endif
