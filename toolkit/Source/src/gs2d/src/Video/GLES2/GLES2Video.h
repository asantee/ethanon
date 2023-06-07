#ifndef GS2D_GLES2_H_
#define GS2D_GLES2_H_

#include "../../Video.h"

#include "../../Platform/Platform.h"
#include "../../Platform/NativeCommandForwarder.h"

#include "../../Math/Vector4.h"

#include "GLES2Texture.h"

#include "../../Platform/FileIOHub.h"

namespace gs2d {

class GLES2Video : public Video, public Platform::NativeCommandForwarder
{
	friend class IOSGLES2Video;
	friend class AndroidGLES2Video;

	boost::weak_ptr<GLES2Video> weak_this;

	// TO-DO protect this
	float m_previousTime;
	unsigned long m_frameCount;

	// TO-DO protect this
	double m_startTime;

public:
	static bool CheckGLError(const std::string& op);

	GLES2Video(
		const unsigned int width,
		const unsigned int height,
		const std::string& winTitle,
		const Platform::FileIOHubPtr& fileIOHub);

	PolygonRendererPtr CreatePolygonRenderer(
			const std::vector<PolygonRenderer::Vertex>& vertices,
			const std::vector<uint32_t>& indices,
			const PolygonRenderer::POLYGON_MODE mode) override;
	
	TexturePtr CreateTextureFromFileInMemory(
		const void *pBuffer,
		const unsigned int bufferLength,
		const unsigned int nMipMaps = 0) override;

	TexturePtr LoadTextureFromFile(
		const std::string& fileName,
		const unsigned int nMipMaps = 0) override;

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

	VIDEO_MODE GetVideoMode(const unsigned int modeIdx) const override;
	unsigned int GetVideoModeCount() const override;

	bool ResetVideoMode(
		const VIDEO_MODE& mode,
		const bool toggleFullscreen = false) override;

	bool ResetVideoMode(
		const unsigned int width,
		const unsigned int height,
		const Texture::PIXEL_FORMAT pfBB,
		const bool toggleFullscreen = false) override;

	void SetZBuffer(const bool enable) override;
	bool GetZBuffer() const override;

	void SetBackgroundColor(const Color& backgroundColor) override;
	Color GetBackgroundColor() const override;

	bool BeginRendering(const Color& color = math::constant::ZERO_VECTOR4) override;
	bool EndRendering() override;

	bool SetAlphaMode(const Video::ALPHA_MODE mode) override;
	ALPHA_MODE GetAlphaMode() const override;
	
	math::Vector2i GetClientScreenSize() const override;
	APP_STATUS HandleEvents() override;
	float GetFPSRate() const override;
	void Message(const std::string& text, const GS_MESSAGE_TYPE type = GSMT_ERROR) const override;
	void Quit() override;
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

	void ForwardCommand(const std::string& cmd) override;
	std::string PullCommands() override;

	bool IsTrue(const GLboolean& enabled);
	void SetBlend(const bool enable);
	
	float GetElapsedTimeF(const TIME_UNITY unity) const override;
	unsigned long GetElapsedTime(const TIME_UNITY unity) const override;

protected:

	Color m_backgroundColor;
	Video::ALPHA_MODE m_alphaMode;

	math::Vector2i m_screenSize;
	std::string m_externalStoragePath;
	std::string m_globalExternalStoragePath;
	std::string m_windowTitle;
	bool m_quit;
	bool m_zBuffer;
	bool m_blend;

	float m_fpsRate;

	Platform::FileIOHubPtr m_fileIOHub;

	void ComputeFPSRate();

	void Enable2D(const int width, const int height, const bool flipY = false);

	Platform::FileIOHubPtr GetFileIOHub() override;
};

} // namespace gs2d

#endif
