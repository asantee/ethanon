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

	GLES2Video(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const Platform::FileIOHubPtr& fileIOHub);

	boost::weak_ptr<GLES2Video> weak_this;

	float m_previousTime;
	unsigned long m_frameCount;

public:
	static bool CheckGLError(const str_type::string& op);

	TexturePtr CreateTextureFromFileInMemory(
		const void *pBuffer,
		const unsigned int bufferLength,
		const unsigned int nMipMaps = 0) override;

	TexturePtr LoadTextureFromFile(
		const str_type::string& fileName,
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

	void SetBGColor(const Color& backgroundColor) override;
	Color GetBGColor() const override;

	bool BeginRendering(const Color& color = math::constant::ZERO_VECTOR4) override;
	bool EndRendering() override;

	bool SetAlphaMode(const Video::ALPHA_MODE mode) override;
	ALPHA_MODE GetAlphaMode() const override;
	
	math::Vector2i GetClientScreenSize() const override;
	APP_STATUS HandleEvents() override;
	float GetFPSRate() const override;
	void Message(const str_type::string& text, const GS_MESSAGE_TYPE type = GSMT_ERROR) const override;
	void Quit() override;
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

	void ForwardCommand(const str_type::string& cmd) override;
	str_type::string PullCommands() override;

	bool IsTrue(const GLboolean& enabled);
	void SetBlend(const bool enable);

protected:

	Color m_backgroundColor;
	Video::ALPHA_MODE m_alphaMode;

	math::Vector2i m_screenSize;
	str_type::string m_externalStoragePath;
	str_type::string m_globalExternalStoragePath;
	str_type::string m_windowTitle;
	bool m_quit;
	bool m_zBuffer;
	bool m_blend;

	float m_fpsRate;

	Platform::FileIOHubPtr m_fileIOHub;

	void ComputeFPSRate();

	void Enable2D(const int width, const int height, const bool flipY = false);

	bool StartApplication(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const bool windowed,
		const bool sync,
		const Texture::PIXEL_FORMAT pfBB = Texture::PF_UNKNOWN,
		const bool maximizable = false) override;

	Platform::FileIOHubPtr GetFileIOHub() override;
};

} // namespace gs2d

#endif
