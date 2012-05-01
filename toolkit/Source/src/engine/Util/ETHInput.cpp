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

#include "ETHInput.h"

void ETHInput::SetProvider(ETHResourceProviderPtr provider)
{
	m_provider = (provider);
}

void ETHInput::SetTargetManager(ETHBackBufferTargetManagerPtr manager)
{
	m_targetManager = manager;
}

GS_KEY_STATE ETHInput::GetLeftClickState()
{
	return m_provider->GetInput()->GetLeftClickState();
}

GS_KEY_STATE ETHInput::GetRightClickState()
{
	return m_provider->GetInput()->GetRightClickState();
}

GS_KEY_STATE ETHInput::GetMiddleClickState()
{
	return m_provider->GetInput()->GetMiddleClickState();
}

Vector2 ETHInput::GetMouseMove()
{
	return m_provider->GetInput()->GetMouseMoveF() * m_targetManager.lock()->GetTargetScale();
}

float ETHInput::GetWheelState()
{
	return m_provider->GetInput()->GetWheelState();
}

GS_KEY_STATE ETHInput::GetKeyState(const GS_KEY key)
{
	return m_provider->GetInput()->GetKeyState(key);
}

bool ETHInput::KeyDown(const GS_KEY key)
{
	return m_provider->GetInput()->IsKeyDown(key);
}

GS_KEY_STATE ETHInput::JoyButtonState(const unsigned int j, const GS_JOYSTICK_BUTTON button) const
{
	return m_provider->GetInput()->GetJoystickButtonState(j, button);
}

bool ETHInput::JoyButtonDown(const unsigned int j, const GS_JOYSTICK_BUTTON button) const
{
	return m_provider->GetInput()->IsJoystickButtonDown(j, button);
}

bool ETHInput::DetectJoysticks() const
{
	return m_provider->GetInput()->DetectJoysticks();
}

GS_JOYSTICK_STATUS ETHInput::GetJoystickStatus(const unsigned int j) const
{
	return m_provider->GetInput()->GetJoystickStatus(j);
}

unsigned int ETHInput::GetNumJoyButtons(const unsigned int j) const
{
	return m_provider->GetInput()->GetNumJoyButtons(j);
}

Vector2 ETHInput::GetJoystickXY(const unsigned int j) const
{
	return m_provider->GetInput()->GetJoystickXY(j);
}

float ETHInput::GetJoystickZ(const unsigned int j) const
{
	return m_provider->GetInput()->GetJoystickZ(j);
}

float ETHInput::GetJoystickRudder(const unsigned int j) const
{
	return m_provider->GetInput()->GetJoystickRudder(j);
}

Vector2 ETHInput::GetJoystickUV(const unsigned int j) const
{
	return m_provider->GetInput()->GetJoystickUV(j);
}

GS_JOYSTICK_BUTTON ETHInput::GetFirstButtonDown(const unsigned int j) const
{
	return m_provider->GetInput()->GetFirstButtonDown(j);
}

unsigned int ETHInput::GetMaxJoysticks() const
{
	return m_provider->GetInput()->GetMaxJoysticks();
}

Vector2 ETHInput::GetTouchPos(const unsigned int n) const
{
	return m_provider->GetInput()->GetTouchPos(n, m_provider->GetVideo()) * m_targetManager.lock()->GetTargetScale();
}

GS_KEY_STATE  ETHInput::GetTouchState(const unsigned int n) const
{
	return m_provider->GetInput()->GetTouchState(n);
}

unsigned int ETHInput::GetMaxTouchCount() const
{
	return m_provider->GetInput()->GetMaxTouchCount();
}

Vector2 ETHInput::GetTouchMove(const unsigned int n) const
{
	return m_provider->GetInput()->GetTouchMove(n) * m_targetManager.lock()->GetTargetScale();
}

Vector3 ETHInput::GetAccelerometerData() const
{
	return m_provider->GetInput()->GetAccelerometerData();
}
