#include "GLSDLVideo.h"

#include "../GL/GLShader.h"
#include "../GL/GLTexture.h"
#include "../GL/GLPolygonRenderer.h"
#include "../../Platform/getRealTime.h"
#include "../../Platform/NativeCommandAssembler.h"
#include "../../Platform/NativeCommandForwarder.h"
#include <time.h>

namespace gs2d {

float GLSDLVideo::m_mouseWheel(0.0f);
std::string GLSDLVideo::m_lastCharInput("");

GLSDLVideo::GLSDLVideo(
	Platform::FileIOHubPtr fileIOHub,
	const unsigned int width,
	const unsigned int height,
	const std::string& winTitle,
	const bool windowed,
	const bool maximizable) :
	Video(),
	m_fileIOHub(fileIOHub),
    m_alphaMode(AM_UNKNOWN),
    m_alphaRef(0.004),
    m_zFar(1.0f),
    m_zNear(0.0f),
    m_backgroundColor(gs2d::constant::BLACK),
	m_maximizable(false),
	m_sync(true),
	m_quit(false),
	m_fpsRate(30.0f),
	m_windowHasFocus(false),
	m_windowIsVisible(false),
	m_window(NULL),
	m_glcontext(NULL)
{
	m_startTime = getRealTime();

	// initialize SDL
	m_screenSize.x = static_cast<float>(width);
	m_screenSize.y = static_cast<float>(height);

	m_maximizable = maximizable;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		Message("SDL initialization failed", GSMT_ERROR);
		return;
	}

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	// enumerates m_videoModes
	ReadDisplayModes();

	// if screen size was set to 0, find the best one instead
	if (m_screenSize.x == 0 || m_screenSize.y == 0)
	{
		m_screenSize = CatchBestScreenResolution();
	}

	if ((m_window = SDL_CreateWindow(
			winTitle.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			static_cast<int>(m_screenSize.x),
			static_cast<int>(m_screenSize.y),
			AssembleFlags(windowed, IsMaximizable(), SyncEnabled()))) != NULL)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		m_glcontext = SDL_GL_CreateContext(m_window);
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
	}
	else
	{
		return;
	}

	SDL_GL_SetSwapInterval(1);

	// initialize OpenGL
	SetAlphaMode(Video::AM_PIXEL);

	SetZBuffer(false);

	Enable2DStates();
	
	gs2d::Application::SharedData.Create("com.ethanonengine.usingSuperSimple", "true", true /*constant*/);
}

GLSDLVideo::~GLSDLVideo()
{
	SDL_GL_DeleteContext(m_glcontext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

PolygonRendererPtr GLSDLVideo::CreatePolygonRenderer(
		const std::vector<PolygonRenderer::Vertex>& vertices,
		const std::vector<uint32_t>& indices,
		const PolygonRenderer::POLYGON_MODE mode)
{
	return PolygonRendererPtr(new GLPolygonRenderer(vertices, indices, mode));
}

ShaderPtr GLSDLVideo::LoadShaderFromFile(
	const std::string& vsFileName,
	const std::string& vsEntry,
	const std::string& psFileName,
	const std::string& psEntry)
{
	ShaderPtr shader = ShaderPtr(new GLShader(GetFileIOHub()->GetFileManager()));
	if (shader->LoadShaderFromFile(ShaderContextPtr(), vsFileName, vsEntry, psFileName, psEntry))
	{
		return shader;
	}
	return ShaderPtr();
}

ShaderPtr GLSDLVideo::LoadShaderFromString(
	const std::string& vsShaderName,
	const std::string& vsCodeAsciiString,
	const std::string& vsEntry,
	const std::string& psShaderName,
	const std::string& psCodeAsciiString,
	const std::string& psEntry)
{
	ShaderPtr shader(new GLShader(GetFileIOHub()->GetFileManager()));
	if (shader->LoadShaderFromString(
		ShaderContextPtr(),
		vsShaderName,
		vsCodeAsciiString,
		vsEntry,
		psShaderName,
		psCodeAsciiString,
		psEntry))
	{
		return shader;
	}
	return ShaderPtr();
}

TexturePtr GLSDLVideo::CreateTextureFromFileInMemory(
	const void *pBuffer,
	const unsigned int bufferLength,
	const unsigned int nMipMaps)
{
	TexturePtr texture(new GLTexture(weak_this, GetFileIOHub()->GetFileManager()));
	if (texture->LoadTexture(pBuffer, nMipMaps, bufferLength))
	{
		return texture;
	}
	return TexturePtr();
}

TexturePtr GLSDLVideo::LoadTextureFromFile(
	const std::string& fileName,
	const unsigned int nMipMaps)
{
	TexturePtr texture(new GLTexture(weak_this, GetFileIOHub()->GetFileManager()));
	if (texture->LoadTexture(fileName, nMipMaps))
	{
		return texture;
	}
	return TexturePtr();
}

void GLSDLVideo::Enable2DStates()
{
	int w, h;
	SDL_GL_GetDrawableSize(m_window, &w, &h);
	glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));

	glDisable(GL_CULL_FACE);
	glDisable(GL_DITHER);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
	glDepthRange(0.0f, 1.0f);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
}

