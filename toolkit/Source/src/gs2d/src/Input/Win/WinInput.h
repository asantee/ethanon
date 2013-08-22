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

#ifndef GS2D_WIN_INPUT_H_
#define GS2D_WIN_INPUT_H_

#include "../../Input.h"

#include "../../Platform/windows/targetver.h"

#include "../KeyStateManager.h"

#include <windows.h>

#include <map>

#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )

namespace gs2d {

struct JOYSTICK_DATA
{
	JOYSTICK_DATA()
	{
		status = GSJS_NOTDETECTED;
		z = rudder = 0.0f;
		for (unsigned int t=0; t<GSB_NUM_BUTTONS; t++)
		{
			buttonID[t] = 0x0;
		}
		nButtons = 0;
	}

	GS_JOYSTICK_STATUS status;
	math::Vector2 xy;
	float z;
	float rudder;
	math::Vector2 uv;
	DWORD buttonID[GSB_NUM_BUTTONS];
	KeyStateManager state[GSB_NUM_BUTTONS];
	int nButtons;
};

class WinInput : public Input
{
	friend InputPtr CreateInput(boost::any data, const bool showJoystickWarnings);
	friend class D3D9Video;

	WinInput(boost::any data, const bool showJoystickWarnings);

	struct TouchSpot
	{
		TouchSpot() : down(false) {}
		math::Vector2 pos, move;
		KeyStateManager keyState;
		bool down;
	};

	static void PrepareTouchInput(HWND hWnd);
	static LRESULT OnTouch(HWND hWnd, WPARAM wParam, LPARAM lParam);
	static void UpdateTouchInput();
	static bool GetTouchSpotByIndex(const unsigned int n, TouchSpot& outSpot);

	static unsigned int m_maxTouchCount;
	static std::map<DWORD, TouchSpot> m_touchSpots;

	const static unsigned int MAX_JOYSTICKS = 4;

	bool UpdateJoystick();
	JOYSTICK_DATA m_joystick[MAX_JOYSTICKS];
	bool IsArrowPressed(const GS_JOYSTICK_BUTTON key, const float direction);

	KeyStateManager m_keyState[GS_NUM_KEYS];
	math::Vector2i m_mouseMove;
	math::Vector2i m_lastPos;
	math::Vector2i m_v2NewPos;
	bool m_newPos;
	unsigned int m_nJoysticks;
	bool m_showJoyWarnings;
	int m_keyID[GS_NUM_KEYS];

	JOYCAPS m_joyCaps[MAX_JOYSTICKS];
	JOYINFOEX m_joyInfoEx[MAX_JOYSTICKS];
	float m_mouseWheel;
	wchar_t m_charInput;

public:
	bool IsKeyDown(const GS_KEY key) const;
	GS_KEY_STATE GetKeyState(const GS_KEY key) const;

	GS_KEY_STATE GetLeftClickState() const;
	GS_KEY_STATE GetRightClickState() const;
	GS_KEY_STATE GetMiddleClickState() const;

	math::Vector2i GetMouseMove() const;
	math::Vector2 GetMouseMoveF() const;

	math::Vector2 GetTouchPos(const unsigned int n, WindowPtr pWindow) const;
	GS_KEY_STATE  GetTouchState(const unsigned int n, WindowPtr pWindow) const;
	unsigned int GetMaxTouchCount() const;
	math::Vector2 GetTouchMove(const unsigned int n) const;

	bool SetCursorPosition(math::Vector2i v2Pos);
	bool SetCursorPositionF(math::Vector2 v2Pos);
	math::Vector2i GetCursorPosition(WindowPtr pWindow) const;
	math::Vector2  GetCursorPositionF(WindowPtr pWindow) const;

	float GetWheelState() const;

	bool Update();

	void ShowJoystickWarnings(const bool enable);
	bool IsShowingJoystickWarnings() const;

	wchar_t GetLastCharInput() const;

	unsigned int GetMaxJoysticks() const;
	GS_KEY_STATE GetJoystickButtonState(const unsigned int id, const GS_JOYSTICK_BUTTON key) const;
	bool IsJoystickButtonDown(const unsigned int id, const GS_JOYSTICK_BUTTON key) const;
	bool DetectJoysticks();
	GS_JOYSTICK_STATUS GetJoystickStatus(const unsigned int id) const;
	unsigned int GetNumJoyButtons(const unsigned int id) const;
	math::Vector2 GetJoystickXY(const unsigned int id) const;
	float GetJoystickZ(const unsigned int id) const;
	float GetJoystickRudder(const unsigned int id) const;
	math::Vector2 GetJoystickUV(const unsigned int id) const;
	GS_JOYSTICK_BUTTON GetFirstButtonDown(const unsigned int id) const;
	unsigned int GetNumJoysticks() const;

	math::Vector3 GetAccelerometerData() const;
};

typedef boost::shared_ptr<WinInput> WinInputPtr;
typedef boost::weak_ptr<WinInput> WinInputWeakPtr;

} // namespace gs2d

#endif
