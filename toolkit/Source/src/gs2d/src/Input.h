#ifndef GS2D_INPUT_H_
#define GS2D_INPUT_H_

#include "Math/GameMath.h"
#include "Window.h"

#include "Input/KeyStateManager.h"

namespace gs2d {

class Application;

enum GS_KEY
{
	GSK_UP = 0,
	GSK_DOWN = 1,
	GSK_LEFT = 2,
	GSK_RIGHT,
	GSK_PAGEDOWN,
	GSK_PAGEUP,
	GSK_SPACE,
	GSK_ENTER,
	GSK_DELETE,
	GSK_HOME,
	GSK_END,
	GSK_INSERT,
	GSK_PAUSE,
	GSK_ESC,
	GSK_BACK,
	GSK_TAB,
	GSK_PRINTSCREEN,
	GSK_SUBTRACT,
	GSK_ADD,
	GSK_F1,
	GSK_F2,
	GSK_F3,
	GSK_F4,
	GSK_F5,
	GSK_F6,
	GSK_F7,
	GSK_F8,
	GSK_F9,
	GSK_F10,
	GSK_F11,
	GSK_F12,
	GSK_F13,
	GSK_F14,
	GSK_F15,
	GSK_F16,
	GSK_F17,
	GSK_F18,
	GSK_F19,
	GSK_F20,
	GSK_F21,
	GSK_F22,
	GSK_F23,
	GSK_F24,
	GSK_A,
	GSK_B,
	GSK_C,
	GSK_D,
	GSK_E,
	GSK_F,
	GSK_G,
	GSK_H,
	GSK_I,
	GSK_J,
	GSK_K,
	GSK_L,
	GSK_M,
	GSK_N,
	GSK_O,
	GSK_P,
	GSK_Q,
	GSK_R,
	GSK_S,
	GSK_T,
	GSK_U,
	GSK_V,
	GSK_W,
	GSK_X,
	GSK_Y,
	GSK_Z,
	GSK_0,
	GSK_1,
	GSK_2,
	GSK_3,
	GSK_4,
	GSK_5,
	GSK_6,
	GSK_7,
	GSK_8,
	GSK_9,
	GSK_MINUS,
	GSK_PLUS,
	GSK_COMMA,
	GSK_DOT,
	GSK_CTRL,
	GSK_ALT,
	GSK_SHIFT,
	GSK_RMOUSE,
	GSK_LMOUSE,
	GSK_MMOUSE,
	GSK_NUMPAD0,
	GSK_NUMPAD1,
	GSK_NUMPAD2,
	GSK_NUMPAD3,
	GSK_NUMPAD4,
	GSK_NUMPAD5,
	GSK_NUMPAD6,
	GSK_NUMPAD7,
	GSK_NUMPAD8,
	GSK_NUMPAD9,
	GSK_META,
	GS_NUM_KEYS,
	GSK_NO_KEY,
};

#define GSK_BACKSPACE GSK_BACK
#define GSK_MAC_DELETE GSK_BACK
#define GSK_RETURN GSK_ENTER

enum GS_JOYSTICK_STATUS
{
	GSJS_DETECTED = 0,
	GSJS_NOTDETECTED = 1,
	GSJS_INVALID = 2,
};

enum GS_JOYSTICK_BUTTON
{
	GSB_01 = 0,
	GSB_02 = 1,
	GSB_03 = 2,
	GSB_04,
	GSB_05,
	GSB_06,
	GSB_07,
	GSB_08,
	GSB_09,
	GSB_10,
	GSB_11,
	GSB_12,
	GSB_13,
	GSB_14,
	GSB_15,
	GSB_16,
	GSB_17,
	GSB_18,
	GSB_19,
	GSB_20,
	GSB_21,
	GSB_22,
	GSB_23,
	GSB_24,
	GSB_25,
	GSB_26,
	GSB_27,
	GSB_28,
	GSB_29,
	GSB_30,
	GSB_31,
	GSB_32,
	GSB_MAX_BUTTONS,
	GSB_UP,
	GSB_DOWN,
	GSB_LEFT,
	GSB_RIGHT,
	GSB_NUM_BUTTONS,
	GSB_NONE,
};

const math::Vector2 GS_NO_TOUCH(-1, -1);

/**
 * \brief Abstracts all Input related methods
 *
 * This class can be used to handle keyboard, mouse and joystick
 * events. After initialized, the method Input::Update must be
 * called at every frame to update the current states.
 */
class Input
{
public:
	virtual bool IsKeyDown(const GS_KEY key) const = 0;
	virtual GS_KEY_STATE GetKeyState(const GS_KEY key) const = 0;

	virtual GS_KEY_STATE GetLeftClickState() const = 0;
	virtual GS_KEY_STATE GetRightClickState() const = 0;
	virtual GS_KEY_STATE GetMiddleClickState() const = 0;

	virtual math::Vector2i GetMouseMove() const = 0;
	virtual math::Vector2  GetMouseMoveF() const = 0;

	virtual math::Vector2 GetTouchPos(const unsigned int n, WindowPtr pWindow = WindowPtr()) const = 0;
	virtual GS_KEY_STATE  GetTouchState(const unsigned int n, WindowPtr pWindow = WindowPtr()) const = 0;
	virtual unsigned int GetMaxTouchCount() const = 0;
	virtual math::Vector2 GetTouchMove(const unsigned int n) const = 0;

	virtual bool SetCursorPosition(math::Vector2i v2Pos) = 0;
	virtual bool SetCursorPositionF(math::Vector2 v2Pos) = 0;
	virtual math::Vector2i GetCursorPosition(WindowPtr pWindow) const = 0;
	virtual math::Vector2  GetCursorPositionF(WindowPtr pWindow) const = 0;

	virtual unsigned int GetMaxJoysticks() const = 0;
	virtual float GetWheelState() const = 0;

	virtual bool Update() = 0;

	virtual void ShowJoystickWarnings(const bool enable) = 0;
	virtual bool IsShowingJoystickWarnings() const = 0;

	virtual str_type::string GetLastCharInput() const = 0;

	virtual GS_KEY_STATE GetJoystickButtonState(const unsigned int id, const GS_JOYSTICK_BUTTON key) const = 0;
	virtual bool IsJoystickButtonDown(const unsigned int id, const GS_JOYSTICK_BUTTON key) const = 0;
	virtual bool DetectJoysticks() = 0;
	virtual GS_JOYSTICK_STATUS GetJoystickStatus(const unsigned int id) const = 0;
	virtual unsigned int GetNumJoyButtons(const unsigned int id) const = 0;
	virtual math::Vector2 GetJoystickXY(const unsigned int id) const = 0;
	virtual float GetJoystickZ(const unsigned int id) const = 0;
	virtual float GetJoystickRudder(const unsigned int id) const = 0;
	virtual math::Vector2 GetJoystickUV(const unsigned int id) const = 0;
	virtual GS_JOYSTICK_BUTTON GetFirstButtonDown(const unsigned int id) const = 0;
	virtual unsigned int GetNumJoysticks() const = 0;

	virtual math::Vector3 GetAccelerometerData() const = 0;
};

typedef boost::shared_ptr<Input> InputPtr;
typedef boost::weak_ptr<Input> InputWeakPtr;

/// Instantiates an Input object
GS2D_API InputPtr CreateInput(boost::any data, const bool showJoystickWarnings);

} // namespace gs2d

#endif