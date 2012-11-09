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

#include "SDLInput.h"
#include "../../Platform/sdl/SDLWindow.h"

#include <SDL/SDL.h>

namespace gs2d {

GS2D_API InputPtr CreateInput(boost::any data, const bool showJoystickWarnings)
{
	GS2D_UNUSED_ARGUMENT(data);
	return InputPtr(new SDLInput(showJoystickWarnings));
}

SDLInput::SDLInput(const bool showJoystickWarnings) :
	SDLJoystick(showJoystickWarnings),
	m_cursorPos(math::Vector2i(-1,-1)),
	m_lastCursorPos(math::Vector2i(0, 0)),
	m_mouseBits(0)
{
	m_sdlKeyID[GSK_UP] = SDLK_UP;
	m_sdlKeyID[GSK_DOWN] = SDLK_DOWN;
	m_sdlKeyID[GSK_LEFT] = SDLK_LEFT;
	m_sdlKeyID[GSK_RIGHT] = SDLK_RIGHT;
	m_sdlKeyID[GSK_CTRL] = SDLK_LCTRL;
	m_sdlKeyID[GSK_ALT] = SDLK_LALT;
	m_sdlKeyID[GSK_SHIFT] = SDLK_LSHIFT;
	m_sdlKeyID[GSK_PAGEDOWN] = SDLK_PAGEDOWN;
	m_sdlKeyID[GSK_PAGEUP] = SDLK_PAGEUP;
	m_sdlKeyID[GSK_SPACE] = SDLK_SPACE;
	m_sdlKeyID[GSK_ENTER] = SDLK_RETURN;
	m_sdlKeyID[GSK_DELETE] = SDLK_DELETE;
	m_sdlKeyID[GSK_HOME] = SDLK_HOME;
	m_sdlKeyID[GSK_END] = SDLK_END;
	m_sdlKeyID[GSK_INSERT] = SDLK_INSERT;
	m_sdlKeyID[GSK_PAUSE] = SDLK_PAUSE;
	m_sdlKeyID[GSK_ESC] = SDLK_ESCAPE;
	m_sdlKeyID[GSK_BACK] = SDLK_BACKSPACE;
	m_sdlKeyID[GSK_TAB] = SDLK_TAB;
	m_sdlKeyID[GSK_PRINTSCREEN] = SDLK_PRINT;
	m_sdlKeyID[GSK_SUBTRACT] = SDLK_MINUS;
	m_sdlKeyID[GSK_ADD] = SDLK_PLUS;
	m_sdlKeyID[GSK_F1] = SDLK_F1;
	m_sdlKeyID[GSK_F2] = SDLK_F2;
	m_sdlKeyID[GSK_F3] = SDLK_F3;
	m_sdlKeyID[GSK_F4] = SDLK_F4;
	m_sdlKeyID[GSK_F5] = SDLK_F5;
	m_sdlKeyID[GSK_F6] = SDLK_F6;
	m_sdlKeyID[GSK_F7] = SDLK_F7;
	m_sdlKeyID[GSK_F8] = SDLK_F8;
	m_sdlKeyID[GSK_F9] = SDLK_F9;
	m_sdlKeyID[GSK_F10] = SDLK_F10;
	m_sdlKeyID[GSK_F11] = SDLK_F11;
	m_sdlKeyID[GSK_F12] = SDLK_F12;
	m_sdlKeyID[GSK_F13] = SDLK_F13;
	m_sdlKeyID[GSK_F14] = SDLK_F14;
	m_sdlKeyID[GSK_F15] = SDLK_F15;
	m_sdlKeyID[GSK_F16] = SDLK_LAST;
	m_sdlKeyID[GSK_F17] = SDLK_LAST;
	m_sdlKeyID[GSK_F18] = SDLK_LAST;
	m_sdlKeyID[GSK_F19] = SDLK_LAST;
	m_sdlKeyID[GSK_F20] = SDLK_LAST;
	m_sdlKeyID[GSK_F21] = SDLK_LAST;
	m_sdlKeyID[GSK_F22] = SDLK_LAST;
	m_sdlKeyID[GSK_F23] = SDLK_LAST;
	m_sdlKeyID[GSK_F24] = SDLK_LAST;
	m_sdlKeyID[GSK_A] = SDLK_a;
	m_sdlKeyID[GSK_B] = SDLK_b;
	m_sdlKeyID[GSK_C] = SDLK_c;
	m_sdlKeyID[GSK_D] = SDLK_d;
	m_sdlKeyID[GSK_E] = SDLK_e;
	m_sdlKeyID[GSK_F] = SDLK_f;
	m_sdlKeyID[GSK_G] = SDLK_g;
	m_sdlKeyID[GSK_H] = SDLK_h;
	m_sdlKeyID[GSK_I] = SDLK_i;
	m_sdlKeyID[GSK_J] = SDLK_j;
	m_sdlKeyID[GSK_K] = SDLK_k;
	m_sdlKeyID[GSK_L] = SDLK_l;
	m_sdlKeyID[GSK_M] = SDLK_m;
	m_sdlKeyID[GSK_N] = SDLK_n;
	m_sdlKeyID[GSK_O] = SDLK_o;
	m_sdlKeyID[GSK_P] = SDLK_p;
	m_sdlKeyID[GSK_Q] = SDLK_q;
	m_sdlKeyID[GSK_R] = SDLK_r;
	m_sdlKeyID[GSK_S] = SDLK_s;
	m_sdlKeyID[GSK_T] = SDLK_t;
	m_sdlKeyID[GSK_U] = SDLK_u;
	m_sdlKeyID[GSK_V] = SDLK_v;
	m_sdlKeyID[GSK_W] = SDLK_w;
	m_sdlKeyID[GSK_X] = SDLK_x;
	m_sdlKeyID[GSK_Y] = SDLK_y;
	m_sdlKeyID[GSK_Z] = SDLK_z;
	m_sdlKeyID[GSK_1] = SDLK_1;
	m_sdlKeyID[GSK_2] = SDLK_2;
	m_sdlKeyID[GSK_3] = SDLK_3;
	m_sdlKeyID[GSK_4] = SDLK_4;
	m_sdlKeyID[GSK_5] = SDLK_5;
	m_sdlKeyID[GSK_6] = SDLK_6;
	m_sdlKeyID[GSK_7] = SDLK_7;
	m_sdlKeyID[GSK_8] = SDLK_8;
	m_sdlKeyID[GSK_9] = SDLK_9;
	m_sdlKeyID[GSK_0] = SDLK_0;
	m_sdlKeyID[GSK_NUMPAD0] = SDLK_KP0;
	m_sdlKeyID[GSK_NUMPAD1] = SDLK_KP1;
	m_sdlKeyID[GSK_NUMPAD2] = SDLK_KP2;
	m_sdlKeyID[GSK_NUMPAD3] = SDLK_KP3;
	m_sdlKeyID[GSK_NUMPAD4] = SDLK_KP4;
	m_sdlKeyID[GSK_NUMPAD5] = SDLK_KP5;
	m_sdlKeyID[GSK_NUMPAD6] = SDLK_KP6;
	m_sdlKeyID[GSK_NUMPAD7] = SDLK_KP7;
	m_sdlKeyID[GSK_NUMPAD8] = SDLK_KP8;
	m_sdlKeyID[GSK_NUMPAD9] = SDLK_KP9;
	m_sdlKeyID[GSK_MINUS] = SDLK_MINUS;
	m_sdlKeyID[GSK_PLUS] = SDLK_PLUS;
	m_sdlKeyID[GSK_COMMA] = SDLK_COMMA;
	m_sdlKeyID[GSK_DOT] = SDLK_PERIOD;

	m_sdlKeyID[GSK_RMOUSE] = SDLK_LAST;
	m_sdlKeyID[GSK_LMOUSE] = SDLK_LAST;
	m_sdlKeyID[GSK_MMOUSE] = SDLK_LAST;

	UpdateCursorPos();
	m_lastCursorPos = m_cursorPos;
}

bool SDLInput::Update()
{
	const Uint8 *keystate = SDL_GetKeyState(NULL);
	for (std::size_t t = 0; t < GS_NUM_KEYS; t++)
	{
		m_keyStates[t].Update(IsKeyPressed((GS_KEY)t, keystate));
	}

	UpdateCursorPos();
	return SDLJoystick::Update();
}

bool SDLInput::IsKeyPressed(const GS_KEY key, const Uint8* keystate)
{
	switch (key)
	{
	case GSK_LMOUSE:
		return (m_mouseBits & SDL_BUTTON(1));
	case GSK_MMOUSE:
		return (m_mouseBits & SDL_BUTTON(2));
	case GSK_RMOUSE:
		return (m_mouseBits & SDL_BUTTON(3));
	default:
		// just preventing the warning
		break;
	};
	return (keystate[m_sdlKeyID[key]] != 0);
}

void SDLInput::UpdateCursorPos()
{
	m_lastCursorPos = m_cursorPos;
	m_mouseBits = SDL_GetMouseState(&m_cursorPos.x, &m_cursorPos.y);
}

bool SDLInput::SetCursorPosition(math::Vector2i v2Pos)
{
	m_cursorPos = v2Pos;
	SDL_WarpMouse(static_cast<Uint16>(v2Pos.x), static_cast<Uint16>(v2Pos.y));
	return true;
}

bool SDLInput::SetCursorPositionF(math::Vector2 v2Pos)
{
	return SetCursorPosition(v2Pos.ToVector2i());
}

math::Vector2i SDLInput::GetCursorPosition(WindowPtr pWindow) const
{
	GS2D_UNUSED_ARGUMENT(pWindow);
	return m_cursorPos;
}

math::Vector2 SDLInput::GetCursorPositionF(WindowPtr pWindow) const
{
	GS2D_UNUSED_ARGUMENT(pWindow);
	return math::ToVector2(m_cursorPos);
}

unsigned int SDLInput::GetMaxTouchCount() const
{
	return 1;
}

math::Vector2i SDLInput::GetMouseMove() const
{
	return m_cursorPos - m_lastCursorPos;
}

math::Vector2 SDLInput::GetMouseMoveF() const
{
	return math::ToVector2(GetMouseMove());
}

math::Vector2 SDLInput::GetTouchMove(const unsigned int n) const
{
	if (n == 0)
	{
		return (IsKeyDown(GSK_LMOUSE)) ? GetMouseMoveF() : math::constant::ZERO_VECTOR2;
	}
	else
	{
		return math::constant::ZERO_VECTOR2;
	}
}

math::Vector2 SDLInput::GetTouchPos(const unsigned int n, WindowPtr pWindow) const
{
	GS2D_UNUSED_ARGUMENT(n);
	return GetCursorPositionF(pWindow);
}

math::Vector3 SDLInput::GetAccelerometerData() const
{
	return math::Vector3();
}

bool SDLInput::IsKeyDown(const GS_KEY key) const
{
	const GS_KEY_STATE state = m_keyStates[key].GetCurrentState();
	return (state == GSKS_DOWN || state == GSKS_HIT);
}

GS_KEY_STATE SDLInput::GetKeyState(const GS_KEY key) const
{
	return m_keyStates[key].GetCurrentState();
}

GS_KEY_STATE SDLInput::GetLeftClickState() const
{
	return m_keyStates[GSK_LMOUSE].GetCurrentState();
}

GS_KEY_STATE SDLInput::GetRightClickState() const
{
	return m_keyStates[GSK_RMOUSE].GetCurrentState();
}

GS_KEY_STATE SDLInput::GetMiddleClickState() const
{
	return m_keyStates[GSK_MMOUSE].GetCurrentState();
}

GS_KEY_STATE SDLInput::GetTouchState(const unsigned int n, WindowPtr pWindow) const
{
	GS2D_UNUSED_ARGUMENT(n);
	GS2D_UNUSED_ARGUMENT(pWindow);
	return GetLeftClickState();
}

float SDLInput::GetWheelState() const
{
	return SDLWindow::m_mouseWheel;
}

str_type::char_t SDLInput::GetLastCharInput() const
{
	str_type::char_t c = SDLWindow::m_lastCharInput;
	switch (c)
	{
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
	case 0x08:
	case 0x0E:
	case 0x0F:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17:
	case 0x18:
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x7F:
		return GS_L('\0');
	default:
		return c;
	}
	return c;
}

} // namespace gs2d
