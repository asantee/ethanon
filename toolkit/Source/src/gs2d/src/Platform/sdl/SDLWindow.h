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

#ifndef GS2D_SDL_WINDOW_H_
#define GS2D_SDL_WINDOW_H_

#include "../../Video.h"
#include <vector>
#include <sys/time.h>

namespace gs2d {

class SDLWindow : public virtual Video
{
	Platform::FileIOHubPtr m_fileIOHub;
	bool m_maximizable, m_sync, m_windowed, m_quitKeysEnabled, m_quit;
	std::vector<VIDEO_MODE> m_videoModes;
	
	float m_fpsRate;

	void ReadDisplayModes();
	timeval m_lastTime;

protected:
	bool StartApplication(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const bool windowed,
		const bool sync,
		const Texture::PIXEL_FORMAT pfBB = Texture::PF_UNKNOWN,
		const bool maximizable = false);

	bool EndSpriteScene();
	unsigned int AssembleFlags(const bool windowed, const bool maximizable, const bool sync);

	math::Vector2 m_screenSize;

public:
	SDLWindow(Platform::FileIOHubPtr fileIOHub);
	~SDLWindow();

	// Application implementations
	math::Vector2i GetClientScreenSize() const;

	APP_STATUS HandleEvents();
	float GetFPSRate() const;
	
	void Message(const str_type::string& text, const GS_MESSAGE_TYPE type = GSMT_ERROR) const;

	unsigned long GetElapsedTime(const TIME_UNITY unity = TU_MILLISECONDS) const;

	float GetElapsedTimeF(const TIME_UNITY unity = TU_MILLISECONDS) const;

	void ResetTimer();

	void ForwardCommand(const str_type::string& cmd);
	str_type::string PullCommands();

	void Quit();

	str_type::string GetPlatformName() const;

	Platform::FileIOHubPtr GetFileIOHub();
	
	// Window implementations
	void EnableQuitShortcuts(const bool enable);
	bool QuitShortcutsEnabled();

	bool SetWindowTitle(const str_type::string& title);
	str_type::string GetWindowTitle() const;

	void EnableMediaPlaying(const bool enable);

	bool IsWindowed() const;

	math::Vector2i GetScreenSize() const;
	math::Vector2 GetScreenSizeF() const;

	math::Vector2i GetWindowPosition();
	void SetWindowPosition(const math::Vector2i &v2);

	math::Vector2i ScreenToWindow(const math::Vector2i &v2Point) const;

	bool WindowVisible() const;
	bool WindowInFocus() const;
	
	bool HideCursor(const bool hide);
	bool IsCursorHidden() const;

	VIDEO_MODE GetVideoMode(const unsigned int modeIdx) const;
	unsigned int GetVideoModeCount() const;

	bool SyncEnabled() const;
	bool IsMaximizable() const;
	double GetElapsedTimeD(const TIME_UNITY unity) const;
	void ComputeFPSRate();
};
	
} // namespace gs2d

#endif
