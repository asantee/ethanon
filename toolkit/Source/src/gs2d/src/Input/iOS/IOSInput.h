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

	KeyStateManager m_pauseState;

	bool m_keyBooleanStates[GS_NUM_KEYS];
	KeyStateManager m_keyStates[GS_NUM_KEYS];

	bool m_forcePause;

public:
	IOSInput(const unsigned int maxTouchCount);

	void SetBooleanKeyState(const GS_KEY key, const bool pressed);
	
	void ForcePause();

	void SetCurrentTouchPos(const unsigned int n, const gs2d::math::Vector2& pos);
	void SetAccelerometerData(const gs2d::math::Vector3& data);

	bool IsKeyDown(const GS_KEY key) const override;
	GS_KEY_STATE GetKeyState(const GS_KEY key) const override;

	bool Update() override;
 
	math::Vector3 GetAccelerometerData() const override;

	unsigned int GetMaxJoysticks() const override;

	GS_KEY_STATE GetJoystickButtonState(const unsigned int index, const GS_JOYSTICK_BUTTON key) const override;
	bool IsJoystickButtonDown(const unsigned int index, const GS_JOYSTICK_BUTTON key) const override;
	bool DetectJoysticks() override;
	GS_JOYSTICK_STATUS GetJoystickStatus(const unsigned int index) const override;
	unsigned int GetNumJoyButtons(const unsigned int index) const override;
	math::Vector2 GetJoystickXY(const unsigned int index) const override;
	float GetJoystickZ(const unsigned int index) const override;
	float GetJoystickRudder(const unsigned int index) const override;
	math::Vector2 GetJoystickUV(const unsigned int index) const override;
	GS_JOYSTICK_BUTTON GetFirstButtonDown(const unsigned int index) const override;
	unsigned int GetNumJoysticks() const override;
}; 

typedef boost::shared_ptr<IOSInput> IOSInputPtr;

} // namespace gs2d

#endif
