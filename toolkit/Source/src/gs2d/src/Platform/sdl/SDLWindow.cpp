#include "SDLWindow.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace gs2d {

float SDLWindow::m_mouseWheel(0.0f);
str_type::string SDLWindow::m_lastCharInput("");

// Application implementations
SDLWindow::SDLWindow(Platform::FileIOHubPtr fileIOHub) :
	m_fileIOHub(fileIOHub),
	m_maximizable(false),
	m_sync(true),
	m_quit(false),
	m_fpsRate(30.0f),
	m_windowHasFocus(false),
	m_windowIsVisible(false),
	m_window(NULL),
	m_glcontext(NULL)
{
    clock_gettime(CLOCK_MONOTONIC, &m_startTime);
}

SDLWindow::~SDLWindow()
{
	SDL_GL_DeleteContext(m_glcontext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

bool SDLWindow::StartApplication(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Texture::PIXEL_FORMAT pfBB,
	const bool maximizable)
{
	m_screenSize.x = static_cast<float>(width);
	m_screenSize.y = static_cast<float>(height);

	m_maximizable = maximizable;
	m_sync = sync;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		Message("SDL initialization failed", GSMT_ERROR);
		return false;
	}

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	// enumerates m_videoModes
	ReadDisplayModes();

	// if screen size was set to 0, find the best one instead
	if (m_screenSize.x == 0 || m_screenSize.y == 0)
	{
		m_screenSize = CatchBestScreenResolution();
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	if ((m_window = SDL_CreateWindow(
			winTitle.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			static_cast<int>(m_screenSize.x),
			static_cast<int>(m_screenSize.y),
			AssembleFlags(windowed, IsMaximizable(), SyncEnabled()))) != NULL)
	{
		m_glcontext = SDL_GL_CreateContext(m_window);
	}
	else
	{
		return false;
	}

	if (sync)
	{
		SDL_GL_SetSwapInterval(1);
	}

	chdir(m_fileIOHub->GetProgramDirectory().c_str());

	return true;
}

math::Vector2 SDLWindow::CatchBestScreenResolution() const
{
	const VIDEO_MODE& highest = m_videoModes[m_videoModes.size() - 1];
	return math::Vector2(static_cast<float>(highest.width), static_cast<float>(highest.height));
}

Uint32 SDLWindow::AssembleFlags(const bool windowed, const bool maximizable, const bool sync)
{
	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

	if (!windowed)
		flags |= SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS;
	else if (maximizable && windowed)
		flags |= SDL_WINDOW_RESIZABLE;

	return flags;
}

void SDLWindow::ReadDisplayModes()
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

Video::VIDEO_MODE SDLWindow::GetVideoMode(const unsigned int modeIdx) const
{
	if (modeIdx >= GetVideoModeCount())
		return VIDEO_MODE();
	else
		return m_videoModes[modeIdx];
}

unsigned int SDLWindow::GetVideoModeCount() const
{
	return static_cast<unsigned int>(m_videoModes.size());
}

Platform::FileIOHubPtr SDLWindow::GetFileIOHub()
{
	return m_fileIOHub;
}

void SDLWindow::Message(const str_type::string& text, const GS_MESSAGE_TYPE type) const
{
	ShowMessage(text, type);
}

Application::APP_STATUS SDLWindow::HandleEvents()
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
				const str_type::string file = event.drop.file;
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

bool SDLWindow::EndRendering()
{
	SDL_GL_SwapWindow(m_window);
	return true;
}

void SDLWindow::Quit()
{
	m_quit = true;
}

bool SDLWindow::SyncEnabled() const
{
	return m_sync;
}

bool SDLWindow::IsMaximizable() const
{
	return m_maximizable;
}

float SDLWindow::GetElapsedTimeF(const TIME_UNITY unity) const
{
	return static_cast<float>(GetElapsedTimeD(unity));
}

unsigned long SDLWindow::GetElapsedTime(const TIME_UNITY unity) const
{
	return static_cast<unsigned long>(GetElapsedTimeD(unity));
}

double SDLWindow::GetElapsedTimeD(const TIME_UNITY unity) const
{
    timespec current;
    clock_gettime(CLOCK_MONOTONIC, &current);
    
    const double sec  = static_cast<double>(current.tv_sec);
    const double nsec = static_cast<double>(current.tv_nsec) / 1000000000.0;

    const double startSec  = static_cast<double>(m_startTime.tv_sec);
    const double startNsec = static_cast<double>(m_startTime.tv_nsec) / 1000000000.0;

    double elapsedTimeS = (sec + nsec) - (startSec + startNsec);

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

void SDLWindow::ComputeFPSRate()
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

float SDLWindow::GetFPSRate() const
{
	return math::Max(1.0f, m_fpsRate);
}

str_type::string SDLWindow::GetWindowTitle() const
{
	return SDL_GetWindowTitle(m_window);
}

bool SDLWindow::SetWindowTitle(const str_type::string& title)
{
	SDL_SetWindowTitle(m_window, title.c_str());
	return true;
}

math::Vector2i SDLWindow::GetScreenSize() const
{
	return GetScreenSizeF().ToVector2i();
}

math::Vector2 SDLWindow::GetScreenSizeF() const
{
	const float virtualScreenHeight = GetVirtualScreenHeight();
	const float virtualWidth = m_screenSize.x * (virtualScreenHeight / m_screenSize.y);
	return math::Vector2(virtualWidth, virtualScreenHeight);
}

math::Vector2 SDLWindow::GetScreenSizeInPixels() const
{
	return m_screenSize;
}

void SDLWindow::EnableQuitShortcuts(const bool enable)
{
	// not allowed on Mac for now (AppStore rejects apps that completely ignore it)
}

bool SDLWindow::QuitShortcutsEnabled()
{
	return true;
}

bool SDLWindow::IsWindowed() const
{
	return !(SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN);
}

bool SDLWindow::WindowVisible() const
{
	return m_windowIsVisible;
}

bool SDLWindow::WindowInFocus() const
{
	return m_windowHasFocus;
}

void SDLWindow::ForwardCommand(const str_type::string& cmd)
{
	// TODO yet to be implemented
}

str_type::string SDLWindow::PullCommands()
{
	// TODO yet to be implemented
	return "";
}

bool SDLWindow::HideCursor(const bool hide)
{
	SDL_ShowCursor(hide ? SDL_DISABLE : SDL_ENABLE);
	return true;
}

bool SDLWindow::IsCursorHidden() const
{
	return (SDL_ShowCursor(-1));
}

math::Vector2i SDLWindow::GetClientScreenSize() const
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

math::Vector2i SDLWindow::GetWindowPosition()
{
	math::Vector2i r;
	SDL_GetWindowPosition(m_window, &r.x, &r.y);
	return r;
}

void SDLWindow::SetWindowPosition(const math::Vector2i &v2)
{
	SDL_SetWindowPosition(m_window, v2.x, v2.y);
}

math::Vector2i SDLWindow::ScreenToWindow(const math::Vector2i &v2Point) const
{
	// yet to be implemented
	return v2Point;
}

} // namespace gs2d
