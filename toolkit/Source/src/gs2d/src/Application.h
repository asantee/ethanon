/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#ifndef GS2D_APPLICATION_H_
#define GS2D_APPLICATION_H_

#include "Types.h"
#include "Texture.h"
#include "Math/GameMath.h"

#include "Platform/FileIOHub.h"
#include "Platform/SharedData/SharedDataManager.h"

namespace gs2d {

/// Shows an error, warning or info message to the user
void ShowMessage(str_type::stringstream& stream, const GS_MESSAGE_TYPE type = GSMT_ERROR);

/// Shows an error, warning or info message to the user
void ShowMessage(const str_type::string& str, const GS_MESSAGE_TYPE type = GSMT_ERROR);

/**
 * \brief Abstracts all application related methods
 *
 * This class contains methods that will handle the application
 * tasks, such as events and timer.
 */
class Application
{
public:
	enum APP_STATUS
	{
		APP_OK = 0,
		APP_QUIT = 1,
		APP_SKIP = 2
	};

	enum TIME_UNITY
	{
		TU_MILLISECONDS = 0,
		TU_SECONDS = 1,
		TU_MINUTES = 2,
		TU_HOURS = 3
	};

	class ScreenSizeChangeListener
	{
	public:
		virtual void ScreenSizeChanged(const math::Vector2& newScreenSize) = 0;
	};
	typedef boost::shared_ptr<ScreenSizeChangeListener> ScreenSizeChangeListenerPtr;
	typedef boost::weak_ptr<ScreenSizeChangeListener> ScreenSizeChangeListenerWeakPtr;

	static Platform::SharedDataManager SharedData;

	/// Presents the back buffer, handle events and clears the screen
	virtual bool ManageLoop() = 0;

	/// Returns the client's current screen size in pixels
	virtual math::Vector2i GetClientScreenSize() const = 0;

	/// Handles application events
	virtual APP_STATUS HandleEvents() = 0;
	virtual float GetFPSRate() const = 0;

	/** \brief Sends a message to the user.
	 * \param text Zero terminated string of the message.
	 * \param type Type of the message.
	 */
	virtual void Message(const str_type::string& text, const GS_MESSAGE_TYPE type = GSMT_ERROR) const = 0;

	/** \brief Returns the current application elapsed time.
	 * \param unity Time unity.
	 */
	virtual unsigned long GetElapsedTime(const TIME_UNITY unity = TU_MILLISECONDS) const = 0;

	/** \brief Returns the current application elapsed time (floating point value).
	 * \param unity Time unity.
	 */
	virtual float GetElapsedTimeF(const TIME_UNITY unity = TU_MILLISECONDS) const = 0;

	/// Resets the application timer
	virtual void ResetTimer() = 0;

	virtual void ForwardCommand(const str_type::string& cmd) = 0;
	virtual str_type::string PullCommands() = 0;

	/// Posts a quit message
	virtual void Quit() = 0;

	static str_type::string GetPlatformName();

	virtual Platform::FileIOHubPtr GetFileIOHub() = 0;

	virtual void SetScreenSizeChangeListener(const ScreenSizeChangeListenerPtr& listener);

protected:
	ScreenSizeChangeListenerWeakPtr m_screenSizeChangeListener;

	virtual bool StartApplication(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const bool windowed,
		const bool sync,
		const Texture::PIXEL_FORMAT pfBB = Texture::PF_UNKNOWN,
		const bool maximizable = false) = 0;
};

typedef boost::shared_ptr<Application> ApplicationPtr;
typedef boost::weak_ptr<Application> ApplicationWeakPtr;

/// When called once per frame, returns the last frame time delta
GS2D_API unsigned long ComputeElapsedTime(ApplicationPtr app);
GS2D_API float ComputeElapsedTimeF(ApplicationPtr app);

} // namespace gs2d

#endif
