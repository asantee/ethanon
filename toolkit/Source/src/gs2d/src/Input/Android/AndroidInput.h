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

	str_type::string PullCommands();

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
	const static str_type::string KEY_STATE_DOWN;
	const static str_type::string KEY_STATE_UP;

	const static str_type::string KEY_CODE_UP;
	const static str_type::string KEY_CODE_DOWN;
	const static str_type::string KEY_CODE_LEFT;
	const static str_type::string KEY_CODE_RIGHT;
	const static str_type::string KEY_CODE_PAGE_UP;
	const static str_type::string KEY_CODE_PAGE_DOWN;
	const static str_type::string KEY_CODE_SPACE;
	const static str_type::string KEY_CODE_ENTER;
	const static str_type::string KEY_CODE_HOME;
	const static str_type::string KEY_CODE_INSERT;
	const static str_type::string KEY_CODE_ESCAPE;
	const static str_type::string KEY_CODE_TAB;
	const static str_type::string KEY_CODE_SHIFT;
	const static str_type::string KEY_CODE_ALT;
	const static str_type::string KEY_CODE_CTRL;
	const static str_type::string KEY_CODE_F1;
	const static str_type::string KEY_CODE_F2;
	const static str_type::string KEY_CODE_F3;
	const static str_type::string KEY_CODE_F4;
	const static str_type::string KEY_CODE_F5;
	const static str_type::string KEY_CODE_F6;
	const static str_type::string KEY_CODE_F7;
	const static str_type::string KEY_CODE_F8;
	const static str_type::string KEY_CODE_F9;
	const static str_type::string KEY_CODE_F10;
	const static str_type::string KEY_CODE_F11;
	const static str_type::string KEY_CODE_F12;
	const static str_type::string KEY_CODE_A;
	const static str_type::string KEY_CODE_B;
	const static str_type::string KEY_CODE_C;
	const static str_type::string KEY_CODE_D;
	const static str_type::string KEY_CODE_E;
	const static str_type::string KEY_CODE_F;
	const static str_type::string KEY_CODE_G;
	const static str_type::string KEY_CODE_H;
	const static str_type::string KEY_CODE_I;
	const static str_type::string KEY_CODE_J;
	const static str_type::string KEY_CODE_K;
	const static str_type::string KEY_CODE_L;
	const static str_type::string KEY_CODE_M;
	const static str_type::string KEY_CODE_N;
	const static str_type::string KEY_CODE_O;
	const static str_type::string KEY_CODE_P;
	const static str_type::string KEY_CODE_Q;
	const static str_type::string KEY_CODE_R;
	const static str_type::string KEY_CODE_S;
	const static str_type::string KEY_CODE_T;
	const static str_type::string KEY_CODE_U;
	const static str_type::string KEY_CODE_V;
	const static str_type::string KEY_CODE_X;
	const static str_type::string KEY_CODE_Y;
	const static str_type::string KEY_CODE_Z;
	const static str_type::string KEY_CODE_W;
	const static str_type::string KEY_CODE_0;
	const static str_type::string KEY_CODE_1;
	const static str_type::string KEY_CODE_2;
	const static str_type::string KEY_CODE_3;
	const static str_type::string KEY_CODE_4;
	const static str_type::string KEY_CODE_5;
	const static str_type::string KEY_CODE_6;
	const static str_type::string KEY_CODE_7;
	const static str_type::string KEY_CODE_8;
	const static str_type::string KEY_CODE_9;
};

typedef boost::shared_ptr<AndroidInput> AndroidInputPtr;

} // namespace gs2d

#endif
