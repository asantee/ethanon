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
