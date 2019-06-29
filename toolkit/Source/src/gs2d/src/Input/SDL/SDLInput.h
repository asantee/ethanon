#ifndef GS2D_SDL_INPUT_H_
#define GS2D_SDL_INPUT_H_

#include "SDLJoystick.h"

namespace gs2d {

class SDLInput : public SDLJoystick
{
	math::Vector2i m_cursorPos;
	math::Vector2i m_lastCursorPos;

	KeyStateManager m_keyStates[GS_NUM_KEYS];
	SDL_Scancode m_sdlKeyID[GS_NUM_KEYS];
	Uint8 m_mouseBits;

	void UpdateCursorPos();
	bool IsKeyPressed(const GS_KEY key, const Uint8* keystate);

public:
	SDLInput(const bool showJoystickWarnings);

	bool IsKeyDown(const GS_KEY key) const;
	GS_KEY_STATE GetKeyState(const GS_KEY key) const;

	GS_KEY_STATE GetLeftClickState() const;
	GS_KEY_STATE GetRightClickState() const;
	GS_KEY_STATE GetMiddleClickState() const;

	math::Vector2i GetMouseMove() const;
	math::Vector2  GetMouseMoveF() const;

	math::Vector2 GetTouchPos(const unsigned int n, WindowPtr pWindow = WindowPtr()) const;
	GS_KEY_STATE  GetTouchState(const unsigned int n, WindowPtr pWindow = WindowPtr()) const;
	unsigned int GetMaxTouchCount() const;
	math::Vector2 GetTouchMove(const unsigned int n) const;

	bool SetCursorPosition(math::Vector2i v2Pos);
	bool SetCursorPositionF(math::Vector2 v2Pos);
	math::Vector2i GetCursorPosition(WindowPtr pWindow) const;
	math::Vector2  GetCursorPositionF(WindowPtr pWindow) const;

	float GetWheelState() const;

	bool Update();

	str_type::string GetLastCharInput() const;

	math::Vector3 GetAccelerometerData() const;
};

} // namespace gs2d

#endif
