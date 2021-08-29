#ifndef GS2D_APPLICATION_H_
#define GS2D_APPLICATION_H_

#include "Texture.h"
#include "Math/GameMath.h"

#include "Platform/FileIOHub.h"
#include "Platform/SharedData/SharedDataManager.h"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <sstream>

namespace gs2d {

enum GS_MESSAGE_TYPE
{
    GSMT_INFO = 0,
    GSMT_WARNING = 1,
    GSMT_ERROR = 2
};

/// Shows an error, warning or info message to the user
void ShowMessage(std::stringstream& stream, const GS_MESSAGE_TYPE type = GSMT_ERROR);

/// Shows an error, warning or info message to the user
void ShowMessage(const std::string& str, const GS_MESSAGE_TYPE type = GSMT_ERROR);

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

	class FileOpenListener
	{
	public:
		virtual bool OnFileOpen(const std::string& fullFilePath) = 0;
	};
	typedef boost::shared_ptr<FileOpenListener> FileOpenListenerPtr;

	virtual void SetFileOpenListener(const FileOpenListenerPtr& listener);

	class ScreenSizeChangeListener
	{
	public:
		virtual void ScreenSizeChanged(const math::Vector2& newScreenSize) = 0;
	};
	typedef boost::shared_ptr<ScreenSizeChangeListener> ScreenSizeChangeListenerPtr;
	typedef boost::weak_ptr<ScreenSizeChangeListener> ScreenSizeChangeListenerWeakPtr;

	virtual void SetScreenSizeChangeListener(const ScreenSizeChangeListenerPtr& listener);

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
	virtual void Message(const std::string& text, const GS_MESSAGE_TYPE type = GSMT_ERROR) const = 0;

	/** \brief Returns the current application elapsed time.
	 * \param unity Time unity.
	 */
	virtual unsigned long GetElapsedTime(const TIME_UNITY unity = TU_MILLISECONDS) const = 0;

	/** \brief Returns the current application elapsed time (floating point value).
	 * \param unity Time unity.
	 */
	virtual float GetElapsedTimeF(const TIME_UNITY unity = TU_MILLISECONDS) const = 0;

	virtual void ForwardCommand(const std::string& cmd) = 0;
	virtual std::string PullCommands() = 0;

	/// Posts a quit message
	virtual void Quit() = 0;

	static std::string GetPlatformName();

	virtual Platform::FileIOHubPtr GetFileIOHub() = 0;

protected:
	ScreenSizeChangeListenerWeakPtr m_screenSizeChangeListener;
	FileOpenListenerPtr m_fileOpenListener;
};

typedef boost::shared_ptr<Application> ApplicationPtr;
typedef boost::weak_ptr<Application> ApplicationWeakPtr;

/// When called once per frame, returns the last frame time delta
unsigned long ComputeElapsedTime(ApplicationPtr app);
float ComputeElapsedTimeF(ApplicationPtr app);

} // namespace gs2d

#endif
