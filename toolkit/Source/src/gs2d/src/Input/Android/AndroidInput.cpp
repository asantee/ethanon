#include "AndroidInput.h"

#include "../../Application.h"

#include <string>
#include <sstream>

#include <stdio.h>

namespace gs2d {

using namespace math;

const std::string AndroidInput::KEY_STATE_DOWN = "down";
const std::string AndroidInput::KEY_STATE_UP = "up";

const std::string AndroidInput::KEY_CODE_UP = "ethanon.system.keyboard.up";
const std::string AndroidInput::KEY_CODE_DOWN = "ethanon.system.keyboard.down";
const std::string AndroidInput::KEY_CODE_LEFT = "ethanon.system.keyboard.left";
const std::string AndroidInput::KEY_CODE_RIGHT = "ethanon.system.keyboard.right";
const std::string AndroidInput::KEY_CODE_PAGE_UP = "ethanon.system.keyboard.pageUp";
const std::string AndroidInput::KEY_CODE_PAGE_DOWN = "ethanon.system.keyboard.pageDown";
const std::string AndroidInput::KEY_CODE_SPACE = "ethanon.system.keyboard.space";
const std::string AndroidInput::KEY_CODE_ENTER = "ethanon.system.keyboard.enter";
const std::string AndroidInput::KEY_CODE_HOME = "ethanon.system.keyboard.home";
const std::string AndroidInput::KEY_CODE_INSERT = "ethanon.system.keyboard.insert";
const std::string AndroidInput::KEY_CODE_ESCAPE = "ethanon.system.keyboard.esc";
const std::string AndroidInput::KEY_CODE_TAB = "ethanon.system.keyboard.tab";
const std::string AndroidInput::KEY_CODE_SHIFT = "ethanon.system.keyboard.shift";
const std::string AndroidInput::KEY_CODE_ALT = "ethanon.system.keyboard.alt";
const std::string AndroidInput::KEY_CODE_CTRL = "ethanon.system.keyboard.ctrl";

const std::string AndroidInput::KEY_CODE_SHIFT_LEFT  = "ethanon.system.keyboard.shiftLeft";
const std::string AndroidInput::KEY_CODE_SHIFT_RIGHT = "ethanon.system.keyboard.shiftRight";
const std::string AndroidInput::KEY_CODE_ALT_LEFT  = "ethanon.system.keyboard.altLeft";
const std::string AndroidInput::KEY_CODE_ALT_RIGHT = "ethanon.system.keyboard.altRight";
const std::string AndroidInput::KEY_CODE_CTRL_LEFT  = "ethanon.system.keyboard.ctrlLeft";
const std::string AndroidInput::KEY_CODE_CTRL_RIGHT = "ethanon.system.keyboard.ctrlRight";

const std::string AndroidInput::KEY_CODE_F1 = "ethanon.system.keyboard.f1";
const std::string AndroidInput::KEY_CODE_F2 = "ethanon.system.keyboard.f2";
const std::string AndroidInput::KEY_CODE_F3 = "ethanon.system.keyboard.f3";
const std::string AndroidInput::KEY_CODE_F4 = "ethanon.system.keyboard.f4";
const std::string AndroidInput::KEY_CODE_F5 = "ethanon.system.keyboard.f5";
const std::string AndroidInput::KEY_CODE_F6 = "ethanon.system.keyboard.f6";
const std::string AndroidInput::KEY_CODE_F7 = "ethanon.system.keyboard.f7";
const std::string AndroidInput::KEY_CODE_F8 = "ethanon.system.keyboard.f8";
const std::string AndroidInput::KEY_CODE_F9 = "ethanon.system.keyboard.f9";
const std::string AndroidInput::KEY_CODE_F10 = "ethanon.system.keyboard.f10";
const std::string AndroidInput::KEY_CODE_F11 = "ethanon.system.keyboard.f11";
const std::string AndroidInput::KEY_CODE_F12 = "ethanon.system.keyboard.f12";
const std::string AndroidInput::KEY_CODE_A = "ethanon.system.keyboard.a";
const std::string AndroidInput::KEY_CODE_B = "ethanon.system.keyboard.b";
const std::string AndroidInput::KEY_CODE_C = "ethanon.system.keyboard.c";
const std::string AndroidInput::KEY_CODE_D = "ethanon.system.keyboard.d";
const std::string AndroidInput::KEY_CODE_E = "ethanon.system.keyboard.e";
const std::string AndroidInput::KEY_CODE_F = "ethanon.system.keyboard.f";
const std::string AndroidInput::KEY_CODE_G = "ethanon.system.keyboard.g";
const std::string AndroidInput::KEY_CODE_H = "ethanon.system.keyboard.h";
const std::string AndroidInput::KEY_CODE_I = "ethanon.system.keyboard.i";
const std::string AndroidInput::KEY_CODE_J = "ethanon.system.keyboard.j";
const std::string AndroidInput::KEY_CODE_K = "ethanon.system.keyboard.k";
const std::string AndroidInput::KEY_CODE_L = "ethanon.system.keyboard.l";
const std::string AndroidInput::KEY_CODE_M = "ethanon.system.keyboard.m";
const std::string AndroidInput::KEY_CODE_N = "ethanon.system.keyboard.n";
const std::string AndroidInput::KEY_CODE_O = "ethanon.system.keyboard.o";
const std::string AndroidInput::KEY_CODE_P = "ethanon.system.keyboard.p";
const std::string AndroidInput::KEY_CODE_Q = "ethanon.system.keyboard.q";
const std::string AndroidInput::KEY_CODE_R = "ethanon.system.keyboard.r";
const std::string AndroidInput::KEY_CODE_S = "ethanon.system.keyboard.s";
const std::string AndroidInput::KEY_CODE_T = "ethanon.system.keyboard.t";
const std::string AndroidInput::KEY_CODE_U = "ethanon.system.keyboard.u";
const std::string AndroidInput::KEY_CODE_V = "ethanon.system.keyboard.v";
const std::string AndroidInput::KEY_CODE_X = "ethanon.system.keyboard.x";
const std::string AndroidInput::KEY_CODE_Y = "ethanon.system.keyboard.y";
const std::string AndroidInput::KEY_CODE_Z = "ethanon.system.keyboard.z";
const std::string AndroidInput::KEY_CODE_W = "ethanon.system.keyboard.w";
const std::string AndroidInput::KEY_CODE_0 = "ethanon.system.keyboard.0";
const std::string AndroidInput::KEY_CODE_1 = "ethanon.system.keyboard.1";
const std::string AndroidInput::KEY_CODE_2 = "ethanon.system.keyboard.2";
const std::string AndroidInput::KEY_CODE_3 = "ethanon.system.keyboard.3";
const std::string AndroidInput::KEY_CODE_4 = "ethanon.system.keyboard.4";
const std::string AndroidInput::KEY_CODE_5 = "ethanon.system.keyboard.5";
const std::string AndroidInput::KEY_CODE_6 = "ethanon.system.keyboard.6";
const std::string AndroidInput::KEY_CODE_7 = "ethanon.system.keyboard.7";
const std::string AndroidInput::KEY_CODE_8 = "ethanon.system.keyboard.8";
const std::string AndroidInput::KEY_CODE_9 = "ethanon.system.keyboard.9";

const std::string AndroidInput::KEY_CODE_NUMPAD_0 = "ethanon.system.keyboard.numpad.0";
const std::string AndroidInput::KEY_CODE_NUMPAD_1 = "ethanon.system.keyboard.numpad.1";
const std::string AndroidInput::KEY_CODE_NUMPAD_2 = "ethanon.system.keyboard.numpad.2";
const std::string AndroidInput::KEY_CODE_NUMPAD_3 = "ethanon.system.keyboard.numpad.3";
const std::string AndroidInput::KEY_CODE_NUMPAD_4 = "ethanon.system.keyboard.numpad.4";
const std::string AndroidInput::KEY_CODE_NUMPAD_5 = "ethanon.system.keyboard.numpad.5";
const std::string AndroidInput::KEY_CODE_NUMPAD_6 = "ethanon.system.keyboard.numpad.6";
const std::string AndroidInput::KEY_CODE_NUMPAD_7 = "ethanon.system.keyboard.numpad.7";
const std::string AndroidInput::KEY_CODE_NUMPAD_8 = "ethanon.system.keyboard.numpad.8";
const std::string AndroidInput::KEY_CODE_NUMPAD_9 = "ethanon.system.keyboard.numpad.9";

const std::string AndroidInput::KEY_CODE_NUMPAD_ADD = "ethanon.system.keyboard.numpad.add";
const std::string AndroidInput::KEY_CODE_NUMPAD_MULTIPLY = "ethanon.system.keyboard.numpad.multiply";
const std::string AndroidInput::KEY_CODE_NUMPAD_SUBTRACT = "ethanon.system.keyboard.numpad.subtract";
const std::string AndroidInput::KEY_CODE_NUMPAD_DIVIDE = "ethanon.system.keyboard.numpad.divide";
const std::string AndroidInput::KEY_CODE_NUMPAD_ENTER = "ethanon.system.keyboard.numpad.enter";
const std::string AndroidInput::KEY_CODE_NUMPAD_DOT = "ethanon.system.keyboard.numpad.dot";
const std::string AndroidInput::KEY_CODE_NUMPAD_COMMA = "ethanon.system.keyboard.numpad.comma";

const std::string AndroidInput::KEY_CODE_PLUS = "ethanon.system.keyboard.plus";
const std::string AndroidInput::KEY_CODE_MINUS = "ethanon.system.keyboard.minus";
const std::string AndroidInput::KEY_CODE_BACKSLASH = "ethanon.system.keyboard.backslash";
const std::string AndroidInput::KEY_CODE_COMMA = "ethanon.system.keyboard.comma";
const std::string AndroidInput::KEY_CODE_PERIOD = "ethanon.system.keyboard.period";
const std::string AndroidInput::KEY_CODE_SEMICOLON = "ethanon.system.keyboard.semicolon";
const std::string AndroidInput::KEY_CODE_META_LEFT = "ethanon.system.keyboard.metaLeft";
const std::string AndroidInput::KEY_CODE_META_RIGHT = "ethanon.system.keyboard.metaRight";
const std::string AndroidInput::KEY_CODE_GRAVE = "ethanon.system.keyboard.grave";
const std::string AndroidInput::KEY_CODE_SLASH = "ethanon.system.keyboard.slash";
const std::string AndroidInput::KEY_CODE_LEFT_BRACKET = "ethanon.system.keyboard.leftBracket";
const std::string AndroidInput::KEY_CODE_RIGHT_BRACKET = "ethanon.system.keyboard.rightBracket";
const std::string AndroidInput::KEY_CODE_EQUALS = "ethanon.system.keyboard.equals";
const std::string AndroidInput::KEY_CODE_APOSTROPHE = "ethanon.system.keyboard.apostrophe";
const std::string AndroidInput::KEY_CODE_STAR = "ethanon.system.keyboard.star";

InputPtr CreateInput(const bool showJoystickWarnings, std::string* inputSource)
{
	return InputPtr(new AndroidInput(5, inputSource));
}

const std::string AndroidInput::KEY_PRESSED_CMD = "key_pressed ";
const std::string AndroidInput::DPAD_0_UP    = "b-2;";
const std::string AndroidInput::DPAD_1_DOWN  = "b-3;";
const std::string AndroidInput::DPAD_2_LEFT  = "b-4;";
const std::string AndroidInput::DPAD_3_RIGHT = "b-5;";

const std::size_t AndroidInput::DPAD_KEY_UP = 0;
const std::size_t AndroidInput::DPAD_KEY_DOWN = 1;
const std::size_t AndroidInput::DPAD_KEY_LEFT = 2;
const std::size_t AndroidInput::DPAD_KEY_RIGHT = 3;

AndroidInput::AndroidInput(const unsigned int maxTouchCount, const std::string *input) :
	MobileInput(maxTouchCount),
	m_input(input),
	m_numJoysticks(0),
	m_maxJoysticks(0),
	m_joyNumButtons(0)
{
	// TODO/TO-DO: implement other keys...
	m_keyName[GSK_BACK]  = "back";
	m_keyName[GSK_PAUSE] = "pause";
}

std::string AndroidInput::PullCommands()
{
	std::string inputCommands;
	ForwardCommands(inputCommands);
	return inputCommands;
}

bool AndroidInput::IsKeyDown(const GS_KEY key) const
{
	const GS_KEY_STATE state = m_keyStates[key].GetCurrentState();
	return (state == GSKS_HIT || state == GSKS_DOWN);
}

GS_KEY_STATE AndroidInput::GetKeyState(const GS_KEY key) const
{
	return m_keyStates[key].GetCurrentState();
}

double ReadValue(std::stringstream& ss)
{
	double r;
	ss >> r;
	return r;
}

double ReadJoystickValue(const std::string& key)
{
	std::stringstream ss; ss << gs2d::Application::SharedData.Get(key);
	double r = 0;
	if (!ss.str().empty())
	{
		ss >> r;
	}
	return r;
}

unsigned int ReadValuePairs(std::stringstream& stream, std::vector<Vector2>& out, unsigned int maxPairs)
{
	unsigned int numPairs = Min(static_cast<unsigned int>(ReadValue(stream)), maxPairs);
	for (unsigned int t=0; t<maxPairs; t++)
	{
		if (!stream.eof() && t < numPairs)
		{
			out[t].x = static_cast<float>(ReadValue(stream));
			out[t].y = static_cast<float>(ReadValue(stream));
		}
		else
		{
			out[t] = GS_NO_TOUCH;
		}
	}
	return numPairs;
}

void AndroidInput::UpdateKeys(const std::string& str)
{
	UpdateKey(str, KEY_PRESSED_CMD + m_keyName[GSK_BACK],  GSK_BACK);
	UpdateKey(str, KEY_PRESSED_CMD + m_keyName[GSK_PAUSE], GSK_PAUSE);

	UpdateKey(KEY_CODE_UP, GSK_UP);
	UpdateKey(KEY_CODE_DOWN, GSK_DOWN);
	UpdateKey(KEY_CODE_LEFT, GSK_LEFT);
	UpdateKey(KEY_CODE_RIGHT, GSK_RIGHT);
	UpdateKey(KEY_CODE_PAGE_UP, GSK_PAGEUP);
	UpdateKey(KEY_CODE_PAGE_DOWN, GSK_PAGEDOWN);
	UpdateKey(KEY_CODE_SPACE, GSK_SPACE);
	UpdateKey(KEY_CODE_ENTER, GSK_ENTER);
	UpdateKey(KEY_CODE_HOME, GSK_HOME);
	UpdateKey(KEY_CODE_INSERT, GSK_INSERT);
	UpdateKey(KEY_CODE_ESCAPE, GSK_ESC);
	UpdateKey(KEY_CODE_TAB, GSK_TAB);
	UpdateKey(KEY_CODE_SHIFT, GSK_SHIFT);
	UpdateKey(KEY_CODE_ALT, GSK_ALT);
	UpdateKey(KEY_CODE_CTRL, GSK_CTRL);

	UpdateKey(KEY_CODE_SHIFT_LEFT, GSK_L_SHIFT);
	UpdateKey(KEY_CODE_SHIFT_RIGHT, GSK_R_SHIFT);
	UpdateKey(KEY_CODE_ALT_LEFT, GSK_L_ALT);
	UpdateKey(KEY_CODE_ALT_RIGHT, GSK_R_ALT);
	UpdateKey(KEY_CODE_CTRL_LEFT, GSK_L_CTRL);
	UpdateKey(KEY_CODE_CTRL_RIGHT, GSK_R_CTRL);

	UpdateKey(KEY_CODE_F1, GSK_F1);
	UpdateKey(KEY_CODE_F2, GSK_F2);
	UpdateKey(KEY_CODE_F3, GSK_F3);
	UpdateKey(KEY_CODE_F4, GSK_F4);
	UpdateKey(KEY_CODE_F5, GSK_F5);
	UpdateKey(KEY_CODE_F6, GSK_F6);
	UpdateKey(KEY_CODE_F7, GSK_F7);
	UpdateKey(KEY_CODE_F8, GSK_F8);
	UpdateKey(KEY_CODE_F9, GSK_F9);
	UpdateKey(KEY_CODE_F10, GSK_F10);
	UpdateKey(KEY_CODE_F11, GSK_F11);
	UpdateKey(KEY_CODE_F12, GSK_F12);
	UpdateKey(KEY_CODE_A, GSK_A);
	UpdateKey(KEY_CODE_B, GSK_B);
	UpdateKey(KEY_CODE_C, GSK_C);
	UpdateKey(KEY_CODE_D, GSK_D);
	UpdateKey(KEY_CODE_E, GSK_E);
	UpdateKey(KEY_CODE_F, GSK_F);
	UpdateKey(KEY_CODE_G, GSK_G);
	UpdateKey(KEY_CODE_H, GSK_H);
	UpdateKey(KEY_CODE_I, GSK_I);
	UpdateKey(KEY_CODE_J, GSK_J);
	UpdateKey(KEY_CODE_K, GSK_K);
	UpdateKey(KEY_CODE_L, GSK_L);
	UpdateKey(KEY_CODE_M, GSK_M);
	UpdateKey(KEY_CODE_N, GSK_N);
	UpdateKey(KEY_CODE_O, GSK_O);
	UpdateKey(KEY_CODE_P, GSK_P);
	UpdateKey(KEY_CODE_Q, GSK_Q);
	UpdateKey(KEY_CODE_R, GSK_R);
	UpdateKey(KEY_CODE_S, GSK_S);
	UpdateKey(KEY_CODE_T, GSK_T);
	UpdateKey(KEY_CODE_U, GSK_U);
	UpdateKey(KEY_CODE_V, GSK_V);
	UpdateKey(KEY_CODE_X, GSK_X);
	UpdateKey(KEY_CODE_Y, GSK_Y);
	UpdateKey(KEY_CODE_Z, GSK_Z);
	UpdateKey(KEY_CODE_W, GSK_W);
	UpdateKey(KEY_CODE_0, GSK_0);
	UpdateKey(KEY_CODE_1, GSK_1);
	UpdateKey(KEY_CODE_2, GSK_2);
	UpdateKey(KEY_CODE_3, GSK_3);
	UpdateKey(KEY_CODE_4, GSK_4);
	UpdateKey(KEY_CODE_5, GSK_5);
	UpdateKey(KEY_CODE_6, GSK_6);
	UpdateKey(KEY_CODE_7, GSK_7);
	UpdateKey(KEY_CODE_8, GSK_8);
	UpdateKey(KEY_CODE_9, GSK_9);

	UpdateKey(KEY_CODE_NUMPAD_0, GSK_NUMPAD0);
	UpdateKey(KEY_CODE_NUMPAD_1, GSK_NUMPAD1);
	UpdateKey(KEY_CODE_NUMPAD_2, GSK_NUMPAD2);
	UpdateKey(KEY_CODE_NUMPAD_3, GSK_NUMPAD3);
	UpdateKey(KEY_CODE_NUMPAD_4, GSK_NUMPAD4);
	UpdateKey(KEY_CODE_NUMPAD_5, GSK_NUMPAD5);
	UpdateKey(KEY_CODE_NUMPAD_6, GSK_NUMPAD6);
	UpdateKey(KEY_CODE_NUMPAD_7, GSK_NUMPAD7);
	UpdateKey(KEY_CODE_NUMPAD_8, GSK_NUMPAD8);
	UpdateKey(KEY_CODE_NUMPAD_9, GSK_NUMPAD9);

	UpdateKey(KEY_CODE_NUMPAD_ADD, GSK_ADD);
	//UpdateKey(KEY_CODE_NUMPAD_MULTIPLY, );
	UpdateKey(KEY_CODE_NUMPAD_SUBTRACT, GSK_SUBTRACT);
	UpdateKey(KEY_CODE_NUMPAD_DIVIDE, GSK_SLASH);
	UpdateKey(KEY_CODE_NUMPAD_ENTER, GSK_ENTER);
	UpdateKey(KEY_CODE_NUMPAD_DOT, GSK_DOT);
	//UpdateKey(KEY_CODE_NUMPAD_COMMA, GSK_COMMA);

	UpdateKey(KEY_CODE_PLUS, GSK_ADD);
	UpdateKey(KEY_CODE_MINUS, GSK_SUBTRACT);
	UpdateKey(KEY_CODE_BACKSLASH, GSK_BACKSLASH);
	UpdateKey(KEY_CODE_COMMA, GSK_COMMA);
	UpdateKey(KEY_CODE_PERIOD, GSK_PERIOD);
	UpdateKey(KEY_CODE_SEMICOLON, GSK_SEMICOLON);
	UpdateKey(KEY_CODE_META_LEFT, GSK_L_META);
	UpdateKey(KEY_CODE_META_RIGHT, GSK_R_META);
	UpdateKey(KEY_CODE_GRAVE, GSK_GRAVE);
	UpdateKey(KEY_CODE_SLASH, GSK_SLASH);
	UpdateKey(KEY_CODE_LEFT_BRACKET, GSK_LEFTBRACKET);
	UpdateKey(KEY_CODE_RIGHT_BRACKET, GSK_RIGHTBRACKET);
	UpdateKey(KEY_CODE_EQUALS, GSK_EQUALS);
	UpdateKey(KEY_CODE_APOSTROPHE, GSK_APOSTROPHE);
	//UpdateKey(KEY_CODE_STAR, );
}

void AndroidInput::UpdateKey(const std::string& str, const std::string& keyName, const GS_KEY key)
{
	m_keyStates[key].Update(strstr(str.c_str(), keyName.c_str()) != NULL);
}

void AndroidInput::UpdateKey(const std::string& keyName, const GS_KEY key)
{
	m_keyStates[key].Update(gs2d::Application::SharedData.Get(keyName) == KEY_STATE_DOWN);
}

bool AndroidInput::Update()
{
	std::stringstream stream;
	stream << *m_input;
	ReadValuePairs(stream, m_currentTouch, m_maxTouchCount);
	m_accelerometer.x = static_cast<float>(ReadValue(stream));
	m_accelerometer.y = static_cast<float>(ReadValue(stream));
	m_accelerometer.z = static_cast<float>(ReadValue(stream));

	UpdateJoysticks();
	UpdateKeys(*m_input);
	MobileInput::Update();
	return true;
}

math::Vector3 AndroidInput::GetAccelerometerData() const
{
	return m_accelerometer;
}

void AndroidInput::UpdateJoysticks()
{
	const std::size_t oldNumJoysticks = m_numJoysticks;
	m_numJoysticks = static_cast<std::size_t>(ReadJoystickValue("ethanon.system.numJoysticks"));
	m_maxJoysticks = static_cast<std::size_t>(ReadJoystickValue("ethanon.system.maxJoysticks"));

	// update internal joystick data
	if (oldNumJoysticks != m_numJoysticks)
	{
		m_joyButtonsPressedList.resize(m_numJoysticks);
		m_joyNumButtons.resize(m_numJoysticks, 0);
		m_joyKeyStates.resize(m_numJoysticks);
		m_joystickDpadStates.resize(m_numJoysticks);
		m_xy.resize(m_numJoysticks);
		m_z.resize(m_numJoysticks);
		m_rudder.resize(m_numJoysticks);
		for (std::size_t j = 0; j < m_numJoysticks; j++)
		{
			m_joystickDpadStates[j].resize(4);
			m_z[j] = m_rudder[j] = 0.0f;
		}
	}

	// update joystick buttons
	for (std::size_t j = 0; j < m_numJoysticks; j++)
	{
		m_joyNumButtons[j]         = ReadJoystickValue(AssembleJoystickSharedDataPath(j, "numButtons"));
		m_joyButtonsPressedList[j] = gs2d::Application::SharedData.Get(AssembleJoystickSharedDataPath(j, "buttonPressedList"));

		if (m_joyNumButtons[j] != m_joyKeyStates[j].size())
		{
			m_joyKeyStates[j].resize(m_joyNumButtons[j]);
		}

		const char* joyButtonPressedList = m_joyButtonsPressedList[j].c_str();
		for (std::size_t b = 0; b < m_joyNumButtons[j]; b++)
		{
			std::stringstream ss; ss << "b" << b << ";";

			// TO-DO/TODO: optimize it... it's not the fastest way to do it
			m_joyKeyStates[j][b].Update(strstr(joyButtonPressedList, ss.str().c_str()) != NULL);
		}

		m_joystickDpadStates[j][DPAD_KEY_UP   ].Update(strstr(joyButtonPressedList, DPAD_0_UP.c_str())    != NULL);
		m_joystickDpadStates[j][DPAD_KEY_DOWN ].Update(strstr(joyButtonPressedList, DPAD_1_DOWN.c_str())  != NULL);
		m_joystickDpadStates[j][DPAD_KEY_LEFT ].Update(strstr(joyButtonPressedList, DPAD_2_LEFT.c_str())  != NULL);
		m_joystickDpadStates[j][DPAD_KEY_RIGHT].Update(strstr(joyButtonPressedList, DPAD_3_RIGHT.c_str()) != NULL);

		m_xy[j].x   = static_cast<float>(ReadJoystickValue(AssembleJoystickAxisValueSharedDataPath(j, "X")));
		m_xy[j].y   = static_cast<float>(ReadJoystickValue(AssembleJoystickAxisValueSharedDataPath(j, "Y")));
		m_z[j]      = static_cast<float>(ReadJoystickValue(AssembleJoystickAxisValueSharedDataPath(j, "Z")));
		m_rudder[j] = static_cast<float>(ReadJoystickValue(AssembleJoystickAxisValueSharedDataPath(j, "Rudder")));
	}
}

std::string AndroidInput::AssembleJoystickSharedDataPath(const std::size_t j, const std::string& parameter)
{
	std::stringstream ss;
	ss << "ethanon.system.joystick" << j << "." << parameter;
	return ss.str();
}

std::string AndroidInput::AssembleJoystickAxisValueSharedDataPath(const std::size_t j, const std::string& axis)
{
	std::stringstream ss;
	ss << "ethanon.system.joystick" << j << "." << "axis" << axis;
	return ss.str();
}

GS_JOYSTICK_STATUS AndroidInput::GetJoystickStatus(const unsigned int id) const
{
	if (id < m_numJoysticks)
	{
		return GSJS_DETECTED;
	}
	else
	{
		return GSJS_NOTDETECTED;
	}
}

bool AndroidInput::DetectJoysticks()
{
	Command(Platform::NativeCommandAssembler::DetectJoysticks());
	return true;
}

unsigned int AndroidInput::GetNumJoyButtons(const unsigned int id) const
{
	if (id < m_joyNumButtons.size())
		return m_joyNumButtons[id];
	else
		return 0;
}

void AndroidInput::ShowJoystickWarnings(const bool enable)
{
	// dummy
}

bool AndroidInput::IsShowingJoystickWarnings() const
{
	return false;
}

unsigned int AndroidInput::GetMaxJoysticks() const
{
	return m_maxJoysticks;
}

GS_KEY_STATE AndroidInput::GetJoystickButtonState(const unsigned int id, const GS_JOYSTICK_BUTTON key) const
{
	if (id < m_joyKeyStates.size())
	{
		if (std::size_t(key) < m_joyKeyStates[id].size())
		{
			return m_joyKeyStates[id][key].GetCurrentState();
		}
		else
		{
			if (key == GSB_LEFT)
				return m_joystickDpadStates[id][DPAD_KEY_LEFT].GetCurrentState();
			else if (key == GSB_RIGHT)
				return m_joystickDpadStates[id][DPAD_KEY_RIGHT].GetCurrentState();
			else if (key == GSB_UP)
				return m_joystickDpadStates[id][DPAD_KEY_UP].GetCurrentState();
			else if (key == GSB_DOWN)
				return m_joystickDpadStates[id][DPAD_KEY_DOWN].GetCurrentState();
		}
	}
	return GSKS_UP;
}

bool AndroidInput::IsJoystickButtonDown(const unsigned int id, const GS_JOYSTICK_BUTTON key) const
{
	const GS_KEY_STATE state = GetJoystickButtonState(id, key);
	return (state == GSKS_HIT || state == GSKS_DOWN);
}

unsigned int AndroidInput::GetNumJoysticks() const
{
	return m_numJoysticks;
}

math::Vector2 AndroidInput::GetJoystickXY(const unsigned int id) const
{
	if (id < m_xy.size())
	{
		return m_xy[id];
	}
	else
	{
		return Vector2();
	}
}

float AndroidInput::GetJoystickZ(const unsigned int id) const
{
	if (id < m_z.size())
	{
		return m_z[id];
	}
	else
	{
		return 0.0f;
	}
}

float AndroidInput::GetJoystickRudder(const unsigned int id) const
{
	if (id < m_rudder.size())
	{
		return m_rudder[id];
	}
	else
	{
		return 0.0f;
	}
}

GS_JOYSTICK_BUTTON AndroidInput::GetFirstButtonDown(const unsigned int id) const
{
	// TODO
	return GSB_NONE;
}

math::Vector2 AndroidInput::GetJoystickUV(const unsigned int id) const
{
	// TODO
	return Vector2();
}

} // namespace gs2d