bool GLSDLVideo::SetAlphaMode(const ALPHA_MODE mode)
{
	m_alphaMode = mode;
	switch(mode)
	{
	case AM_ADD:
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		break;
	case AM_MODULATE:
		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		break;
	case AM_NONE:
		glDisable(GL_BLEND);
		break;
	case AM_PIXEL:
	default:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	};
	return true;
}

Video::ALPHA_MODE GLSDLVideo::GetAlphaMode() const
{
	return m_alphaMode;
}

void GLSDLVideo::SetZBuffer(const bool enable)
{
	if (enable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

bool GLSDLVideo::GetZBuffer() const
{
	GLboolean enabled;
	glGetBooleanv(GL_DEPTH_TEST, &enabled);
	return (enabled == GL_TRUE);
}

void GLSDLVideo::SetBackgroundColor(const Color& backgroundColor)
{
	m_backgroundColor = backgroundColor;
}

Color GLSDLVideo::GetBackgroundColor() const
{
	return m_backgroundColor;
}

void GLSDLVideo::ReadDisplayModes()
{
    const int PRIMARY_DISPLAY = 0;

    const int numDisplayModes = SDL_GetNumDisplayModes(PRIMARY_DISPLAY);

    for (int t = 0; t < numDisplayModes; t++)
    {
        SDL_DisplayMode mode;
        if (SDL_GetDisplayMode(PRIMARY_DISPLAY, t, &mode) == 0)
        {
            VIDEO_MODE videoMode;
            videoMode.width  = mode.w;
            videoMode.height = mode.h;
            videoMode.idx = mode.format;

            switch (SDL_BYTESPERPIXEL(mode.format))
            {
            case 4:
                videoMode.pf = Texture::PF_32BIT;
                break;
            case 3:
                videoMode.pf = Texture::PF_24BIT;
                break;
            case 2:
                videoMode.pf = Texture::PF_16BIT;
                break;
            default:
                videoMode.pf = Texture::PF_DEFAULT;
                break;
            }
            m_videoModes.push_back(videoMode);
        }
    }
    std::sort(m_videoModes.begin(), m_videoModes.end());
}

bool GLSDLVideo::BeginRendering(const Color& bgColor)
{
	const Color color(bgColor != math::constant::ZERO_VECTOR4 ? bgColor : m_backgroundColor);
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetAlphaMode(Video::AM_PIXEL);
	
	m_rendering = true;
	return true;
}

bool GLSDLVideo::EndRendering()
{
	ComputeFPSRate();
	m_rendering = false;
	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		ResetVideoMode(static_cast<unsigned int>(GetScreenSize().x), static_cast<unsigned int>(GetScreenSize().y), Texture::PF_DEFAULT);
	}

	SDL_GL_SwapWindow(m_window);
	return true;
}

bool GLSDLVideo::ResetVideoMode(
	const VIDEO_MODE& mode,
	const bool toggleFullscreen)
{
	return ResetVideoMode(mode.width, mode.height, mode.pf, toggleFullscreen);
}

bool GLSDLVideo::ResetVideoMode(
	const unsigned int width,
	const unsigned int height,
	const Texture::PIXEL_FORMAT pfBB,
	const bool toggleFullscreen)
{
	return ResetVideoMode(width, height, pfBB, toggleFullscreen, true);
}

bool GLSDLVideo::ResetVideoMode(
	const unsigned int width,
	const unsigned int height,
	const Texture::PIXEL_FORMAT pfBB,
	const bool toggleFullscreen,
	const bool forceWindowResize)
{
	if (toggleFullscreen)
	{
		SDL_SetWindowFullscreen(m_window, (IsWindowed()) ? SDL_WINDOW_FULLSCREEN : 0);
	}

	if (forceWindowResize)
	{
		SDL_SetWindowSize(m_window, static_cast<int>(width), static_cast<int>(height));
	}

	// Re-assign OpenGL context to new window
	SDL_GL_MakeCurrent(m_window, m_glcontext);

	m_screenSize.x = static_cast<float>(width);
	m_screenSize.y = static_cast<float>(height);

	Enable2DStates();

	// call listener if there's any
	ScreenSizeChangeListenerPtr listener = m_screenSizeChangeListener.lock();
	if (listener)
	{
		listener->ScreenSizeChanged(GetScreenSizeF());
	}

	return true;
}

math::Vector2 GLSDLVideo::CatchBestScreenResolution() const
{
	SDL_DisplayMode current_dm;
	math::Vector2 screen;
	if (SDL_GetDesktopDisplayMode(0, &current_dm) == 0)
	{
		screen.x = static_cast<float>(current_dm.w);
		screen.y = static_cast<float>(current_dm.h);
	}
	else
	{
		const VIDEO_MODE& highest = m_videoModes[m_videoModes.size() - 1];
		screen.x = static_cast<float>(highest.width);
		screen.y = static_cast<float>(highest.height);
	}
	return screen;

}

// Window and application implementations

Uint32 GLSDLVideo::AssembleFlags(const bool windowed, const bool maximizable, const bool sync)
{
	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;

	if (!windowed)
		flags |= SDL_WINDOW_FULLSCREEN;
	else if (maximizable && windowed)
		flags |= SDL_WINDOW_RESIZABLE;

	return flags;
}

Video::VIDEO_MODE GLSDLVideo::GetVideoMode(const unsigned int modeIdx) const
{
	if (modeIdx >= GetVideoModeCount())
		return VIDEO_MODE();
	else
		return m_videoModes[modeIdx];
}

unsigned int GLSDLVideo::GetVideoModeCount() const
{
	return static_cast<unsigned int>(m_videoModes.size());
}

Platform::FileIOHubPtr GLSDLVideo::GetFileIOHub()
{
	return m_fileIOHub;
}

void GLSDLVideo::Message(const std::string& text, const GS_MESSAGE_TYPE type) const
{
	ShowMessage(text, type);
}

Application::APP_STATUS GLSDLVideo::HandleEvents()
{
	m_mouseWheel = 0.0f;
	m_lastCharInput.clear();
	APP_STATUS r = APP_OK;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
				case SDL_WINDOWEVENT_RESIZED:
					ResetVideoMode(event.window.data1, event.window.data2, Texture::PF_UNKNOWN, false, false);
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					m_windowHasFocus = true;
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					m_windowHasFocus = false;
					break;
				case SDL_WINDOWEVENT_SHOWN:
					m_windowIsVisible = true;
					break;
				case SDL_WINDOWEVENT_HIDDEN:
					m_windowIsVisible = false;
					break;
			}
			break;
		case SDL_QUIT:
			r = APP_QUIT;
			break;
		case SDL_MOUSEWHEEL:
				m_mouseWheel = static_cast<float>(event.wheel.y);
			break;
		case SDL_TEXTINPUT:
			m_lastCharInput = event.text.text;
			break;
		case SDL_DROPFILE:
			if (m_fileOpenListener)
			{
				const std::string file = event.drop.file;
				m_fileOpenListener->OnFileOpen(file);
			}
			break;
		}
	}

	if (!m_windowIsVisible)
		r = APP_SKIP;

	if (m_quit)
		r = APP_QUIT;

	return r;
}

