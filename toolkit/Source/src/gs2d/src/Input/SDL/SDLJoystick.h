#ifndef GS2D_SDL_JOYSTICK_H_
#define GS2D_SDL_JOYSTICK_H_

#include "../../Input.h"

#include <SDL2/SDL.h>

#include <vector>

namespace gs2d {

class SDLJoystick : virtual public Input
{
	bool m_showJoystickWarnings;
	unsigned int m_nDetectedJoysticks;

public:
	class Joystick
	{
	public:
		Joystick();
		SDL_Joystick* sdlJoystick;
		GS_JOYSTICK_STATUS status;
		math::Vector2 xy;
		float z;
		float rudder;
		math::Vector2 uv;
		KeyStateManager state[GSB_NUM_BUTTONS];
		int nButtons;
	};

	SDLJoystick(const bool showJoystickWarnings);

	bool Update();

	void ShowJoystickWarnings(const bool enable);
	bool IsShowingJoystickWarnings() const;
	unsigned int GetMaxJoysticks() const;

	GS_KEY_STATE GetJoystickButtonState(const unsigned int id, const GS_JOYSTICK_BUTTON key) const;
	bool IsJoystickButtonDown(const unsigned int id, const GS_JOYSTICK_BUTTON key) const;
	bool DetectJoysticks();
	GS_JOYSTICK_STATUS GetJoystickStatus(const unsigned int id) const;
	unsigned int GetNumJoyButtons(const unsigned int id) const;
	math::Vector2 GetJoystickXY(const unsigned int id) const;
	float GetJoystickZ(const unsigned int id) const;
	float GetJoystickRudder(const unsigned int id) const;
	math::Vector2 GetJoystickUV(const unsigned int id) const;
	GS_JOYSTICK_BUTTON GetFirstButtonDown(const unsigned int id) const;
	unsigned int GetNumJoysticks() const;

private:
	std::vector<Joystick> m_joysticks;
};

} // namespace gs2d

#endif
