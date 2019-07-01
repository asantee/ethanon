#include "GLES2Video.h"

#ifdef APPLE_IOS
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>
  #include "../../Platform/ios/Platform.ios.h"
#endif

#ifdef ANDROID
  #include <GLES2/gl2.h>
  #include <GLES2/gl2ext.h>
  #include "../../Platform/android/Platform.android.h"
#endif

#include "../../Shader.h"
#include "GLES2Shader.h"
#include "GLES2Sprite.h"

#include <sstream>

#include "../glslShaderCode.h"

namespace gs2d {

using namespace math;

const float GLES2Video::ZFAR = 5.0f;
const float GLES2Video::ZNEAR = 0.0f;
const str_type::string GLES2Video::VIDEO_LOG_FILE("GLES2Video.log.txt");
const unsigned long GLES2Video::ALPHAREF = (0x01);

void UnbindFrameBuffer()
{
	GLuint idx;
#	ifdef APPLE_IOS
		idx = 1;
#	else
		idx = 0;
#	endif
	glBindFramebuffer(GL_FRAMEBUFFER, idx);
}

GLES2Video::GLES2Video(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub) :
	m_backgroundColor(gs2d::constant::BLACK),
	m_screenSize(width, height),
	m_windowTitle(winTitle),
	m_quit(false),
	m_rendering(false),
	m_logger(Platform::FileLogger::GetLogDirectory() + VIDEO_LOG_FILE),
	m_fpsRate(30.0f),
	m_scissor(Vector2i(0, 0), Vector2i(0, 0)),
	m_textureFilterMode(Video::TM_IFNEEDED),
	m_blend(false),
	m_zBuffer(true),
	m_zWrite(true),
	m_fileIOHub(fileIOHub),
	m_frameCount(0),
	m_previousTime(0),
	m_spriteDepth(0.0f)
{
	m_logger.Log("Creating shader context...", Platform::FileLogger::INFO);
	m_shaderContext = GLES2ShaderContextPtr(new GLES2ShaderContext(this));
	m_logger.Log("StartApplication...", Platform::FileLogger::INFO);
	StartApplication(width, height, winTitle, false, false, Texture::PF_DEFAULT, false);
}

static bool HasFragmentShaderMaximumPrecision()
{
	GLint range[2], precision[1];
	glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, range, precision);
	return (range[0] != 0 && range[1] != 0 && precision[0] != 0);
}

static void LogFragmentShaderMaximumPrecision(const Platform::FileLogger& logger)
{
	const bool precisionResult = HasFragmentShaderMaximumPrecision();
	const str_type::string logStr = (precisionResult)
		? GS_L("High floating point fragment shader precision supported")
		: GS_L("High floating point fragment shader precision is not supported");
	logger.Log(logStr, (precisionResult) ? Platform::FileLogger::INFO : Platform::FileLogger::WARNING);
}

bool GLES2Video::StartApplication(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Texture::PIXEL_FORMAT pfBB,
	const bool maximizable)
{
	glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);

	// toggle dither
	if (gs2d::Application::SharedData.Get("ethanon.system.gles2dither") == "enable")
		glEnable(GL_DITHER);
	else
		glDisable(GL_DITHER);

	// create shaders
	m_defaultShader  = LoadGLES2ShaderFromFile("default.vs",    gs2dshaders::GLSL_default_default_vs,    "default.ps", gs2dshaders::GLSL_default_default_ps);
	m_rectShader     = LoadGLES2ShaderFromFile("default.vs",    gs2dshaders::GLSL_default_default_vs,    "default.ps", gs2dshaders::GLSL_default_default_ps);
	m_fastShader     = LoadGLES2ShaderFromFile("fastRender.vs", gs2dshaders::GLSL_default_fastRender_vs, "default.ps", gs2dshaders::GLSL_default_default_ps);
	m_modulateShader = LoadGLES2ShaderFromFile("default.vs",    gs2dshaders::GLSL_default_default_vs,    "modulate1",  gs2dshaders::GLSL_default_modulate1_ps);
	m_addShader      = LoadGLES2ShaderFromFile("default.vs",    gs2dshaders::GLSL_default_default_vs,    "add1",       gs2dshaders::GLSL_default_add1_ps);

	LogFragmentShaderMaximumPrecision(m_logger);

	SetZBuffer(false);
	SetZWrite(false);
	SetFilterMode(Video::TM_IFNEEDED);
	UnsetScissor();

	ResetVideoMode(width, height, pfBB, false);
	m_logger.Log("Application started...", Platform::FileLogger::INFO);
	return true;
}

