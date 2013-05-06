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
	UpdateKey(str, KEY_PRESSED_CMD + "back",  GSK_BACK);
	UpdateKey(str, KEY_PRESSED_CMD + "pause", GSK_PAUSE);
}

void AndroidInput::UpdateKey(const std::string& str, const std::string& keyName, const GS_KEY key)
{
	m_keyStates[key].Update(strstr(str.c_str(), keyName.c_str()) != NULL);
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
		for (std::size_t j = 0; j < m_numJoysticks; j++)
			m_joystickDpadStates[j].resize(4);
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
	}
}

std::string AndroidInput::AssembleJoystickSharedDataPath(const std::size_t j, const std::string& parameter)
{
	std::stringstream ss;
	ss << "ethanon.system.joystick" << j << "." << parameter;
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

GS_JOYSTICK_BUTTON AndroidInput::GetFirstButtonDown(const unsigned int id) const
{
	// TODO
	return GSB_NONE;
}

math::Vector2 AndroidInput::GetJoystickXY(const unsigned int id) const
{
	// TODO
	return Vector2();
}

float AndroidInput::GetJoystickZ(const unsigned int id) const
{
	// TODO
	return 0.0f;
}

float AndroidInput::GetJoystickRudder(const unsigned int id) const
{
	// TODO
	return 0.0f;
}

math::Vector2 AndroidInput::GetJoystickUV(const unsigned int id) const
{
	// TODO
	return Vector2();
}

} // namespace gs2d