void GLSDLVideo::Quit()
{
	m_quit = true;
}

bool GLSDLVideo::SyncEnabled() const
{
	return m_sync;
}

bool GLSDLVideo::IsMaximizable() const
{
	return m_maximizable;
}

float GLSDLVideo::GetElapsedTimeF(const TIME_UNITY unity) const
{
	return static_cast<float>(GetElapsedTimeD(unity));
}

unsigned long GLSDLVideo::GetElapsedTime(const TIME_UNITY unity) const
{
	return static_cast<unsigned long>(GetElapsedTimeD(unity));
}

double GLSDLVideo::GetElapsedTimeD(const TIME_UNITY unity) const
{
	double elapsedTimeS = getRealTime() - m_startTime;
	switch (unity)
	{
	case TU_HOURS:
		elapsedTimeS /= 60.0;
		elapsedTimeS /= 60.0;
		break;
	case TU_MINUTES:
		elapsedTimeS /= 60.0;
		break;
	case TU_MILLISECONDS:
		elapsedTimeS *= 1000.0;
	case TU_SECONDS:
	default:
		break;
	};
	return elapsedTimeS;
}

void GLSDLVideo::ComputeFPSRate()
{
	static float counter = 0.0f;
	const clock_t current = GetElapsedTime(TU_MILLISECONDS);
	static clock_t last = current;

	const clock_t elapsed = current - last;
	if (elapsed > 500)
	{
		m_fpsRate = (counter * 2);
		counter = 0.0f;
		last = current;
	}
	else
	{
		counter++;
	}
}

