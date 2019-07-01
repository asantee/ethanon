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

GS2D_API InputPtr CreateInput(boost::any data, const bool showJoystickWarnings)
{
	return InputPtr(new IOSInput(5));
}

IOSInput::IOSInput(const unsigned int maxTouchCount) :
	MobileInput(maxTouchCount),
	m_forcePause(false)
{
}

bool IOSInput::DetectJoysticks()
{
	m_joysticks.clear();

	NSArray* controllers = [GCController controllers];
	const std::size_t count = [controllers count];
	for (std::size_t t = 0; t < count; t++)
	{
		GCController* controller = [[GCController controllers] objectAtIndex:t];
		if ([controller gamepad])
		{
			[controller setPlayerIndex:static_cast<GCControllerPlayerIndex>(m_joysticks.size())];
			m_joysticks.push_back(Joystick(controller));

			[controller setControllerPausedHandler:^(GCController *controller)
			{
				[[NSNotificationCenter defaultCenter] postNotificationName:@"GameTogglePauseNotification" object:nil];
			}];
		}
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
	return false;
}

GS_KEY_STATE IOSInput::GetKeyState(const GS_KEY key) const
{
	return GSKS_UP;
}

void IOSInput::ForceGamepadPause()
{
	m_forcePause = true;
}

bool IOSInput::Update()
{
	NSArray* controllers = [GCController controllers];
	const std::size_t count = [controllers count];
	for (std::size_t t = 0; t < count; t++)
	{
		GCController* controller = [[GCController controllers] objectAtIndex:t];
		GCGamepad* gamepad = [controller gamepad];

		if (!gamepad || t >= m_joysticks.size())
			continue;

		Joystick& joystick = m_joysticks[t];

		joystick.state[GSB_01].Update([[gamepad buttonY] isPressed] || [[gamepad buttonY] value] > 0.01f);
		joystick.state[GSB_02].Update([[gamepad buttonB] isPressed] || [[gamepad buttonB] value] > 0.01f);
		joystick.state[GSB_03].Update([[gamepad buttonA] isPressed] || [[gamepad buttonA] value] > 0.01f);
		joystick.state[GSB_04].Update([[gamepad buttonX] isPressed] || [[gamepad buttonX] value] > 0.01f);

		joystick.state[GSB_05].Update([[gamepad  leftShoulder] isPressed] || [[gamepad  leftShoulder] value] > 0.0f);
		joystick.state[GSB_06].Update([[gamepad rightShoulder] isPressed] || [[gamepad rightShoulder] value] > 0.0f);

		joystick.state[GSB_10].Update(m_forcePause);

		const Vector2 dpad([[gamepad dpad] xAxis].value, [[gamepad dpad] yAxis].value);

		Vector2 leftThumbstick(0.0f, 0.0f);
 
		if ([controller extendedGamepad])
		{
			GCExtendedGamepad* exGamepad = [controller extendedGamepad];
			leftThumbstick.x = [[exGamepad leftThumbstick] xAxis].value;
			leftThumbstick.y = [[exGamepad leftThumbstick] yAxis].value;
			joystick.xy = leftThumbstick;
		}

		joystick.state[GSB_LEFT ].Update(leftThumbstick.x < -0.8f || dpad.x < -0.05f);
		joystick.state[GSB_RIGHT].Update(leftThumbstick.x >  0.8f || dpad.x >  0.05f);
		joystick.state[GSB_UP   ].Update(leftThumbstick.y >  0.8f || dpad.y >  0.05f);
		joystick.state[GSB_DOWN ].Update(leftThumbstick.y < -0.8f || dpad.y < -0.05f);
	}
	m_forcePause =  false;
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
