#ifndef GS2D_MOBILE_INPUT_H_
#define GS2D_MOBILE_INPUT_H_

#include "../Input.h"

#include "KeyStateManager.h"

#include <vector>

#include "../Platform/FileLogger.h"

namespace gs2d {

class MobileInput : public Input
{
protected:
	const unsigned int m_maxTouchCount;

	std::vector<math::Vector2> m_currentTouch;
	std::vector<math::Vector2> m_touchMove;
	std::vector<math::Vector2> m_lastTouch;

	std::vector<KeyStateManager> m_touchStates;
	Platform::FileLogger m_logger;

public:
	MobileInput(const unsigned int maxTouchCount);

	GS_KEY_STATE GetLeftClickState() const;
	GS_KEY_STATE GetRightClickState() const;
	GS_KEY_STATE GetMiddleClickState() const;

	math::Vector2i GetMouseMove() const;
	math::Vector2  GetMouseMoveF() const;

	bool SetCursorPosition(math::Vector2i v2Pos);
	bool SetCursorPositionF(math::Vector2 v2Pos);
	math::Vector2i GetCursorPosition(WindowPtr pWindow) const;
	math::Vector2  GetCursorPositionF(WindowPtr pWindow) const;

	float GetWheelState() const;

	bool Update();

	math::Vector2 GetTouchPos(const unsigned int n, WindowPtr pWindow) const;
	GS_KEY_STATE  GetTouchState(const unsigned int n, WindowPtr pWindow) const;
	unsigned int GetMaxTouchCount() const;
	math::Vector2 GetTouchMove(const unsigned int n) const;

	str_type::string GetLastCharInput() const;

	unsigned int GetMaxJoysticks() const;
	void ShowJoystickWarnings(const bool enable);
	bool IsShowingJoystickWarnings() const;
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
};

} // namespace gs2d

#endif
