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

#ifndef GS2D_WINDOW_H_
#define GS2D_WINDOW_H_

#include "Math/Math.h"

namespace gs2d {

/**
 * \brief Abstracts all window related methods
 *
 * This class contains methods that will handle window
 * tasks, such as positioning and title.
 */
class Window
{
public:
	virtual void EnableQuitShortcuts(const bool enable) = 0;
	virtual bool QuitShortcutsEnabled() = 0;

	virtual bool SetWindowTitle(const str_type::string& title) = 0;
	virtual str_type::string GetWindowTitle() const = 0;

	virtual void EnableMediaPlaying(const bool enable) = 0;

	virtual bool IsWindowed() const = 0;

	virtual math::Vector2i GetScreenSize() const = 0;
	virtual math::Vector2 GetScreenSizeF() const = 0;

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
