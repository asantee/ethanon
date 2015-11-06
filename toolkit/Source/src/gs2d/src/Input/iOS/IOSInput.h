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

#ifndef GS2D_IOS_INPUT_H_
#define GS2D_IOS_INPUT_H_

#include "../MobileInput.h"

#include <vector>

#import <GameController/GameController.h>

namespace gs2d {

class IOSInput : public MobileInput
{
	math::Vector3 m_accelerometer;

	class Joystick
	{
	public:
		Joystick(GCController*);
		GCController* controller;
		GS_JOYSTICK_STATUS status;
		math::Vector2 xy;
		KeyStateManager state[GSB_NUM_BUTTONS];
	};

	std::vector<Joystick> m_joysticks;

	bool m_forcePause;

public:
	IOSInput(const unsigned int maxTouchCount);

	void ForceGamepadPause();

	void SetCurrentTouchPos(const unsigned int n, const gs2d::math::Vector2& pos);
	void SetAccelerometerData(const gs2d::math::Vector3& data);

	bool IsKeyDown(const GS_KEY key) const;
	GS_KEY_STATE GetKeyState(const GS_KEY key) const;

	bool Update();
 
	math::Vector3 GetAccelerometerData() const;

	unsigned int GetMaxJoysticks() const;

	GS_KEY_STATE GetJoystickButtonState(const unsigned int index, const GS_JOYSTICK_BUTTON key) const;
	bool IsJoystickButtonDown(const unsigned int index, const GS_JOYSTICK_BUTTON key) const;
	bool DetectJoysticks();
	GS_JOYSTICK_STATUS GetJoystickStatus(const unsigned int index) const;
	unsigned int GetNumJoyButtons(const unsigned int index) const;
	math::Vector2 GetJoystickXY(const unsigned int index) const;
	float GetJoystickZ(const unsigned int index) const;
	float GetJoystickRudder(const unsigned int index) const;
	math::Vector2 GetJoystickUV(const unsigned int index) const;
	GS_JOYSTICK_BUTTON GetFirstButtonDown(const unsigned int index) const;
	unsigned int GetNumJoysticks() const;
}; 

typedef boost::shared_ptr<IOSInput> IOSInputPtr;

} // namespace gs2d

#endif
