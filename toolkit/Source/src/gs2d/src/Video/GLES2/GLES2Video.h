#ifndef GS2D_GLES2_H_
#define GS2D_GLES2_H_

#include "../../Video.h"

#include "../../Platform/Platform.h"
#include "../../Platform/FileLogger.h"

#include "../../Math/Vector4.h"

#include "GLES2Texture.h"

#include "GLES2PolygonRenderer.h"

#include "../../Platform/FileIOHub.h"

#ifdef APPLE_IOS
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>
#endif

#ifdef ANDROID
  #include <GLES2/gl2.h>
  #include <GLES2/gl2ext.h>
#endif

namespace gs2d {

class GLES2Shader;
typedef boost::shared_ptr<GLES2Shader> GLES2ShaderPtr;
class GLES2ShaderContext;
typedef boost::shared_ptr<GLES2ShaderContext> GLES2ShaderContextPtr;

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
	static const float ZNEAR;
	static const float ZFAR;
	static const str_type::string VIDEO_LOG_FILE;
	static const unsigned long ALPHAREF;

	static GLES2PolygonRendererPtr m_polygonRenderer;

	static bool CheckGLError(const str_type::string& op, const Platform::FileLogger& logger);

	TexturePtr CreateTextureFromFileInMemory(
		const void *pBuffer,
		const unsigned int bufferLength,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0);

	TexturePtr LoadTextureFromFile(
		const str_type::string& fileName,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0);

	SpritePtr CreateSprite(
		unsigned char *pBuffer,
		const unsigned int bufferLength,
		const unsigned int width = 0,
		const unsigned int height = 0);

	SpritePtr CreateSprite(
		const str_type::string& fileName,
		const unsigned int width = 0,
		const unsigned int height = 0);

	ShaderPtr LoadShaderFromFile(
		const std::string& vsFileName,
		const std::string& vsEntry,
		const std::string& psFileName,
		const std::string& psEntry);

	ShaderPtr LoadShaderFromString(
		const std::string& vsShaderName,
		const std::string& vsCodeAsciiString,
		const std::string& vsEntry,
		const std::string& psShaderName,
		const std::string& psCodeAsciiString,
		const std::string& psEntry);

	GLES2ShaderPtr LoadGLES2ShaderFromFile(
		const std::string& vsFileName,
		const std::string& psFileName);

	GLES2ShaderPtr LoadGLES2ShaderFromString(
		const std::string& vsShaderName,
		const std::string& vsCodeAsciiString,
		const std::string& psShaderName,
		const std::string& psCodeAsciiString);

	boost::any GetVideoInfo();

	ShaderPtr GetDefaultShader();
	ShaderPtr GetOptimalShader();
	ShaderPtr GetRectShader();
	ShaderPtr GetFastShader();
	ShaderPtr GetModulateShader();
	ShaderPtr GetAddShader();
	ShaderPtr GetCurrentShader();

	GLES2ShaderPtr GetDefaultPS();

	ShaderContextPtr GetShaderContext();
	bool SetCurrentShader(ShaderPtr shader);

	boost::any GetGraphicContext();

	VIDEO_MODE GetVideoMode(const unsigned int modeIdx) const;
	unsigned int GetVideoModeCount() const;

	bool ResetVideoMode(
		const VIDEO_MODE& mode,
		const bool toggleFullscreen = false);

	bool ResetVideoMode(
		const unsigned int width,
		const unsigned int height,
		const Texture::PIXEL_FORMAT pfBB,
		const bool toggleFullscreen = false);

	void SetZBuffer(const bool enable);
	bool GetZBuffer() const;

	bool SetSpriteDepth(const float depth);
	float GetSpriteDepth() const;

	void SetBGColor(const Color& backgroundColor);
	Color GetBGColor() const;

	bool BeginRendering(const Color& color = math::constant::ZERO_VECTOR4);
	bool EndRendering();

	bool SetAlphaMode(const Video::ALPHA_MODE mode);
	ALPHA_MODE GetAlphaMode() const;
	
	math::Vector2i GetClientScreenSize() const;
	APP_STATUS HandleEvents();
	float GetFPSRate() const;
	void Message(const str_type::string& text, const GS_MESSAGE_TYPE type = GSMT_ERROR) const;
	void Quit();
	void EnableQuitShortcuts(const bool enable);
	bool QuitShortcutsEnabled();
	bool SetWindowTitle(const str_type::string& title);
	str_type::string GetWindowTitle() const;
	bool IsWindowed() const;
	math::Vector2i GetScreenSize() const;
	math::Vector2 GetScreenSizeF() const;
	math::Vector2 GetScreenSizeInPixels() const;
	math::Vector2i GetWindowPosition();
	void SetWindowPosition(const math::Vector2i &v2);
	math::Vector2i ScreenToWindow(const math::Vector2i &v2Point) const;
	bool WindowVisible() const;
	bool WindowInFocus() const;
	bool HideCursor(const bool hide);
	bool IsCursorHidden() const;
	const Platform::FileLogger& GetLogger() const;

	void ForwardCommand(const str_type::string& cmd);
	str_type::string PullCommands();

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
	Platform::FileLogger m_logger;
	GLES2ShaderContextPtr m_shaderContext;

	float m_spriteDepth;

	GLES2ShaderPtr m_defaultShader;
	GLES2ShaderPtr m_optimalShader;
	GLES2ShaderPtr m_rectShader;
	GLES2ShaderPtr m_fastShader;
	GLES2ShaderPtr m_modulateShader;
	GLES2ShaderPtr m_addShader;

	ShaderPtr m_currentShader;

	float m_fpsRate;

	Video::TEXTUREFILTER_MODE m_textureFilterMode;

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
		const bool maximizable = false);

	Platform::FileIOHubPtr GetFileIOHub();
};
	
void UnbindFrameBuffer();

} // namespace gs2d

#endif