void GLES2Video::Enable2D(const int width, const int height, const bool flipY)
{
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	glDisable(GL_CULL_FACE);

	glDepthFunc(GL_LEQUAL);
	glDepthRangef(0.0f, 1.0f);
}

bool GLES2Video::CheckGLError(const str_type::string& op, const Platform::FileLogger& logger)
{
	bool r = false;
	for (GLint error = glGetError(); error; error = glGetError())
	{
		std::stringstream ss;
		ss << "ERROR: after " << op << ". Error code " << error;
		logger.Log(ss.str(), Platform::FileLogger::ERROR);
		r = true;
	}
	return r;
}

TexturePtr GLES2Video::CreateTextureFromFileInMemory(
	const void *pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	TexturePtr texture(new GLES2Texture(weak_this, GS_L("from_memory"), m_fileIOHub->GetFileManager()));
	if (texture->LoadTexture(
		weak_this,
		pBuffer,
		mask,
		width,
		height,
		nMipMaps,
		bufferLength))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr GLES2Video::LoadTextureFromFile(
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height,
	const unsigned int nMipMaps)
{
	TexturePtr texture(new GLES2Texture(weak_this, fileName, m_fileIOHub->GetFileManager()));
	if (texture->LoadTexture(weak_this, fileName, mask, width, height, nMipMaps))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr GLES2Video::CreateRenderTargetTexture(
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT fmt)
{
	TexturePtr texture(new GLES2Texture(weak_this, GS_L("render_target"), m_fileIOHub->GetFileManager()));
	if (texture->CreateRenderTarget(weak_this, width, height, fmt))
	{
		return texture;
	}
	return TexturePtr();
}

SpritePtr GLES2Video::CreateSprite(
	unsigned char *pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	// TODO
	return SpritePtr();
}

SpritePtr GLES2Video::CreateSprite(
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	SpritePtr sprite(new GLES2Sprite(m_shaderContext));
	if (sprite->LoadSprite(weak_this, fileName, mask, width, height))
	{
		return sprite;
	}
	return SpritePtr();
}

SpritePtr GLES2Video::CreateRenderTarget(
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT format)
{
	SpritePtr sprite(new GLES2Sprite(m_shaderContext));
	if (sprite->CreateRenderTarget(weak_this, width, height, format))
	{
		return sprite;
	}
	return SpritePtr();
}

ShaderPtr GLES2Video::LoadShaderFromFile(
	const std::string& vsFileName,
	const std::string& vsEntry,
	const std::string& psFileName,
	const std::string& psEntry)
{
	GLES2ShaderPtr shader(new GLES2Shader(m_fileIOHub->GetFileManager(), m_shaderContext));
	if (shader->LoadShaderFromFile(m_shaderContext, vsFileName, vsEntry, psFileName, psEntry))
	{
		return shader;
	}
	return ShaderPtr();
}

ShaderPtr GLES2Video::LoadShaderFromString(
    const std::string& vsShaderName,
    const std::string& vsCodeAsciiString,
    const std::string& vsEntry,
    const std::string& psShaderName,
    const std::string& psCodeAsciiString,
    const std::string& psEntry)
{
	GLES2ShaderPtr shader(new GLES2Shader(m_fileIOHub->GetFileManager(), m_shaderContext));
	if (shader->LoadShaderFromString(m_shaderContext, vsShaderName, vsCodeAsciiString, vsEntry, psShaderName, psCodeAsciiString, psEntry))
	{
		return shader;
	}
	return ShaderPtr();
}

GLES2ShaderPtr GLES2Video::LoadGLES2ShaderFromFile(
	const std::string& vsFileName,
	const std::string& vsEntry,
	const std::string& psFileName,
	const std::string& psEntry)
{
	GLES2ShaderPtr shader(new GLES2Shader(m_fileIOHub->GetFileManager(), m_shaderContext));
	if (shader->LoadShaderFromFile(m_shaderContext, vsFileName, vsEntry, psFileName, psEntry))
	{
		return shader;
	}
	return GLES2ShaderPtr();
}

GLES2ShaderPtr GLES2Video::LoadGLES2ShaderFromString(
    const std::string& vsShaderName,
    const std::string& vsCodeAsciiString,
    const std::string& vsEntry,
    const std::string& psShaderName,
    const std::string& psCodeAsciiString,
    const std::string& psEntry)
{
	GLES2ShaderPtr shader(new GLES2Shader(m_fileIOHub->GetFileManager(), m_shaderContext));
	if (shader->LoadShaderFromString(m_shaderContext, vsShaderName, vsCodeAsciiString, vsEntry, psShaderName, psCodeAsciiString, psEntry))
	{
		return shader;
	}
	return GLES2ShaderPtr();
}

boost::any GLES2Video::GetVideoInfo()
{
	// TODO
	return 0;
}

ShaderPtr GLES2Video::GetDefaultShader()
{
	return m_defaultShader;
}

ShaderPtr GLES2Video::GetRectShader()
{
	return m_rectShader;
}

ShaderPtr GLES2Video::GetFastShader()
{
	return m_fastShader;
}

ShaderPtr GLES2Video::GetModulateShader()
{
	return m_modulateShader;
}

ShaderPtr GLES2Video::GetAddShader()
{
	return m_addShader;
}

ShaderPtr GLES2Video::GetCurrentShader()
{
	return m_currentShader;
}

ShaderContextPtr GLES2Video::GetShaderContext()
{
	return m_shaderContext;
}

bool GLES2Video::SetCurrentShader(ShaderPtr pShader)
{
	m_currentShader = pShader;
	return true;
}

boost::any GLES2Video::GetGraphicContext()
{
	// it has actually two command forwarders at the moment...
	return PullCommands(); // TO-DO/TODO: use only one!
}

Video::VIDEO_MODE GLES2Video::GetVideoMode(const unsigned int modeIdx) const
{
	VIDEO_MODE vm;
	vm.width = m_screenSize.x;
	vm.height = m_screenSize.y;
	vm.pf = Texture::PF_DEFAULT;
	vm.idx = 0x01;
	return vm;
}

unsigned int GLES2Video::GetVideoModeCount() const
{
	return 1;
}

bool GLES2Video::ResetVideoMode(const VIDEO_MODE& mode, const bool toggleFullscreen)
{
	return ResetVideoMode(mode.width, mode.height, mode.pf, false);
}

bool GLES2Video::ResetVideoMode(
	const unsigned int width,
	const unsigned int height,
	const Texture::PIXEL_FORMAT pfBB,
	const bool toggleFullscreen)
{
	m_screenSize.x = width;
	m_screenSize.y = height;

	Enable2D(width, height);

	ScreenSizeChangeListenerPtr listener = m_screenSizeChangeListener.lock();
	if (listener)
		listener->ScreenSizeChanged(GetScreenSizeF());

	return true;
}

bool GLES2Video::SetRenderTarget(SpritePtr pTarget, const unsigned int target)
{
	if (!pTarget)
	{
		m_currentTarget.reset();
		UnbindFrameBuffer();
	}
	else
	{
		if (pTarget->GetType() == Sprite::T_TARGET)
		{
			m_currentTarget = pTarget->GetTexture();
		}
		else
		{
			Message(GS_L("The current sprite has no render target texture"), GSMT_ERROR);
		}
	}
	return true;
}

unsigned int GLES2Video::GetMaxRenderTargets() const
{
	return 1;
}

unsigned int GLES2Video::GetMaxMultiTextures() const
{
	return 2;
}

bool GLES2Video::UnsetTexture(const unsigned int passIdx)
{
	return true;
}

void GLES2Video::SetZBuffer(const bool enable)
{
	if (m_zBuffer)
	{
		if (!enable)
			glDisable(GL_DEPTH_TEST);
	}
	else
	{
		if (enable)
			glEnable(GL_DEPTH_TEST);
	}
	m_zBuffer = enable;
}

bool GLES2Video::GetZBuffer() const
{
	return m_zBuffer;
}

void GLES2Video::SetZWrite(const bool enable)
{
	// dummy on opengl
}

bool GLES2Video::GetZWrite() const
{
	return true;
}

bool GLES2Video::SetClamp(const bool set)
{
	// TODO
	return false;
}

bool GLES2Video::GetClamp() const
{
	// TODO
	return false;
}

bool GLES2Video::SetSpriteDepth(const float depth)
{
	m_spriteDepth = depth;
	return true;
}

float GLES2Video::GetSpriteDepth() const
{
	return m_spriteDepth;
}

bool GLES2Video::SetScissor(const bool& enable)
{
	if (enable)
	{
		glEnable(GL_SCISSOR_TEST);
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}
	return true;
}

bool GLES2Video::SetScissor(const Rect2Di& rect)
{
	SetScissor(true);
	GLint posY;
	TexturePtr target = m_currentTarget.lock();
	if (target)
	{
		posY = static_cast<GLint>(rect.pos.y);
	}
	else
	{
		posY = static_cast<GLint>(m_screenSize.y) - static_cast<GLint>(rect.pos.y + rect.size.y);
	}

	if (m_scissor != rect)
	{
		m_scissor = rect;
		glScissor(static_cast<GLint>(rect.pos.x), posY,
				  static_cast<GLsizei>(rect.size.x), static_cast<GLsizei>(rect.size.y));
	}
	return true;
}

Rect2Di GLES2Video::GetScissor() const
{
	return m_scissor;
}

void GLES2Video::UnsetScissor()
{
	SetScissor(false);
}

bool GLES2Video::DrawLine(const Vector2& p1, const Vector2& p2, const Color& color1, const Color& color2)
{
	// TODO
	return false;
}

bool GLES2Video::DrawRectangle(
	const Vector2& v2Pos,
	const Vector2& v2Size,
	const Color& color,
	const float angle,
	const Sprite::ENTITY_ORIGIN origin)
{
	// TODO
	return false;
}

bool GLES2Video::DrawRectangle(
	const Vector2& v2Pos,
	const Vector2& v2Size,
	const Color& color0,
	const Color& color1,
	const Color& color2,
	const Color& color3,
	const float angle,
	const Sprite::ENTITY_ORIGIN origin)
{
	// TODO
	return false;
}

void GLES2Video::SetBGColor(const Color& backgroundColor)
{
	m_backgroundColor = backgroundColor;
}

Color GLES2Video::GetBGColor() const
{
	return m_backgroundColor;
}


bool GLES2Video::BeginSpriteScene(const Color& color)
{
	UnbindFrameBuffer();
	if (color != math::constant::ZERO_VECTOR4)
	{
		m_backgroundColor = color;
	}
	glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, m_backgroundColor.w);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetAlphaMode(Video::AM_PIXEL);
	m_rendering = true;
	return true;
}

bool GLES2Video::EndSpriteScene()
{
	m_rendering = false;
	return true;
}

bool GLES2Video::BeginTargetScene(const Color& color, const bool clear)
{
	// explicit static cast for better performance
	TexturePtr texturePtr = m_currentTarget.lock();
	if (texturePtr)
	{
		Texture *pTexture = texturePtr.get(); // safety compile-time error checking
		GLES2Texture *pGLES2Texture = static_cast<GLES2Texture*>(pTexture); // safer direct cast
		const GLuint target = pGLES2Texture->GetFrameBufferID();
		glBindFramebuffer(GL_FRAMEBUFFER, target);

		CheckFrameBufferStatus(m_logger, target, pGLES2Texture->GetTextureID(), false);

		if (clear)
		{
			glClearColor(color.x, color.y, color.z, color.w);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		}

		const Texture::PROFILE& profile = pGLES2Texture->GetProfile();
		Enable2D(static_cast<int>(profile.width), static_cast<int>(profile.height), true);
	}
	else
	{
		Message(GS_L("There's no render target"), GSMT_ERROR);
	}
	m_rendering = true;
	return true;
}

bool GLES2Video::EndTargetScene()
{
	SetRenderTarget(SpritePtr());
	m_rendering = false;
	UnbindFrameBuffer();
	Enable2D(m_screenSize.x, m_screenSize.y);
	return true;
}

bool GLES2Video::IsTrue(const GLboolean& enabled)
{
	return (enabled == GL_TRUE) ? true : false;
}

void GLES2Video::SetBlend(const bool enable)
{
	if (m_blend)
	{
		if (!enable)
			glDisable(GL_BLEND);
	}
	else
	{
		if (enable)
			glEnable(GL_BLEND);
	}
	m_blend = enable;
}	

bool GLES2Video::SetAlphaMode(const Video::ALPHA_MODE mode)
{
	m_alphaMode = mode;
	switch(mode)
	{
	case Video::AM_PIXEL:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		SetBlend(true);
		return true;
	case Video::AM_ADD:
		glBlendFunc(GL_ONE, GL_ONE);
		SetBlend(true);
		return true;
	case Video::AM_MODULATE:
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		SetBlend(true);
		return true;
	case Video::AM_NONE:
	case Video::AM_ALPHA_TEST: // alpha test not supported
	default:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		SetBlend(false);
		return true;
	};
}

Video::ALPHA_MODE GLES2Video::GetAlphaMode() const
{
	return m_alphaMode;
}

bool GLES2Video::SetFilterMode(const Video::TEXTUREFILTER_MODE tfm)
{
	// NOTE: it won't work on OpenGL ES exactly like in the D3D9 implementation since
	// the texture has to be previously bound before we reset filter mode
	m_textureFilterMode = tfm;
	switch (tfm)
	{
	case Video::TM_IFNEEDED:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case Video::TM_ALWAYS:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case Video::TM_NEVER:
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	};
	return true;
}

Video::TEXTUREFILTER_MODE GLES2Video::GetFilterMode() const
{
	return m_textureFilterMode;
}

bool GLES2Video::Rendering() const
{
	return m_rendering;
}

bool GLES2Video::SaveScreenshot(
	const str_type::char_t* name,
	const Texture::BITMAP_FORMAT fmt,
	Rect2Di rect)
{
	// TODO
	return false;
}

Vector2i GLES2Video::GetClientScreenSize() const
{
	return m_screenSize;
}

Application::APP_STATUS GLES2Video::HandleEvents()
{
	ComputeFPSRate();
	if (m_quit)
	{
		Command(Platform::NativeCommandAssembler::QuitApplication());
	}
	return m_quit ? APP_QUIT : APP_OK;
}

float GLES2Video::GetFPSRate() const
{
	return Max(1.0f, m_fpsRate);
}

void GLES2Video::ComputeFPSRate()
{
	m_frameCount++;

	const float currentTime = GetElapsedTimeF(TU_SECONDS);
	const float timeInterval = currentTime - m_previousTime;

	if (timeInterval > 1.0f)
	{
		m_fpsRate = static_cast<float>(m_frameCount) * (timeInterval);
		m_previousTime = currentTime;
		m_frameCount = 0;
	}
}

void GLES2Video::Message(const str_type::string& text, const GS_MESSAGE_TYPE type) const
{
	str_type::string str;
	Platform::FileLogger::TYPE logType = Platform::FileLogger::ERROR;
	switch (type)
	{
	case GSMT_WARNING:
		str = "WARNING: ";
		logType = Platform::FileLogger::WARNING;
		break;
	case GSMT_INFO:
		str = "Info: ";
		logType = Platform::FileLogger::INFO;
		break;
	case GSMT_ERROR:
		str = "ERROR: ";
		logType = Platform::FileLogger::ERROR;
		break;
	}
	m_logger.Log(str + text, logType);
}

void GLES2Video::Quit()
{
	m_quit = true;
	Command(Platform::NativeCommandAssembler::QuitApplication());
}

void GLES2Video::EnableQuitShortcuts(const bool enable)
{
	// TODO
}

bool GLES2Video::QuitShortcutsEnabled()
{
	// TODO
	return false;
}

bool GLES2Video::SetWindowTitle(const str_type::string& title)
{
	m_windowTitle = title;
	return true;
}

str_type::string GLES2Video::GetWindowTitle() const
{
	return m_windowTitle;
}

void GLES2Video::EnableMediaPlaying(const bool enable)
{
	// TODO
}

Vector2 GLES2Video::GetCurrentTargetSize() const
{
	TexturePtr currentTarget = m_currentTarget.lock();
	return (currentTarget) ? currentTarget->GetBitmapSize() : GetScreenSizeF();
}

bool GLES2Video::IsWindowed() const
{
	return false;
}

Vector2i GLES2Video::GetScreenSize() const
{
	return GetScreenSizeF().ToVector2i();
}

Vector2 GLES2Video::GetScreenSizeF() const
{
	const float virtualScreenHeight = GetVirtualScreenHeight();
	const float virtualWidth = m_screenSize.x * (virtualScreenHeight / m_screenSize.y);
	return math::Vector2(virtualWidth, virtualScreenHeight);
}

math::Vector2 GLES2Video::GetScreenSizeInPixels() const
{
	return Vector2(static_cast<float>(m_screenSize.x), static_cast<float>(m_screenSize.y));
}

Vector2i GLES2Video::GetWindowPosition()
{
	return Vector2i(0, 0);
}

void GLES2Video::SetWindowPosition(const Vector2i &v2)
{
	// dummy
}

Vector2i GLES2Video::ScreenToWindow(const Vector2i &v2Point) const
{
	return v2Point;
}

bool GLES2Video::WindowVisible() const
{
	return true;
}

bool GLES2Video::WindowInFocus() const
{
	return true;
}

bool GLES2Video::HideCursor(const bool hide)
{
	return true;
}

bool GLES2Video::IsCursorHidden() const
{
	return true;
}

const Platform::FileLogger& GLES2Video::GetLogger() const
{
	return m_logger;
}

str_type::string GLES2Video::PullCommands()
{
	str_type::string out;
	ForwardCommands(out);
	return out;
}

void GLES2Video::ForwardCommand(const str_type::string& cmd)
{
#	ifdef DEBUG
	std::cout << cmd << std::endl;
#	endif
	Command(cmd);
}

Platform::FileIOHubPtr GLES2Video::GetFileIOHub()
{
	return m_fileIOHub;
}

} // namespace gs2d
