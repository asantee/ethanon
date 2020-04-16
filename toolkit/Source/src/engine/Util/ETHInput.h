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

	GS_KEY_STATE JoyButtonState(const uint32_t, const GS_JOYSTICK_BUTTON) const;
	bool JoyButtonDown(const uint32_t, const GS_JOYSTICK_BUTTON) const;
	bool DetectJoysticks() const;
	GS_JOYSTICK_STATUS GetJoystickStatus(const uint32_t) const;
	uint32_t GetNumJoyButtons(const uint32_t) const;
	Vector2 GetJoystickXY(const uint32_t) const;
	float GetJoystickZ(const uint32_t) const;
	float GetJoystickRudder(const uint32_t) const;
	Vector2 GetJoystickUV(const uint32_t) const;
	GS_JOYSTICK_BUTTON GetFirstButtonDown(const uint32_t) const;
	uint32_t GetMaxJoysticks() const;

	Vector2 GetTouchPos(const uint32_t n) const;
	GS_KEY_STATE  GetTouchState(const uint32_t n) const;
	uint32_t GetMaxTouchCount() const;
	Vector2 GetTouchMove(const uint32_t n) const;
	Vector3 GetAccelerometerData() const;
};
#endif
