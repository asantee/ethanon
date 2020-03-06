#include "SDLInput.h"

#include "../../Video/GLSDL/GLSDLVideo.h"

#include <SDL2/SDL.h>

#define GS2D_UNUSED_ARGUMENT(argument) ((void)(argument))

namespace gs2d {

InputPtr CreateInput(const bool showJoystickWarnings, std::string* inputSource)
{
	return InputPtr(new SDLInput(showJoystickWarnings));
}

SDLInput::SDLInput(const bool showJoystickWarnings) :
	SDLJoystick(showJoystickWarnings),
	m_cursorPos(math::Vector2i(-1,-1)),
	m_lastCursorPos(math::Vector2i(0, 0)),
	m_mouseBits(0)
{
	m_sdlKeyID[GSK_UP] = SDL_SCANCODE_UP;
	m_sdlKeyID[GSK_DOWN] = SDL_SCANCODE_DOWN;
	m_sdlKeyID[GSK_LEFT] = SDL_SCANCODE_LEFT;
	m_sdlKeyID[GSK_RIGHT] = SDL_SCANCODE_RIGHT;
	m_sdlKeyID[GSK_CTRL] = SDL_SCANCODE_LCTRL;
	m_sdlKeyID[GSK_RCTRL] = SDL_SCANCODE_RCTRL;
	m_sdlKeyID[GSK_ALT] = SDL_SCANCODE_LALT;
	m_sdlKeyID[GSK_RALT] = SDL_SCANCODE_RALT;
	m_sdlKeyID[GSK_SHIFT] = SDL_SCANCODE_LSHIFT;
	m_sdlKeyID[GSK_RSHIFT] = SDL_SCANCODE_RSHIFT;
	m_sdlKeyID[GSK_PAGEDOWN] = SDL_SCANCODE_PAGEDOWN;
	m_sdlKeyID[GSK_PAGEUP] = SDL_SCANCODE_PAGEUP;
	m_sdlKeyID[GSK_SPACE] = SDL_SCANCODE_SPACE;
	m_sdlKeyID[GSK_ENTER] = SDL_SCANCODE_RETURN;
	m_sdlKeyID[GSK_DELETE] = SDL_SCANCODE_DELETE;
	m_sdlKeyID[GSK_HOME] = SDL_SCANCODE_HOME;
	m_sdlKeyID[GSK_END] = SDL_SCANCODE_END;
	m_sdlKeyID[GSK_INSERT] = SDL_SCANCODE_INSERT;
	m_sdlKeyID[GSK_PAUSE] = SDL_SCANCODE_PAUSE;
	m_sdlKeyID[GSK_ESC] = SDL_SCANCODE_ESCAPE;
	m_sdlKeyID[GSK_BACK] = SDL_SCANCODE_BACKSPACE;
	m_sdlKeyID[GSK_TAB] = SDL_SCANCODE_TAB;
	m_sdlKeyID[GSK_PRINTSCREEN] = SDL_SCANCODE_PRINTSCREEN;
	m_sdlKeyID[GSK_SUBTRACT] = SDL_SCANCODE_MINUS;
	m_sdlKeyID[GSK_ADD] = SDL_SCANCODE_KP_PLUS; // this is messed up
	m_sdlKeyID[GSK_F1] = SDL_SCANCODE_F1;
	m_sdlKeyID[GSK_F2] = SDL_SCANCODE_F2;
	m_sdlKeyID[GSK_F3] = SDL_SCANCODE_F3;
	m_sdlKeyID[GSK_F4] = SDL_SCANCODE_F4;
	m_sdlKeyID[GSK_F5] = SDL_SCANCODE_F5;
	m_sdlKeyID[GSK_F6] = SDL_SCANCODE_F6;
	m_sdlKeyID[GSK_F7] = SDL_SCANCODE_F7;
	m_sdlKeyID[GSK_F8] = SDL_SCANCODE_F8;
	m_sdlKeyID[GSK_F9] = SDL_SCANCODE_F9;
	m_sdlKeyID[GSK_F10] = SDL_SCANCODE_F10;
	m_sdlKeyID[GSK_F11] = SDL_SCANCODE_F11;
	m_sdlKeyID[GSK_F12] = SDL_SCANCODE_F12;
	m_sdlKeyID[GSK_F13] = SDL_SCANCODE_F13;
	m_sdlKeyID[GSK_F14] = SDL_SCANCODE_F14;
	m_sdlKeyID[GSK_F15] = SDL_SCANCODE_F15;
	m_sdlKeyID[GSK_F16] = SDL_SCANCODE_F16;
	m_sdlKeyID[GSK_F17] = SDL_SCANCODE_F17;
	m_sdlKeyID[GSK_F18] = SDL_SCANCODE_F18;
	m_sdlKeyID[GSK_F19] = SDL_SCANCODE_F19;
	m_sdlKeyID[GSK_F20] = SDL_SCANCODE_F20;
	m_sdlKeyID[GSK_F21] = SDL_SCANCODE_F21;
	m_sdlKeyID[GSK_F22] = SDL_SCANCODE_F22;
	m_sdlKeyID[GSK_F23] = SDL_SCANCODE_F23;
	m_sdlKeyID[GSK_F24] = SDL_SCANCODE_F24;
	m_sdlKeyID[GSK_A] = SDL_SCANCODE_A;
	m_sdlKeyID[GSK_B] = SDL_SCANCODE_B;
	m_sdlKeyID[GSK_C] = SDL_SCANCODE_C;
	m_sdlKeyID[GSK_D] = SDL_SCANCODE_D;
	m_sdlKeyID[GSK_E] = SDL_SCANCODE_E;
	m_sdlKeyID[GSK_F] = SDL_SCANCODE_F;
	m_sdlKeyID[GSK_G] = SDL_SCANCODE_G;
	m_sdlKeyID[GSK_H] = SDL_SCANCODE_H;
	m_sdlKeyID[GSK_I] = SDL_SCANCODE_I;
	m_sdlKeyID[GSK_J] = SDL_SCANCODE_J;
	m_sdlKeyID[GSK_K] = SDL_SCANCODE_K;
	m_sdlKeyID[GSK_L] = SDL_SCANCODE_L;
	m_sdlKeyID[GSK_M] = SDL_SCANCODE_M;
	m_sdlKeyID[GSK_N] = SDL_SCANCODE_N;
	m_sdlKeyID[GSK_O] = SDL_SCANCODE_O;
	m_sdlKeyID[GSK_P] = SDL_SCANCODE_P;
	m_sdlKeyID[GSK_Q] = SDL_SCANCODE_Q;
	m_sdlKeyID[GSK_R] = SDL_SCANCODE_R;
	m_sdlKeyID[GSK_S] = SDL_SCANCODE_S;
	m_sdlKeyID[GSK_T] = SDL_SCANCODE_T;
	m_sdlKeyID[GSK_U] = SDL_SCANCODE_U;
	m_sdlKeyID[GSK_V] = SDL_SCANCODE_V;
	m_sdlKeyID[GSK_W] = SDL_SCANCODE_W;
	m_sdlKeyID[GSK_X] = SDL_SCANCODE_X;
	m_sdlKeyID[GSK_Y] = SDL_SCANCODE_Y;
	m_sdlKeyID[GSK_Z] = SDL_SCANCODE_Z;
	m_sdlKeyID[GSK_1] = SDL_SCANCODE_1;
	m_sdlKeyID[GSK_2] = SDL_SCANCODE_2;
	m_sdlKeyID[GSK_3] = SDL_SCANCODE_3;
	m_sdlKeyID[GSK_4] = SDL_SCANCODE_4;
	m_sdlKeyID[GSK_5] = SDL_SCANCODE_5;
	m_sdlKeyID[GSK_6] = SDL_SCANCODE_6;
	m_sdlKeyID[GSK_7] = SDL_SCANCODE_7;
	m_sdlKeyID[GSK_8] = SDL_SCANCODE_8;
	m_sdlKeyID[GSK_9] = SDL_SCANCODE_9;
	m_sdlKeyID[GSK_0] = SDL_SCANCODE_0;
	m_sdlKeyID[GSK_NUMPAD0] = SDL_SCANCODE_KP_0;
	m_sdlKeyID[GSK_NUMPAD1] = SDL_SCANCODE_KP_1;
	m_sdlKeyID[GSK_NUMPAD2] = SDL_SCANCODE_KP_2;
	m_sdlKeyID[GSK_NUMPAD3] = SDL_SCANCODE_KP_3;
	m_sdlKeyID[GSK_NUMPAD4] = SDL_SCANCODE_KP_4;
	m_sdlKeyID[GSK_NUMPAD5] = SDL_SCANCODE_KP_5;
	m_sdlKeyID[GSK_NUMPAD6] = SDL_SCANCODE_KP_6;
	m_sdlKeyID[GSK_NUMPAD7] = SDL_SCANCODE_KP_7;
	m_sdlKeyID[GSK_NUMPAD8] = SDL_SCANCODE_KP_8;
	m_sdlKeyID[GSK_NUMPAD9] = SDL_SCANCODE_KP_9;
	m_sdlKeyID[GSK_MINUS] = SDL_SCANCODE_KP_MINUS;
	m_sdlKeyID[GSK_PLUS] = SDL_SCANCODE_KP_PLUS;
	m_sdlKeyID[GSK_COMMA] = SDL_SCANCODE_COMMA;
	m_sdlKeyID[GSK_DOT] = SDL_SCANCODE_PERIOD;
	m_sdlKeyID[GSK_META] = SDL_SCANCODE_LGUI;

	// not used:
	m_sdlKeyID[GSK_RMOUSE] = SDL_NUM_SCANCODES;
	m_sdlKeyID[GSK_LMOUSE] = SDL_NUM_SCANCODES;
	m_sdlKeyID[GSK_MMOUSE] = SDL_NUM_SCANCODES;

	UpdateCursorPos();
	m_lastCursorPos = m_cursorPos;
}

bool SDLInput::Update()
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
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
	SDL_WarpMouseInWindow(NULL, static_cast<Uint16>(v2Pos.x), static_cast<Uint16>(v2Pos.y));
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
	return math::Vector2::ToVector2(m_cursorPos);
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
	return math::Vector2::ToVector2(GetMouseMove());
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
	return GLSDLVideo::m_mouseWheel;
}

std::string SDLInput::GetLastCharInput() const
{
	if (GLSDLVideo::m_lastCharInput.length() == 1)
	{
		char c = GLSDLVideo::m_lastCharInput[0];
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
			return ("");
		}
	}
	return GLSDLVideo::m_lastCharInput;
}

} // namespace gs2d
