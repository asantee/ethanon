/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this
 software and associated documentation files (the "Software"), to deal in the
 Software without restriction, including without limitation the rights to use, copy,
 modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 and to permit persons to whom the Software is furnished to do so, subject to the
 following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 --------------------------------------------------------------------------------------*/

#include "SDLWindow.h"

#include <SDL/SDL.h>

namespace gs2d {

static int SetPixelFormat(const SDL_VideoInfo* info, const Texture::PIXEL_FORMAT format)
{
	int bpp; // = info->vfmt->BitsPerPixel;
	if (format == Texture::PF_16BIT)
	{
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		bpp = 16;
	}
	else if (format == Texture::PF_32BIT)
	{
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		bpp = 32;
	}
	else
	{
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		bpp = 0;
	}
	return bpp;
}

// Application implementations
SDLWindow::SDLWindow(Platform::FileIOHubPtr fileIOHub) :
	m_fileIOHub(fileIOHub),
	m_maximizable(false)
{
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

	if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		Message("SDL initialization failed", GSMT_ERROR);
		return false;
	}

	const SDL_VideoInfo* info = SDL_GetVideoInfo();

	const int bpp = SetPixelFormat(info, pfBB);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	Uint32 flags = SDL_OPENGL;
	if (!windowed)
		flags |= SDL_FULLSCREEN;

	if (sync)
	{
		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
		flags |= SDL_ASYNCBLIT;
	}

	if (SDL_SetVideoMode(static_cast<int>(m_screenSize.x), static_cast<int>(m_screenSize.y), bpp, flags) == 0)
	{
		Message("Invalid video mode - GAMESPACE_VIDEO_HANDLER::StartApplication", GSMT_ERROR);
		return false;
	}
		
	SetWindowTitle(winTitle);
	ReadDisplayModes();

	/*g_bRunning = true;

	m_ShaderContext.RegisterShaderHandler(m_pDevice);
	m_DefaultVS.LoadShader(&m_ShaderContext, g_szDefaultVS, GSSF_VERTEX, GSSM_STRING, GSSP_MODEL_2, "sprite");
	m_RectVS.LoadShader(&m_ShaderContext, g_szDefaultVS, GSSF_VERTEX, GSSM_STRING, GSSP_MODEL_2, "rectangle");
	m_FontVS.LoadShader(&m_ShaderContext, g_szDefaultVS, GSSF_VERTEX, GSSM_STRING, GSSP_MODEL_2, "font");
	m_pCurrentVS = &m_DefaultVS;
	m_DefaultVS.SetConstant2F("cameraPos", GetCameraPos());

	gsSetupShaderViewData(GetCurrentVS(), &m_RectVS, &m_FontVS);

	pInfo->m_dwTime = SDL_GetTicks();*/
	return true;
}

void SDLWindow::ReadDisplayModes()
{
	SDL_Rect **modes;
	modes = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_HWSURFACE);

	if (modes == NULL || modes == (SDL_Rect**)-1)
	{
		return;
	}

	// count number of modes
	std::size_t videoModeCount;
	for (videoModeCount = 0; modes[videoModeCount]; videoModeCount++);

	const Uint8 bitsPerPixel = SDL_GetVideoInfo()->vfmt->BitsPerPixel;

	for (std::size_t t = 0; t < videoModeCount; t++)
	{
		VIDEO_MODE videoMode;
		videoMode.width  = modes[t]->w;
		videoMode.height = modes[t]->h;
		videoMode.idx = static_cast<GS_DWORD>(bitsPerPixel);
		videoMode.pf = (bitsPerPixel == 32) ? Texture::PF_16BIT : Texture::PF_32BIT;
		m_videoModes.push_back(videoMode);
	}
}

str_type::string SDLWindow::GetPlatformName() const
{
#	ifdef MACOSX
	return "macosx";
#	endif
}

Platform::FileIOHubPtr SDLWindow::GetFileIOHub()
{
	return m_fileIOHub;
}

void SDLWindow::Message(const str_type::string& text, const GS_MESSAGE_TYPE type) const
{
	ShowMessage(text, type);
}











bool SDLWindow::ManageLoop()
{
	return false;
}

math::Vector2i SDLWindow::GetClientScreenSize() const
{
	return math::Vector2i(0, 0);
}

Application::APP_STATUS SDLWindow::HandleEvents()
{
	return Application::APP_OK;
}

float SDLWindow::GetFPSRate() const
{
	return 0.0f;
}

unsigned long SDLWindow::GetElapsedTime(const TIME_UNITY unity) const
{
	return 0;
}

float SDLWindow::GetElapsedTimeF(const TIME_UNITY unity) const
{
	return 0.0f;
}

void SDLWindow::ResetTimer()
{
}

void SDLWindow::ForwardCommand(const str_type::string& cmd)
{
}

str_type::string SDLWindow::PullCommands()
{
	return "";
}

void SDLWindow::Quit()
{
}
	
/////////////////////////////
//
//  Window implementations
//
/////////////////////////////

str_type::string SDLWindow::GetWindowTitle() const
{
	char *title;
	char *iconText;
	SDL_WM_GetCaption(&title, &iconText);
	return title;
}

bool SDLWindow::SetWindowTitle(const str_type::string& title)
{
	SDL_WM_SetCaption(title.c_str(), title.c_str());
	return true;
}

math::Vector2i SDLWindow::GetScreenSize() const
{
	return GetScreenSizeF().ToVector2i();
}

math::Vector2 SDLWindow::GetScreenSizeF() const
{
	return m_screenSize;
}












void SDLWindow::EnableQuitShortcuts(const bool enable)
{
}

bool SDLWindow::QuitShortcutsEnabled()
{
	return false;
}

void SDLWindow::EnableMediaPlaying(const bool enable)
{
}

bool SDLWindow::IsWindowed() const
{
	return false;
}

math::Vector2i SDLWindow::GetWindowPosition()
{
	return math::Vector2i(0, 0);
}

void SDLWindow::SetWindowPosition(const math::Vector2i &v2)
{
}

math::Vector2i SDLWindow::ScreenToWindow(const math::Vector2i &v2Point) const
{
	return v2Point;
}

bool SDLWindow::WindowVisible() const
{
	return false;
}

bool SDLWindow::WindowInFocus() const
{
	return false;
}

bool SDLWindow::HideCursor(const bool hide)
{
	return false;
}

bool SDLWindow::IsCursorHidden() const
{
	return false;
}

} // namespace gs2d
