/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

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

#include "gs2dAndroidInput.h"
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

AndroidInput::AndroidInput(const unsigned int maxTouchCount, const std::string *input) :
	MobileInput(maxTouchCount),
	m_input(input)
{
	for (int t=0; t<GS_NUM_KEYS; t++)
	{
		m_count[t] = 0;
		m_keyState[t] = GSKS_UP;
	}

	// TODO/TO-DO: implement other keys...
	m_keyName[GSK_BACK]  = "back";
	m_keyName[GSK_PAUSE] = "pause";
}

bool AndroidInput::IsKeyDown(const GS_KEY key) const
{
	return (m_keyState[key] == GSKS_HIT || m_keyState[key] == GSKS_DOWN);
}

GS_KEY_STATE AndroidInput::GetKeyState(const GS_KEY key) const
{
	return m_keyState[key];
}


double ReadValue(std::stringstream& ss)
{
	double r;
	ss >> r;
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
	if (strstr(str.c_str(), keyName.c_str()) != NULL)
	{
		m_count[key]++;
	}
	else if (m_count[key] > 0)
	{
		m_count[key] = 0;
		m_keyState[key] = GSKS_RELEASE;
		return;
	}
	else if (m_count[key] == 0)
	{
		m_keyState[key] = GSKS_UP;
		return;
	}

	if (m_count[key] == 1)
	{
		m_keyState[key] = GSKS_HIT;
		return;
	}

	m_keyState[key] = GSKS_DOWN;
}

bool AndroidInput::Update()
{
	std::stringstream stream;
	stream << *m_input;
	ReadValuePairs(stream, m_currentTouch, m_maxTouchCount);
	m_accelerometer.x = static_cast<float>(ReadValue(stream));
	m_accelerometer.y = static_cast<float>(ReadValue(stream));
	m_accelerometer.z = static_cast<float>(ReadValue(stream));

	UpdateKeys(*m_input);
	MobileInput::Update();
}

math::Vector3 AndroidInput::GetAccelerometerData() const
{
	return m_accelerometer;
}

} // namespace gs2d