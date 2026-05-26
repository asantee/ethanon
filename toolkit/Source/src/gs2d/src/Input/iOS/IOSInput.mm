#include "IOSInput.h"

#include <string.h>
#include <stdio.h>

namespace gs2d {

using namespace math;

IOSInput::Joystick::Joystick(GCController* _controller) :
	controller(_controller),
	status(GSJS_DETECTED)
{
}

InputPtr CreateInput(const bool showJoystickWarnings, std::string* inputSource)
{
	return InputPtr(new IOSInput(5));
}

IOSInput::IOSInput(const unsigned int maxTouchCount) :
	MobileInput(maxTouchCount),
	m_forcePause(false)
{
	for (unsigned int t = 0; t < GS_NUM_KEYS; t++)
	{
		m_keyBooleanStates[t] = false;
	}
}

bool IOSInput::DetectJoysticks()
{
	m_joysticks.clear();

	NSArray* controllers = [GCController controllers];
	const std::size_t count = [controllers count];
	for (std::size_t t = 0; t < count; t++)
	{
		GCController* controller = [[GCController controllers] objectAtIndex:t];
		GCExtendedGamepad *extentedGamepad = controller.extendedGamepad;
		GCMicroGamepad    *microGamepad = controller.microGamepad;

		if (!extentedGamepad && !microGamepad)
			continue;

		[controller setPlayerIndex:static_cast<GCControllerPlayerIndex>(m_joysticks.size())];
		m_joysticks.push_back(Joystick(controller));
	}
	return MobileInput::DetectJoysticks();
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
	return GetKeyState(key) == GSKS_DOWN;
}

GS_KEY_STATE IOSInput::GetKeyState(const GS_KEY key) const
{
	if (key == GSK_PAUSE)
	{
		return SumKeyStates(m_pauseState.GetCurrentState(), m_keyStates[GSK_PAUSE].GetCurrentState());
	}
	else
	{
		return m_keyStates[key].GetCurrentState();
	}
}

void IOSInput::ForcePause()
{
	m_forcePause = true;
}

void IOSInput::SetBooleanKeyState(const GS_KEY key, const bool pressed)
{
	m_keyBooleanStates[key] = pressed;
}

bool IOSInput::Update()
{
	NSArray* controllers = [GCController controllers];
	const std::size_t count = [controllers count];
	for (std::size_t t = 0; t < count; t++)
	{
		GCController* controller = [[GCController controllers] objectAtIndex:t];
		GCExtendedGamepad *extentedGamepad = controller.extendedGamepad;
		GCMicroGamepad    *microGamepad = controller.microGamepad;

		if ((!extentedGamepad && !microGamepad) || t >= m_joysticks.size())
			continue;

		Joystick& joystick = m_joysticks[t];

		if (extentedGamepad)
		{
			joystick.state[GSB_01].Update([[extentedGamepad buttonY] isPressed] || [[extentedGamepad buttonY] value] > 0.01f);
			joystick.state[GSB_02].Update([[extentedGamepad buttonB] isPressed] || [[extentedGamepad buttonB] value] > 0.01f);
			joystick.state[GSB_03].Update([[extentedGamepad buttonA] isPressed] || [[extentedGamepad buttonA] value] > 0.01f);
			joystick.state[GSB_04].Update([[extentedGamepad buttonX] isPressed] || [[extentedGamepad buttonX] value] > 0.01f);

			joystick.state[GSB_05].Update([[extentedGamepad  leftShoulder] isPressed] || [[extentedGamepad leftShoulder] value] > 0.0f);
			joystick.state[GSB_06].Update([[extentedGamepad rightShoulder] isPressed] || [[extentedGamepad rightShoulder] value] > 0.0f);
			joystick.state[GSB_07].Update([[extentedGamepad  leftShoulder] isPressed] || [[extentedGamepad leftTrigger] value] > 0.0f);
			joystick.state[GSB_08].Update([[extentedGamepad rightShoulder] isPressed] || [[extentedGamepad rightTrigger] value] > 0.0f);

			if (extentedGamepad.buttonMenu)
			{
				m_forcePause |= extentedGamepad.buttonMenu.isPressed;
			}

			const Vector2 dpad([[extentedGamepad dpad] xAxis].value, [[extentedGamepad dpad] yAxis].value);

			Vector2 leftThumbstick(0.0f, 0.0f);
	 
			leftThumbstick.x = [[extentedGamepad leftThumbstick] xAxis].value;
			leftThumbstick.y = [[extentedGamepad leftThumbstick] yAxis].value;
			joystick.xy = leftThumbstick;

			joystick.state[GSB_LEFT ].Update(leftThumbstick.x < -0.8f || dpad.x < -0.05f);
			joystick.state[GSB_RIGHT].Update(leftThumbstick.x >  0.8f || dpad.x >  0.05f);
			joystick.state[GSB_UP   ].Update(leftThumbstick.y >  0.8f || dpad.y >  0.05f);
			joystick.state[GSB_DOWN ].Update(leftThumbstick.y < -0.8f || dpad.y < -0.05f);
		}
		else
		{
			// Micro profile: A, X, D-pad only
			joystick.state[GSB_03].Update(microGamepad.buttonA.isPressed || microGamepad.buttonA.value > 0.01f);
			joystick.state[GSB_04].Update(microGamepad.buttonX.isPressed || microGamepad.buttonX.value > 0.01f);

			Vector2 dpad(microGamepad.dpad.xAxis.value, microGamepad.dpad.yAxis.value);
			Vector2 leftThumb(0.0f, 0.0f); // no sticks on micro
			joystick.xy = leftThumb;

			joystick.state[GSB_LEFT ].Update(dpad.x < -0.05f);
			joystick.state[GSB_RIGHT].Update(dpad.x >  0.05f);
			joystick.state[GSB_UP   ].Update(dpad.y >  0.05f);
			joystick.state[GSB_DOWN ].Update(dpad.y < -0.05f);

			if (microGamepad.buttonMenu)
			{
				m_forcePause |= microGamepad.buttonMenu.isPressed;
			}
		}

		joystick.state[GSB_10].Update(m_forcePause);
	}

	// Update keyboard states
	for (unsigned int t = 0; t < GS_NUM_KEYS; t++)
	{
		m_keyStates[t].Update(m_keyBooleanStates[t]);
	}

	m_pauseState.Update(m_forcePause);

	m_forcePause = false;
	return MobileInput::Update();
}

math::Vector3 IOSInput::GetAccelerometerData() const
{
	return m_accelerometer;
}

unsigned int IOSInput::GetMaxJoysticks() const
{
	return 4;
}

unsigned int IOSInput::GetNumJoysticks() const
{
	return static_cast<unsigned int>(m_joysticks.size());
}

GS_JOYSTICK_STATUS IOSInput::GetJoystickStatus(const unsigned int index) const
{
	if (index >= m_joysticks.size())
		return GSJS_INVALID;
	return m_joysticks[index].status;
}

unsigned int IOSInput::GetNumJoyButtons(const unsigned int index) const
{
	if (GetJoystickStatus(index) != GSJS_DETECTED)
		return 0;
	return 6; // todo: make it more accurate
}

GS_KEY_STATE IOSInput::GetJoystickButtonState(const unsigned int index, const GS_JOYSTICK_BUTTON key) const
{
	if (GetJoystickStatus(index) != GSJS_DETECTED)
		return GSKS_UP;
	return m_joysticks[index].state[key].GetCurrentState();
}

bool IOSInput::IsJoystickButtonDown(const unsigned int index, const GS_JOYSTICK_BUTTON key) const
{
	const GS_KEY_STATE state = GetJoystickButtonState(index, key);
	return (state == GSKS_DOWN || state == GSKS_HIT);
}

GS_JOYSTICK_BUTTON IOSInput::GetFirstButtonDown(const unsigned int index) const
{
	if (GetJoystickStatus(index) != GSJS_DETECTED)
		return GSB_NONE;

	for (unsigned int t = 0; t < GetNumJoyButtons(index); t++)
	{
		if (IsJoystickButtonDown(index, (GS_JOYSTICK_BUTTON)t))
			return (GS_JOYSTICK_BUTTON)t;
	}
	return GSB_NONE;
}

math::Vector2 IOSInput::GetJoystickXY(const unsigned int index) const
{
	if (GetJoystickStatus(index) != GSJS_DETECTED)
		return math::Vector2();
	return m_joysticks[index].xy;
}

float IOSInput::GetJoystickZ(const unsigned int id) const
{
	return 0.0f;
}

float IOSInput::GetJoystickRudder(const unsigned int id) const
{
	return 0.0f;
}

math::Vector2 IOSInput::GetJoystickUV(const unsigned int id) const
{
	return math::Vector2();
}

} // namespace gs2d
