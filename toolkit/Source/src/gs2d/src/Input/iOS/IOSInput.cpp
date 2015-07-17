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

#include "IOSInput.h"
#include <string.h>
#include <stdio.h>

namespace gs2d {

using namespace math;

GS2D_API InputPtr CreateInput(boost::any data, const bool showJoystickWarnings)
{
	return InputPtr(new IOSInput(5));
}

IOSInput::IOSInput(const unsigned int maxTouchCount) :
	MobileInput(maxTouchCount)
{
}

void IOSInput::SetAccelerometerData(const gs2d::math::Vector3& data)
{
	m_accelerometer = data * 9.1f;
}

void IOSInput::SetCurrentTouchPos(const unsigned int n, const gs2d::math::Vector2& pos)
{	
	if (n < m_maxTouchCount)
	{
		m_currentTouch[n] = pos;
	}
}

bool IOSInput::IsKeyDown(const GS_KEY key) const
{
	return false;
}

GS_KEY_STATE IOSInput::GetKeyState(const GS_KEY key) const
{
	return GSKS_UP;
}

bool IOSInput::Update()
{
	return MobileInput::Update();
}

math::Vector3 IOSInput::GetAccelerometerData() const
{
	return m_accelerometer;
}

} // namespace gs2d
