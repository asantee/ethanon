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

	bool IsKeyDown(const GS_KEY key) const override;
	GS_KEY_STATE GetKeyState(const GS_KEY key) const override;

	GS_KEY_STATE GetLeftClickState() const override;
	GS_KEY_STATE GetRightClickState() const override;
	GS_KEY_STATE GetMiddleClickState() const override;

	math::Vector2i GetMouseMove() const override;
	math::Vector2  GetMouseMoveF() const override;

	math::Vector2 GetTouchPos(const unsigned int n, WindowPtr pWindow = WindowPtr()) const override;
	GS_KEY_STATE  GetTouchState(const unsigned int n, WindowPtr pWindow = WindowPtr()) const override;
	unsigned int GetMaxTouchCount() const override;
	math::Vector2 GetTouchMove(const unsigned int n) const override;

	bool SetCursorPosition(math::Vector2i v2Pos) override;
	bool SetCursorPositionF(math::Vector2 v2Pos) override;
	math::Vector2i GetCursorPosition(WindowPtr pWindow) const override;
	math::Vector2  GetCursorPositionF(WindowPtr pWindow) const override;

	float GetWheelState() const override;

	bool Update() override;

	std::string GetLastCharInput() const override;

	math::Vector3 GetAccelerometerData() const override;
};

} // namespace gs2d

#endif
