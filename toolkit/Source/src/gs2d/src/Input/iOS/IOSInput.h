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
