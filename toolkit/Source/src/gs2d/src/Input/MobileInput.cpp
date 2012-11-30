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

#include "MobileInput.h"

namespace gs2d {

using namespace math;

MobileInput::MobileInput(const unsigned int maxTouchCount) :
	m_maxTouchCount(maxTouchCount),
	m_currentTouch(maxTouchCount),
	m_touchStates(maxTouchCount),
	m_touchMove(maxTouchCount),
	m_lastTouch(maxTouchCount),
	m_logger(Platform::FileLogger::GetLogDirectory() + "MobileInput.log.txt")
{
	for (unsigned int t = 0; t < m_maxTouchCount; t++)
	{
		m_touchMove[t] = Vector2(0, 0);
		m_lastTouch[t] = GS_NO_TOUCH;
		m_currentTouch[t] = GS_NO_TOUCH;
	}
}

GS_KEY_STATE MobileInput::GetLeftClickState() const
{
	return m_touchStates[0].GetCurrentState();
}

GS_KEY_STATE MobileInput::GetRightClickState() const
{
	// TODO
	return GSKS_UP;
}

GS_KEY_STATE MobileInput::GetMiddleClickState() const
{
	// TODO
	return GSKS_UP;
}

math::Vector2i MobileInput::GetMouseMove() const
{
	// TODO
	return Vector2i();
}

math::Vector2  MobileInput::GetMouseMoveF() const
{
	// TODO
	return Vector2();
}

bool MobileInput::SetCursorPosition(math::Vector2i v2Pos)
{
	// TODO
	return false;
}

bool MobileInput::SetCursorPositionF(math::Vector2 v2Pos)
{
	// TODO
	return false;
}

math::Vector2i MobileInput::GetCursorPosition(WindowPtr pWindow) const
{
	return Vector2i(static_cast<int>(m_currentTouch[0].x), static_cast<int>(m_currentTouch[0].y));
}

math::Vector2  MobileInput::GetCursorPositionF(WindowPtr pWindow) const
{
	return m_currentTouch[0];
}

unsigned int MobileInput::GetMaxJoysticks() const
{
	// TODO
	return 0;
}

float MobileInput::GetWheelState() const
{
	// TODO
	return 0.0f;
}

bool MobileInput::Update()
{
	for (unsigned int t = 0; t < m_maxTouchCount; t++)
	{
		m_touchStates[t].Update(m_currentTouch[t] != GS_NO_TOUCH);

		const GS_KEY_STATE state = m_touchStates[t].GetCurrentState();
		if (state == GSKS_HIT)
		{
			m_lastTouch[t] = m_currentTouch[t];
		}
		else if (state == GSKS_DOWN)
		{
			m_touchMove[t] = m_currentTouch[t] - m_lastTouch[t];
			m_lastTouch[t] = m_currentTouch[t];
		}
		else
		{
			m_touchMove[t] = Vector2(0, 0);
		}
	}
	return true;
}

void MobileInput::ShowJoystickWarnings(const bool enable)
{
	// TODO
}

bool MobileInput::IsShowingJoystickWarnings() const
{
	// TODO
	return false;
}

math::Vector2 MobileInput::GetTouchPos(const unsigned int n, WindowPtr pWindow) const
{
	if (n < m_maxTouchCount)
	{
		return m_currentTouch[n];
	}
	else
	{
		return GS_NO_TOUCH;
	}
}

GS_KEY_STATE MobileInput::GetTouchState(const unsigned int n, WindowPtr pWindow) const
{
	if (n < m_maxTouchCount)
	{
		return m_touchStates[n].GetCurrentState();
	}
	else
	{
		return GSKS_UP;
	}
}

Vector2 MobileInput::GetTouchMove(const unsigned int n) const
{
	if (n < m_maxTouchCount)
	{
		return m_touchMove[n];
	}
	else
	{
		return Vector2(0, 0);
	}
}

unsigned int MobileInput::GetMaxTouchCount() const
{
	return m_maxTouchCount;
}

str_type::char_t MobileInput::GetLastCharInput() const
{
	// TODO
	return '\0';
}

GS_KEY_STATE MobileInput::GetJoystickButtonState(const unsigned int id, const GS_JOYSTICK_BUTTON key) const
{
	// TODO
	return GSKS_UP;
}

bool MobileInput::IsJoystickButtonDown(const unsigned int id, const GS_JOYSTICK_BUTTON key) const
{
	// TODO
	return false;
}

bool MobileInput::DetectJoysticks()
{
	// TODO
	return false;
}

GS_JOYSTICK_STATUS MobileInput::GetJoystickStatus(const unsigned int id) const
{
	// TODO
	return GSJS_NOTDETECTED;
}

unsigned int MobileInput::GetNumJoyButtons(const unsigned int id) const
{
	// TODO
	return 0;
}

math::Vector2 MobileInput::GetJoystickXY(const unsigned int id) const
{
	// TODO
	return Vector2();
}

float MobileInput::GetJoystickZ(const unsigned int id) const
{
	// TODO
	return 0.0f;
}

float MobileInput::GetJoystickRudder(const unsigned int id) const
{
	// TODO
	return 0.0f;
}

math::Vector2 MobileInput::GetJoystickUV(const unsigned int id) const
{
	// TODO
	return Vector2();
}

GS_JOYSTICK_BUTTON MobileInput::GetFirstButtonDown(const unsigned int id) const
{
	// TODO
	return GSB_01;
}

unsigned int MobileInput::GetNumJoysticks() const
{
	// TODO
	return 0;
}

} // namespace gs2d