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

	bool Update() override;

	void ShowJoystickWarnings(const bool enable) override;
	bool IsShowingJoystickWarnings() const override;
	unsigned int GetMaxJoysticks() const override;

	GS_KEY_STATE GetJoystickButtonState(const unsigned int id, const GS_JOYSTICK_BUTTON key) const override;
	bool IsJoystickButtonDown(const unsigned int id, const GS_JOYSTICK_BUTTON key) const override;
	bool DetectJoysticks() override;
	GS_JOYSTICK_STATUS GetJoystickStatus(const unsigned int id) const override;
	unsigned int GetNumJoyButtons(const unsigned int id) const override;
	math::Vector2 GetJoystickXY(const unsigned int id) const override;
	float GetJoystickZ(const unsigned int id) const override;
	float GetJoystickRudder(const unsigned int id) const override;
	math::Vector2 GetJoystickUV(const unsigned int id) const override;
	GS_JOYSTICK_BUTTON GetFirstButtonDown(const unsigned int id) const override;
	unsigned int GetNumJoysticks() const override;

private:
	std::vector<Joystick> m_joysticks;
};

} // namespace gs2d

#endif
