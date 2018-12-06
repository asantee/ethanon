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

#include "AndroidInput.h"

#include "../../Application.h"

#include <string.h>
#include <stdio.h>

namespace gs2d {

using namespace math;

const str_type::string AndroidInput::KEY_STATE_DOWN = "down";
const str_type::string AndroidInput::KEY_STATE_UP = "up";

const str_type::string AndroidInput::KEY_CODE_UP = "ethanon.system.keyboard.up";
const str_type::string AndroidInput::KEY_CODE_DOWN = "ethanon.system.keyboard.down";
const str_type::string AndroidInput::KEY_CODE_LEFT = "ethanon.system.keyboard.left";
const str_type::string AndroidInput::KEY_CODE_RIGHT = "ethanon.system.keyboard.right";
const str_type::string AndroidInput::KEY_CODE_PAGE_UP = "ethanon.system.keyboard.pageUp";
const str_type::string AndroidInput::KEY_CODE_PAGE_DOWN = "ethanon.system.keyboard.pageDown";
const str_type::string AndroidInput::KEY_CODE_SPACE = "ethanon.system.keyboard.space";
const str_type::string AndroidInput::KEY_CODE_ENTER = "ethanon.system.keyboard.enter";
const str_type::string AndroidInput::KEY_CODE_HOME = "ethanon.system.keyboard.home";
const str_type::string AndroidInput::KEY_CODE_INSERT = "ethanon.system.keyboard.insert";
const str_type::string AndroidInput::KEY_CODE_ESCAPE = "ethanon.system.keyboard.esc";
const str_type::string AndroidInput::KEY_CODE_TAB = "ethanon.system.keyboard.tab";
const str_type::string AndroidInput::KEY_CODE_SHIFT = "ethanon.system.keyboard.shift";
const str_type::string AndroidInput::KEY_CODE_ALT = "ethanon.system.keyboard.alt";
const str_type::string AndroidInput::KEY_CODE_CTRL = "ethanon.system.keyboard.ctrl";
const str_type::string AndroidInput::KEY_CODE_F1 = "ethanon.system.keyboard.f1";
const str_type::string AndroidInput::KEY_CODE_F2 = "ethanon.system.keyboard.f2";
const str_type::string AndroidInput::KEY_CODE_F3 = "ethanon.system.keyboard.f3";
const str_type::string AndroidInput::KEY_CODE_F4 = "ethanon.system.keyboard.f4";
const str_type::string AndroidInput::KEY_CODE_F5 = "ethanon.system.keyboard.f5";
const str_type::string AndroidInput::KEY_CODE_F6 = "ethanon.system.keyboard.f6";
const str_type::string AndroidInput::KEY_CODE_F7 = "ethanon.system.keyboard.f7";
const str_type::string AndroidInput::KEY_CODE_F8 = "ethanon.system.keyboard.f8";
const str_type::string AndroidInput::KEY_CODE_F9 = "ethanon.system.keyboard.f9";
const str_type::string AndroidInput::KEY_CODE_F10 = "ethanon.system.keyboard.f10";
const str_type::string AndroidInput::KEY_CODE_F11 = "ethanon.system.keyboard.f11";
const str_type::string AndroidInput::KEY_CODE_F12 = "ethanon.system.keyboard.f12";
const str_type::string AndroidInput::KEY_CODE_A = "ethanon.system.keyboard.a";
const str_type::string AndroidInput::KEY_CODE_B = "ethanon.system.keyboard.b";
const str_type::string AndroidInput::KEY_CODE_C = "ethanon.system.keyboard.c";
const str_type::string AndroidInput::KEY_CODE_D = "ethanon.system.keyboard.d";
const str_type::string AndroidInput::KEY_CODE_E = "ethanon.system.keyboard.e";
const str_type::string AndroidInput::KEY_CODE_F = "ethanon.system.keyboard.f";
const str_type::string AndroidInput::KEY_CODE_G = "ethanon.system.keyboard.g";
const str_type::string AndroidInput::KEY_CODE_H = "ethanon.system.keyboard.h";
const str_type::string AndroidInput::KEY_CODE_I = "ethanon.system.keyboard.i";
const str_type::string AndroidInput::KEY_CODE_J = "ethanon.system.keyboard.j";
const str_type::string AndroidInput::KEY_CODE_K = "ethanon.system.keyboard.k";
const str_type::string AndroidInput::KEY_CODE_L = "ethanon.system.keyboard.l";
const str_type::string AndroidInput::KEY_CODE_M = "ethanon.system.keyboard.m";
const str_type::string AndroidInput::KEY_CODE_N = "ethanon.system.keyboard.n";
const str_type::string AndroidInput::KEY_CODE_O = "ethanon.system.keyboard.o";
const str_type::string AndroidInput::KEY_CODE_P = "ethanon.system.keyboard.p";
const str_type::string AndroidInput::KEY_CODE_Q = "ethanon.system.keyboard.q";
const str_type::string AndroidInput::KEY_CODE_R = "ethanon.system.keyboard.r";
const str_type::string AndroidInput::KEY_CODE_S = "ethanon.system.keyboard.s";
const str_type::string AndroidInput::KEY_CODE_T = "ethanon.system.keyboard.t";
const str_type::string AndroidInput::KEY_CODE_U = "ethanon.system.keyboard.u";
const str_type::string AndroidInput::KEY_CODE_V = "ethanon.system.keyboard.v";
const str_type::string AndroidInput::KEY_CODE_X = "ethanon.system.keyboard.x";
const str_type::string AndroidInput::KEY_CODE_Y = "ethanon.system.keyboard.y";
const str_type::string AndroidInput::KEY_CODE_Z = "ethanon.system.keyboard.z";
const str_type::string AndroidInput::KEY_CODE_W = "ethanon.system.keyboard.w";
const str_type::string AndroidInput::KEY_CODE_0 = "ethanon.system.keyboard.0";
const str_type::string AndroidInput::KEY_CODE_1 = "ethanon.system.keyboard.1";
const str_type::string AndroidInput::KEY_CODE_2 = "ethanon.system.keyboard.2";
const str_type::string AndroidInput::KEY_CODE_3 = "ethanon.system.keyboard.3";
const str_type::string AndroidInput::KEY_CODE_4 = "ethanon.system.keyboard.4";
const str_type::string AndroidInput::KEY_CODE_5 = "ethanon.system.keyboard.5";
const str_type::string AndroidInput::KEY_CODE_6 = "ethanon.system.keyboard.6";
const str_type::string AndroidInput::KEY_CODE_7 = "ethanon.system.keyboard.7";
const str_type::string AndroidInput::KEY_CODE_8 = "ethanon.system.keyboard.8";
const str_type::string AndroidInput::KEY_CODE_9 = "ethanon.system.keyboard.9";

GS2D_API InputPtr CreateInput(boost::any data, const bool showJoystickWarnings)
{
	std::string *str;
	try
	{
		str = boost::any_cast<std::string*>(data);
	}
	catch(const boost::bad_any_cast &)
	{
		return InputPtr();
	}
	return InputPtr(new AndroidInput(5, str));
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

str_type::string AndroidInput::PullCommands()
{
	str_type::string inputCommands;
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
			str_type::stringstream ss; ss << "b" << b << ";";

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
