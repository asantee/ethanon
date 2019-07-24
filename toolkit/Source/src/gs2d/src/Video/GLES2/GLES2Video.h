#ifndef GS2D_GLES2_H_
#define GS2D_GLES2_H_

#include "../../Video.h"

#include "../../Platform/Platform.h"
#include "../../Platform/FileLogger.h"

#include "../../Math/Vector4.h"

#include "GLES2Texture.h"

#include "GLES2PolygonRenderer.h"

#include "../../Platform/FileIOHub.h"

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
		Color mask,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0);

	TexturePtr LoadTextureFromFile(
		const str_type::string& fileName,
		Color mask,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0);

	TexturePtr CreateRenderTargetTexture(
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT fmt);

	SpritePtr CreateSprite(
		unsigned char *pBuffer,
		const unsigned int bufferLength,
		Color mask = math::constant::ZERO_VECTOR4,
		const unsigned int width = 0,
		const unsigned int height = 0);

	SpritePtr CreateSprite(
		const str_type::string& fileName,
		Color mask = math::constant::ZERO_VECTOR4,
		const unsigned int width = 0,
		const unsigned int height = 0);

	SpritePtr CreateRenderTarget(
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT format = Texture::TF_DEFAULT);

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

	bool SetRenderTarget(SpritePtr pTarget, const unsigned int target = 0);
	unsigned int GetMaxRenderTargets() const;
	unsigned int GetMaxMultiTextures() const;
	bool UnsetTexture(const unsigned int passIdx);

	void SetZBuffer(const bool enable);
	bool GetZBuffer() const;

	void SetZWrite(const bool enable);
	bool GetZWrite() const;

	bool SetClamp(const bool set);
	bool GetClamp() const;

	bool SetSpriteDepth(const float depth);
	float GetSpriteDepth() const;

	bool SetScissor(const bool& enable);
	bool SetScissor(const math::Rect2Di& rect);
	math::Rect2Di GetScissor() const;
	void UnsetScissor();

	bool DrawLine(
		const math::Vector2& p1,
		const math::Vector2& p2,
		const Color& color1,
		const Color& color2);

	bool DrawRectangle(
		const math::Vector2& v2Pos,
		const math::Vector2& v2Size,
		const Color& color,
		const float angle = 0.0f,
		const Sprite::ENTITY_ORIGIN origin = Sprite::EO_DEFAULT);

	bool DrawRectangle(
		const math::Vector2& v2Pos,
		const math::Vector2& v2Size,
		const Color& color0,
		const Color& color1,
		const Color& color2,
		const Color& color3,
		const float angle = 0.0f,
		const Sprite::ENTITY_ORIGIN origin = Sprite::EO_DEFAULT);

	void SetBGColor(const Color& backgroundColor);
	Color GetBGColor() const;

	bool BeginSpriteScene(const Color& color = math::constant::ZERO_VECTOR4);
	bool EndSpriteScene();
	bool BeginTargetScene(const Color& color = math::constant::ZERO_VECTOR4, const bool clear = true);
	bool EndTargetScene();

	bool SetAlphaMode(const Video::ALPHA_MODE mode);
	ALPHA_MODE GetAlphaMode() const;

	bool SetFilterMode(const Video::TEXTUREFILTER_MODE tfm);
	Video::TEXTUREFILTER_MODE GetFilterMode() const;

	bool Rendering() const;

	bool SaveScreenshot(
		const str_type::char_t* name,
		const Texture::BITMAP_FORMAT fmt = Texture::BF_BMP,
		math::Rect2Di rect = math::Rect2Di(0, 0, 0, 0));
	
	math::Vector2i GetClientScreenSize() const;
	APP_STATUS HandleEvents();
	float GetFPSRate() const;
	void Message(const str_type::string& text, const GS_MESSAGE_TYPE type = GSMT_ERROR) const;
	void Quit();
	void EnableQuitShortcuts(const bool enable);
	bool QuitShortcutsEnabled();
	bool SetWindowTitle(const str_type::string& title);
	str_type::string GetWindowTitle() const;
	void EnableMediaPlaying(const bool enable);
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
	math::Vector2 GetCurrentTargetSize() const;

	Color m_backgroundColor;
	Video::ALPHA_MODE m_alphaMode;

	math::Vector2i m_screenSize;
	str_type::string m_externalStoragePath;
	str_type::string m_globalExternalStoragePath;
	str_type::string m_windowTitle;
	math::Rect2Di m_scissor;
	bool m_quit;
	bool m_rendering;
	bool m_zWrite;
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

	TextureWeakPtr m_currentTarget;

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
