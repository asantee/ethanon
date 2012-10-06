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

#ifndef ETH_INPUT_H_
#define ETH_INPUT_H_

#include <Input.h>
#include "../Resource/ETHResourceProvider.h"
#include "../Shader/ETHBackBufferTargetManager.h"
using namespace gs2d::math;
using namespace gs2d;

/// InputPtr object wrapper
class ETHInput
{
	ETHResourceProviderPtr m_provider;
	ETHBackBufferTargetManagerWeakPtr m_targetManager;

public:
	void SetProvider(ETHResourceProviderPtr provider);
	void SetTargetManager(ETHBackBufferTargetManagerPtr manager);

	GS_KEY_STATE GetLeftClickState();
	GS_KEY_STATE GetRightClickState();
	GS_KEY_STATE GetMiddleClickState();
	Vector2 GetMouseMove();
	float GetWheelState();
	GS_KEY_STATE GetKeyState(const GS_KEY);
	bool KeyDown(const GS_KEY);

	GS_KEY_STATE JoyButtonState(const unsigned int, const GS_JOYSTICK_BUTTON) const;
	bool JoyButtonDown(const unsigned int, const GS_JOYSTICK_BUTTON) const;
	bool DetectJoysticks() const;
	GS_JOYSTICK_STATUS GetJoystickStatus(const unsigned int) const;
	unsigned int GetNumJoyButtons(const unsigned int) const;
	Vector2 GetJoystickXY(const unsigned int) const;
	float GetJoystickZ(const unsigned int) const;
	float GetJoystickRudder(const unsigned int) const;
	Vector2 GetJoystickUV(const unsigned int) const;
	GS_JOYSTICK_BUTTON GetFirstButtonDown(const unsigned int) const;
	unsigned int GetMaxJoysticks() const;

	Vector2 GetTouchPos(const unsigned int n) const;
	GS_KEY_STATE  GetTouchState(const unsigned int n) const;
	unsigned int GetMaxTouchCount() const;
	Vector2 GetTouchMove(const unsigned int n) const;
	Vector3 GetAccelerometerData() const;
};
#endif