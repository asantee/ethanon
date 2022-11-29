#ifndef GS2D_ANDROID_INPUT_H_
#define GS2D_ANDROID_INPUT_H_

#include <vector>

#include "../MobileInput.h"

#include "../../Platform/android/Platform.android.h"

namespace gs2d {

class AndroidInput : public MobileInput, public Platform::NativeCommandForwarder
{
	math::Vector3 m_accelerometer;
	const std::string *m_input;
	KeyStateManager m_keyStates[GS_NUM_KEYS];
	std::string m_keyName[GS_NUM_KEYS];
	std::size_t m_numJoysticks;
	std::size_t m_maxJoysticks;
	std::vector<std::string>     m_joyButtonsPressedList;
	std::vector<std::size_t>     m_joyNumButtons;
	std::vector<std::vector<KeyStateManager> > m_joyKeyStates;
	std::vector<std::vector<KeyStateManager> > m_joystickDpadStates;
	std::vector<math::Vector2> m_xy;
	std::vector<float> m_z;
	std::vector<float> m_rudder;

	static const std::string KEY_PRESSED_CMD;

	static const std::string DPAD_0_UP;
	static const std::string DPAD_1_DOWN;
	static const std::string DPAD_2_LEFT;
	static const std::string DPAD_3_RIGHT;

	static const std::size_t DPAD_KEY_UP;
	static const std::size_t DPAD_KEY_DOWN;
	static const std::size_t DPAD_KEY_LEFT;
	static const std::size_t DPAD_KEY_RIGHT;

	void UpdateKeys(const std::string& str);
	void UpdateKey(const std::string& str, const std::string& keyName, const GS_KEY key);
	void UpdateKey(const std::string& keyName, const GS_KEY key);

	void UpdateJoysticks();
	std::string AssembleJoystickSharedDataPath(const std::size_t j, const std::string& parameter);
	std::string AssembleJoystickAxisValueSharedDataPath(const std::size_t j, const std::string& axis);

public:
	AndroidInput(const unsigned int maxTouchCount, const std::string *input);

	std::string PullCommands();

	bool IsKeyDown(const GS_KEY key) const;
	GS_KEY_STATE GetKeyState(const GS_KEY key) const;

	bool Update();

	math::Vector3 GetAccelerometerData() const;

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

public:
	const static std::string KEY_STATE_DOWN;
	const static std::string KEY_STATE_UP;

	const static std::string KEY_CODE_UP;
	const static std::string KEY_CODE_DOWN;
	const static std::string KEY_CODE_LEFT;
	const static std::string KEY_CODE_RIGHT;
	const static std::string KEY_CODE_PAGE_UP;
	const static std::string KEY_CODE_PAGE_DOWN;
	const static std::string KEY_CODE_SPACE;
	const static std::string KEY_CODE_ENTER;
	const static std::string KEY_CODE_HOME;
	const static std::string KEY_CODE_INSERT;
	const static std::string KEY_CODE_ESCAPE;
	const static std::string KEY_CODE_TAB;
	const static std::string KEY_CODE_SHIFT;
	const static std::string KEY_CODE_ALT;
	const static std::string KEY_CODE_CTRL;

	const static std::string KEY_CODE_SHIFT_LEFT;
	const static std::string KEY_CODE_SHIFT_RIGHT;
	const static std::string KEY_CODE_ALT_LEFT;
	const static std::string KEY_CODE_ALT_RIGHT;
	const static std::string KEY_CODE_CTRL_LEFT;
	const static std::string KEY_CODE_CTRL_RIGHT;

	const static std::string KEY_CODE_F1;
	const static std::string KEY_CODE_F2;
	const static std::string KEY_CODE_F3;
	const static std::string KEY_CODE_F4;
	const static std::string KEY_CODE_F5;
	const static std::string KEY_CODE_F6;
	const static std::string KEY_CODE_F7;
	const static std::string KEY_CODE_F8;
	const static std::string KEY_CODE_F9;
	const static std::string KEY_CODE_F10;
	const static std::string KEY_CODE_F11;
	const static std::string KEY_CODE_F12;
	const static std::string KEY_CODE_A;
	const static std::string KEY_CODE_B;
	const static std::string KEY_CODE_C;
	const static std::string KEY_CODE_D;
	const static std::string KEY_CODE_E;
	const static std::string KEY_CODE_F;
	const static std::string KEY_CODE_G;
	const static std::string KEY_CODE_H;
	const static std::string KEY_CODE_I;
	const static std::string KEY_CODE_J;
	const static std::string KEY_CODE_K;
	const static std::string KEY_CODE_L;
	const static std::string KEY_CODE_M;
	const static std::string KEY_CODE_N;
	const static std::string KEY_CODE_O;
	const static std::string KEY_CODE_P;
	const static std::string KEY_CODE_Q;
	const static std::string KEY_CODE_R;
	const static std::string KEY_CODE_S;
	const static std::string KEY_CODE_T;
	const static std::string KEY_CODE_U;
	const static std::string KEY_CODE_V;
	const static std::string KEY_CODE_X;
	const static std::string KEY_CODE_Y;
	const static std::string KEY_CODE_Z;
	const static std::string KEY_CODE_W;
	const static std::string KEY_CODE_0;
	const static std::string KEY_CODE_1;
	const static std::string KEY_CODE_2;
	const static std::string KEY_CODE_3;
	const static std::string KEY_CODE_4;
	const static std::string KEY_CODE_5;
	const static std::string KEY_CODE_6;
	const static std::string KEY_CODE_7;
	const static std::string KEY_CODE_8;
	const static std::string KEY_CODE_9;

	const static std::string KEY_CODE_NUMPAD_0;
	const static std::string KEY_CODE_NUMPAD_1;
	const static std::string KEY_CODE_NUMPAD_2;
	const static std::string KEY_CODE_NUMPAD_3;
	const static std::string KEY_CODE_NUMPAD_4;
	const static std::string KEY_CODE_NUMPAD_5;
	const static std::string KEY_CODE_NUMPAD_6;
	const static std::string KEY_CODE_NUMPAD_7;
	const static std::string KEY_CODE_NUMPAD_8;
	const static std::string KEY_CODE_NUMPAD_9;

	const static std::string KEY_CODE_NUMPAD_ADD;
	const static std::string KEY_CODE_NUMPAD_MULTIPLY;
	const static std::string KEY_CODE_NUMPAD_SUBTRACT;
	const static std::string KEY_CODE_NUMPAD_DIVIDE;
	const static std::string KEY_CODE_NUMPAD_ENTER;
	const static std::string KEY_CODE_NUMPAD_DOT;
	const static std::string KEY_CODE_NUMPAD_COMMA;

	const static std::string KEY_CODE_PLUS;
	const static std::string KEY_CODE_MINUS;
	const static std::string KEY_CODE_BACKSLASH;
	const static std::string KEY_CODE_COMMA;
	const static std::string KEY_CODE_PERIOD;
	const static std::string KEY_CODE_SEMICOLON;
	const static std::string KEY_CODE_META_LEFT;
	const static std::string KEY_CODE_META_RIGHT;
	const static std::string KEY_CODE_GRAVE;
	const static std::string KEY_CODE_SLASH;
	const static std::string KEY_CODE_LEFT_BRACKET;
	const static std::string KEY_CODE_RIGHT_BRACKET;
	const static std::string KEY_CODE_EQUALS;
	const static std::string KEY_CODE_APOSTROPHE;
	const static std::string KEY_CODE_STAR;
};

typedef boost::shared_ptr<AndroidInput> AndroidInputPtr;

} // namespace gs2d

#endif