float GLSDLVideo::GetFPSRate() const
{
	return math::Max(1.0f, m_fpsRate);
}

std::string GLSDLVideo::GetWindowTitle() const
{
	return SDL_GetWindowTitle(m_window);
}

bool GLSDLVideo::SetWindowTitle(const std::string& title)
{
	SDL_SetWindowTitle(m_window, title.c_str());
	return true;
}

math::Vector2i GLSDLVideo::GetScreenSize() const
{
	return GetScreenSizeF().ToVector2i();
}

math::Vector2 GLSDLVideo::GetScreenSizeF() const
{
	const float virtualScreenHeight = GetVirtualScreenHeight();
	const float virtualWidth = m_screenSize.x * (virtualScreenHeight / m_screenSize.y);
	return math::Vector2(virtualWidth, virtualScreenHeight);
}

math::Vector2 GLSDLVideo::GetScreenSizeInPixels() const
{
	return m_screenSize;
}

void GLSDLVideo::EnableQuitShortcuts(const bool enable)
{
	// not allowed on Mac for now (AppStore rejects apps that completely ignore it)
}

bool GLSDLVideo::QuitShortcutsEnabled()
{
	return true;
}

bool GLSDLVideo::IsWindowed() const
{
	return !(SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN);
}

bool GLSDLVideo::WindowVisible() const
{
	return m_windowIsVisible;
}

bool GLSDLVideo::WindowInFocus() const
{
	return m_windowHasFocus;
}

void GLSDLVideo::ForwardCommand(const std::string& cmd)
{
#	ifdef _DEBUG
	std::cout << cmd << std::endl;
#	endif
	Command(cmd);
}


std::string GLSDLVideo::PullCommands()
{
	std::string out;
	ForwardCommands(out);
	return out;
}

bool GLSDLVideo::HideCursor(const bool hide)
{
	SDL_ShowCursor(hide ? SDL_DISABLE : SDL_ENABLE);
	return true;
}

bool GLSDLVideo::IsCursorHidden() const
{
	return (SDL_ShowCursor(-1));
}

math::Vector2i GLSDLVideo::GetClientScreenSize() const
{
	SDL_DisplayMode mode;
	
	// if the window's display can't be found, use primary as default
	int display = SDL_GetWindowDisplayIndex(m_window);
	if (display < 0)
	{
		display = 0;
	}
	
	SDL_GetCurrentDisplayMode(display, &mode);
	return math::Vector2i(mode.w, mode.h);
}

math::Vector2i GLSDLVideo::GetWindowPosition()
{
	math::Vector2i r;
	SDL_GetWindowPosition(m_window, &r.x, &r.y);
	return r;
}

void GLSDLVideo::SetWindowPosition(const math::Vector2i &v2)
{
	SDL_SetWindowPosition(m_window, v2.x, v2.y);
}

math::Vector2i GLSDLVideo::ScreenToWindow(const math::Vector2i &v2Point) const
{
	// yet to be implemented
	return v2Point;
}

} // namespace gs2d
