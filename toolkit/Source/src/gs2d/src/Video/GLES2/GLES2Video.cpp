#include "GLES2Video.h"

#include "GLES2Include.h"

#ifdef __APPLE__
 #include "TargetConditionals.h"
#endif

#if defined(TARGET_OS_IPHONE)
  #include "../../Platform/ios/Platform.ios.h"
#elif defined(__ANDROID__)
  #include "../../Platform/android/Platform.android.h"
#endif

#include "GLES2Shader.h"

#include <sstream>

namespace gs2d {

using namespace math;

GLES2Video::GLES2Video(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub) :
	m_backgroundColor(gs2d::constant::BLACK),
	m_screenSize(width, height),
	m_windowTitle(winTitle),
	m_quit(false),
	m_fpsRate(30.0f),
	m_blend(false),
	m_zBuffer(true),
	m_fileIOHub(fileIOHub),
	m_frameCount(0),
	m_previousTime(0)
{
	StartApplication(width, height, winTitle, false, false, Texture::PF_DEFAULT, false);

    gs2d::Application::SharedData.Create("com.ethanonengine.usingSuperSimple", "true", true /*constant*/);
}

static bool HasFragmentShaderMaximumPrecision()
{
	GLint range[2], precision[1];
	glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, range, precision);
	return (range[0] != 0 && range[1] != 0 && precision[0] != 0);
}

static void LogFragmentShaderMaximumPrecision()
{
	const bool precisionResult = HasFragmentShaderMaximumPrecision();
	const str_type::string logStr = (precisionResult)
		? GS_L("High floating point fragment shader precision supported")
		: GS_L("High floating point fragment shader precision is not supported");
	std::cout << logStr << std::endl;
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
	{
		glEnable(GL_DITHER);
	}
	else
	{
		glDisable(GL_DITHER);
	}

	LogFragmentShaderMaximumPrecision();

	SetZBuffer(false);

	ResetVideoMode(width, height, pfBB, false);
	std::cout << "Application started..." << std::endl;
	return true;
}

void GLES2Video::Enable2D(const int width, const int height, const bool flipY)
{
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	glDisable(GL_CULL_FACE);

	glDepthFunc(GL_LEQUAL);
	glDepthRangef(0.0f, 1.0f);
}

bool GLES2Video::CheckGLError(const str_type::string& op)
{
	bool r = false;
	for (GLint error = glGetError(); error; error = glGetError())
	{
		std::cerr << "ERROR: after " << op << ". Error code " << error << std::endl;
		r = true;
	}
	return r;
}

TexturePtr GLES2Video::CreateTextureFromFileInMemory(
	const void *pBuffer,
	const unsigned int bufferLength,
	const unsigned int nMipMaps)
{
	TexturePtr texture(new GLES2Texture(weak_this, GS_L("from_memory"), m_fileIOHub->GetFileManager()));
	if (texture->LoadTexture(
		weak_this,
		pBuffer,
		nMipMaps,
		bufferLength))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr GLES2Video::LoadTextureFromFile(
	const str_type::string& fileName,
	const unsigned int nMipMaps)
{
	TexturePtr texture(new GLES2Texture(weak_this, fileName, m_fileIOHub->GetFileManager()));
	if (texture->LoadTexture(weak_this, fileName, nMipMaps))
	{
		return texture;
	}
	return TexturePtr();
}

ShaderPtr GLES2Video::LoadShaderFromFile(
	const std::string& vsFileName,
	const std::string& vsEntry,
	const std::string& psFileName,
	const std::string& psEntry)
{
	ShaderPtr shader(new GLES2Shader(m_fileIOHub->GetFileManager()));
	if (shader->LoadShaderFromFile(ShaderContextPtr(), vsFileName, vsEntry, psFileName, psEntry))
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
	ShaderPtr shader(new GLES2Shader(m_fileIOHub->GetFileManager()));
	if (shader->LoadShaderFromString(ShaderContextPtr(), vsShaderName, vsCodeAsciiString, vsEntry, psShaderName, psCodeAsciiString, psEntry))
	{
		return shader;
	}
	return ShaderPtr();
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

void GLES2Video::SetBGColor(const Color& backgroundColor)
{
	m_backgroundColor = backgroundColor;
}

Color GLES2Video::GetBGColor() const
{
	return m_backgroundColor;
}

bool GLES2Video::BeginRendering(const Color& color)
{
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

bool GLES2Video::EndRendering()
{
	m_rendering = false;
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
	switch (type)
	{
	case GSMT_WARNING:
		str = "WARNING: ";
		break;
	case GSMT_INFO:
		str = "Info: ";
		break;
	case GSMT_ERROR:
		str = "ERROR: ";
		break;
	}
	
	if (type == GSMT_ERROR)
	{
		std::cerr << str << text << std::endl;
	}
	else
	{
		std::cout << str << text << std::endl;
	}
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
