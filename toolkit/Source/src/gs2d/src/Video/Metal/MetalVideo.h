#ifndef GS2D_METAL_VIDEO_H_
#define GS2D_METAL_VIDEO_H_

#include "../../Video.h"
#include "../../Math/Color.h"
#include "../../Platform/Platform.h"
#include "../../Platform/NativeCommandForwarder.h"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

namespace gs2d {

class MetalVideo : public Video, public Platform::NativeCommandForwarder
{
	boost::weak_ptr<MetalVideo> weak_this;

	id<MTLDevice> m_device;
	id<MTLCommandQueue> m_commandQueue;
	id<MTLCommandBuffer> m_commandBuffer;
	id<MTLRenderCommandEncoder> m_commandEncoder;
	MTLRenderPassDescriptor* m_renderPassDescriptor;
	MTKView* m_view;
	
	Platform::FileIOHubPtr m_fileIOHub;
	bool m_quit;
	double m_startTime;
	float m_fpsRate;
	math::Vector2 m_screenSizeInPixels;
	gs2d::Color m_backgroundColor;

	void ComputeFPSRate();
	
	dispatch_semaphore_t m_inFlightSemaphore;
	uint8_t m_uniformBufferIndex;

public:
	MetalVideo(Platform::FileIOHubPtr fileIOHub, MTKView* view);

	id<MTLDevice> GetDevice();
	MTKView* GetView();
	id<MTLRenderCommandEncoder> GetRenderCommandEncoder();
	uint8_t GetUniformBufferIndex() const;

	PolygonRendererPtr CreatePolygonRenderer(
			const std::vector<PolygonRenderer::Vertex>& vertices,
			const std::vector<uint32_t>& indices,
			const PolygonRenderer::POLYGON_MODE mode) override;
	
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
	
	void SetBackgroundColor(const Color& backgroundColor) override;
	Color GetBackgroundColor() const override;

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

} // namespace gs2d

#endif
