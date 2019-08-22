#ifndef GS2D_WINDOW_H_
#define GS2D_WINDOW_H_

#include "Math/Vector2.h"
#include "Math/Vector2i.h"

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace gs2d {

class Window
{
public:
	virtual void EnableQuitShortcuts(const bool enable) = 0;
	virtual bool QuitShortcutsEnabled() = 0;

	virtual bool SetWindowTitle(const std::string& title) = 0;
	virtual std::string GetWindowTitle() const = 0;

	virtual bool IsWindowed() const = 0;

	virtual math::Vector2i GetScreenSize() const = 0;
	virtual math::Vector2 GetScreenSizeF() const = 0;
	virtual math::Vector2 GetScreenSizeInPixels() const = 0;

	virtual math::Vector2i GetWindowPosition() = 0;
	virtual void SetWindowPosition(const math::Vector2i &v2) = 0;

	virtual math::Vector2i ScreenToWindow(const math::Vector2i &v2Point) const = 0;

	virtual bool WindowVisible() const = 0;
	virtual bool WindowInFocus() const = 0;

	virtual bool HideCursor(const bool hide) = 0;
	virtual bool IsCursorHidden() const = 0;
};

typedef boost::shared_ptr<Window> WindowPtr;
typedef boost::weak_ptr<Window> WindowWeakPtr;

} // namespace gs2d

#endif
