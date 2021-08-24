#ifndef GS2D_METAL_VIDEO_H_
#define GS2D_METAL_VIDEO_H_

#include "../../Video.h"
#include "../../Platform/Platform.h"
#include "../../Platform/NativeCommandForwarder.h"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

namespace gs2d {

class MetalVideo : public Video, public Platform::NativeCommandForwarder
{
	boost::weak_ptr<MetalVideo> weak_this;

	id<MTLDevice> m_device;
	MTKView* m_view;
	
	Platform::FileIOHubPtr m_fileIOHub;
	bool m_quit;
	double m_startTime;
	float m_fpsRate;
	math::Vector2 m_screenSizeInPixels;

	void ComputeFPSRate();

	bool StartApplication(
		const unsigned int width,
		const unsigned int height,
		const std::string& winTitle,
		const bool windowed,
		const bool sync,
		const Texture::PIXEL_FORMAT pfBB = Texture::PF_UNKNOWN,
		const bool maximizable = false) override;

	static boost::shared_ptr<MetalVideo> Create(
		const unsigned int width,
		const unsigned int height,
		const std::string& winTitle,
		const bool windowed,
		const bool sync,
		const Platform::FileIOHubPtr& fileIOHub,
		const Texture::PIXEL_FORMAT pfBB,
		const bool maximizable);

public:
	
	id<MTLDevice> GetDevice();

	static boost::shared_ptr<MetalVideo> Create(const Platform::FileIOHubPtr& fileIOHub, MTKView* view);

	MetalVideo(Platform::FileIOHubPtr fileIOHub, MTKView* view);

	TexturePtr CreateTextureFromFileInMemory(
		const void *pBuffer,
		const unsigned int bufferLength,
		const unsigned int nMipMaps) override;
	
	TexturePtr LoadTextureFromFile(
		const std::string& fileName,
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
	
	void Message(const std::string& text, const GS_MESSAGE_TYPE type = GSMT_ERROR) const override;

	unsigned long GetElapsedTime(const TIME_UNITY unity = TU_MILLISECONDS) const override;

	float GetElapsedTimeF(const TIME_UNITY unity = TU_MILLISECONDS) const override;

	void ForwardCommand(const std::string& cmd) override;
	std::string PullCommands() override;

	void Quit() override;

	Platform::FileIOHubPtr GetFileIOHub() override;
	
	// Window implementations
	void EnableQuitShortcuts(const bool enable) override;
	bool QuitShortcutsEnabled() override;

	bool SetWindowTitle(const std::string& title) override;
	std::string GetWindowTitle() const override;

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
	
	double GetElapsedTimeD(const TIME_UNITY unity = TU_MILLISECONDS) const;
};

VideoPtr CreateVideo(const Platform::FileIOHubPtr& fileIOHub, MTKView* view);

} // namespace gs2d

#endif
