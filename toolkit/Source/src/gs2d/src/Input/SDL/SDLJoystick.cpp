#include "SDLJoystick.h"

#include "../../Application.h"

namespace gs2d {

#define GS_SDL_MIN_JAXIS (-32768)
#define GS_SDL_MAX_JAXIS (32767)
#define GS_JOYSTICK_MIN_ARROW_VALUE (0.8f)

	SDLJoystick::Joystick::Joystick() :
		sdlJoystick(0),
		sdlGameController(0),
		status(GSJS_INVALID),
		z(0.0f),
		rudder(0.0f),
		nButtons(0)
{
}

SDLJoystick::SDLJoystick(const bool showJoystickWarnings) :
	m_showJoystickWarnings(showJoystickWarnings)
{
	DetectJoysticks();
}

static bool IsArrowPressed(SDLJoystick::Joystick& joy, const GS_JOYSTICK_BUTTON button, float axisValue)
{
	bool pressed = false;

	switch (button)
	{
	case GSB_DOWN:
		pressed = SDL_GameControllerGetButton(joy.sdlGameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN) || (axisValue >= GS_JOYSTICK_MIN_ARROW_VALUE);
		break;

	case GSB_RIGHT:
		pressed = SDL_GameControllerGetButton(joy.sdlGameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || (axisValue >= GS_JOYSTICK_MIN_ARROW_VALUE);
		break;

	case GSB_UP:
		pressed = SDL_GameControllerGetButton(joy.sdlGameController, SDL_CONTROLLER_BUTTON_DPAD_UP) || (axisValue <= -GS_JOYSTICK_MIN_ARROW_VALUE);
		break;

	case GSB_LEFT:
		pressed = SDL_GameControllerGetButton(joy.sdlGameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT) || (axisValue <= -GS_JOYSTICK_MIN_ARROW_VALUE);
		break;

	default:
		pressed = false;
		break;
	}
	return pressed;
}

static void UpdateJoystickAxes(SDLJoystick::Joystick& joy)
{
	const int nAxes = SDL_JoystickNumAxes(joy.sdlJoystick);

	static Sint16 axes[6];
	for (int a = 0; a < 6; a++)
	{
		axes[a] = SDL_JoystickGetAxis(joy.sdlJoystick, a);
		if (math::Abs(static_cast<int>(axes[a])) < 130)
			axes[a] = 0;
	}

	// update X and Y axis
	const float stride = (float)GS_SDL_MAX_JAXIS;
	joy.xy.x = (float)axes[0] / stride;
	joy.xy.y = (float)axes[1] / stride;
	joy.xy.x = math::Min(math::Max(joy.xy.x, -1.0f), 1.0f);
	joy.xy.y = math::Min(math::Max(joy.xy.y, -1.0f), 1.0f);

	// update Z axis
	if (nAxes > 2)
	{
		joy.z = (float)axes[2] / stride;
		joy.z = math::Min(math::Max(joy.z, -1.0f), 1.0f);
	}

	// update Rudder axis
	if (nAxes > 3)
	{
		joy.rudder = (float)axes[3] / stride;
		joy.rudder = math::Min(math::Max(joy.rudder, -1.0f), 1.0f);
	}

	// update U and V axis
	if (nAxes > 4)
	{
		joy.uv.x = (float)axes[4] / stride;
		joy.uv.x = math::Min(math::Max(joy.uv.x, -1.0f), 1.0f);
	}
	if (nAxes > 5)
	{
		joy.uv.y = (float)axes[5] / stride;
		joy.uv.y = math::Min(math::Max(joy.uv.y, -1.0f), 1.0f);
	}

	joy.state[GSB_LEFT ].Update(IsArrowPressed(joy, GSB_LEFT,  joy.xy.x));
	joy.state[GSB_RIGHT].Update(IsArrowPressed(joy, GSB_RIGHT, joy.xy.x));
	joy.state[GSB_UP   ].Update(IsArrowPressed(joy, GSB_UP,    joy.xy.y));
	joy.state[GSB_DOWN ].Update(IsArrowPressed(joy, GSB_DOWN,  joy.xy.y));
}

bool SDLJoystick::Update()
{
	if (m_nDetectedJoysticks != SDL_NumJoysticks())
		DetectJoysticks();

	for (unsigned int j = 0; j < m_nDetectedJoysticks; j++)
	{
		if (m_joysticks[j].status == GSJS_DETECTED)
		{
			m_joysticks[j].status = (SDL_JoystickGetAttached(m_joysticks[j].sdlJoystick)) ? GSJS_DETECTED : GSJS_NOTDETECTED;

			if (m_joysticks[j].status != GSJS_DETECTED)
			{
				continue;
			}
			if (m_joysticks[j].sdlGameController)
			{
				m_joysticks[j].state[GSB_A].Update((bool)SDL_GameControllerGetButton(m_joysticks[j].sdlGameController, SDL_CONTROLLER_BUTTON_A));
				m_joysticks[j].state[GSB_B].Update((bool)SDL_GameControllerGetButton(m_joysticks[j].sdlGameController, SDL_CONTROLLER_BUTTON_B));
				m_joysticks[j].state[GSB_X].Update((bool)SDL_GameControllerGetButton(m_joysticks[j].sdlGameController, SDL_CONTROLLER_BUTTON_X));
				m_joysticks[j].state[GSB_Y].Update((bool)SDL_GameControllerGetButton(m_joysticks[j].sdlGameController, SDL_CONTROLLER_BUTTON_Y));
				m_joysticks[j].state[GSB_LEFT_SHOULDER].Update((bool)SDL_GameControllerGetButton(m_joysticks[j].sdlGameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER));
				m_joysticks[j].state[GSB_RIGHT_SHOULDER].Update((bool)SDL_GameControllerGetButton(m_joysticks[j].sdlGameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER));
				m_joysticks[j].state[GSB_LEFT_STICK].Update((bool)SDL_GameControllerGetButton(m_joysticks[j].sdlGameController, SDL_CONTROLLER_BUTTON_LEFTSTICK));
				m_joysticks[j].state[GSB_RIGHT_STICK].Update((bool)SDL_GameControllerGetButton(m_joysticks[j].sdlGameController, SDL_CONTROLLER_BUTTON_RIGHTSTICK));
				m_joysticks[j].state[GSB_START].Update((bool)SDL_GameControllerGetButton(m_joysticks[j].sdlGameController, SDL_CONTROLLER_BUTTON_START));
				m_joysticks[j].state[GSB_SELECT].Update((bool)SDL_GameControllerGetButton(m_joysticks[j].sdlGameController, SDL_CONTROLLER_BUTTON_BACK));
			}
			// Fallback generic joystick
			else
			{
				for (unsigned int t = 0; (t < m_joysticks[j].nButtons) &&  (t < GSB_MAX_BUTTONS); t++)
				{
					const bool isPressed = SDL_JoystickGetButton(m_joysticks[j].sdlJoystick, static_cast<int>(t));
					m_joysticks[j].state[t].Update(isPressed);
				}
			}
			UpdateJoystickAxes(m_joysticks[j]);
		}
	}
	return true;
}

void SDLJoystick::ShowJoystickWarnings(const bool enable)
{
	m_showJoystickWarnings = enable;
}

bool SDLJoystick::IsShowingJoystickWarnings() const
{
	return m_showJoystickWarnings;
}

bool SDLJoystick::DetectJoysticks()
{
	m_nDetectedJoysticks = SDL_NumJoysticks();

	if (m_nDetectedJoysticks <= 0)
	{
		if (IsShowingJoystickWarnings())
			ShowMessage("No joysticks were detected", GSMT_INFO);
		return false;
	}
	m_joysticks.clear();
	m_joysticks.resize(m_nDetectedJoysticks);
	for (unsigned int t = 0; t < m_nDetectedJoysticks; t++)
	{
		if (SDL_IsGameController(t)) {
			char* mapping;
			SDL_Log("Index \'%i\' is a compatible controller, named \'%s\'", t, SDL_GameControllerNameForIndex(t));
			m_joysticks[t].sdlGameController = SDL_GameControllerOpen(t);
			m_joysticks[t].sdlJoystick = SDL_GameControllerGetJoystick(m_joysticks[t].sdlGameController);
			mapping = SDL_GameControllerMapping(m_joysticks[t].sdlGameController);
			SDL_Log("Controller %i is mapped as \"%s\".", t, mapping);
			SDL_free(mapping);
			m_joysticks[t].status = GSJS_DETECTED;
			m_joysticks[t].nButtons = SDL_JoystickNumButtons(m_joysticks[t].sdlJoystick);
		}
		else
		{
			// Fallback to generic
			m_joysticks[t].sdlJoystick = SDL_JoystickOpen(t);
			if (m_joysticks[t].sdlJoystick && SDL_JoystickGetAttached(m_joysticks[t].sdlJoystick))
			{
				m_joysticks[t].status = GSJS_DETECTED;
				m_joysticks[t].nButtons = SDL_JoystickNumButtons(m_joysticks[t].sdlJoystick);
			}
			else
			{
				m_joysticks[t].nButtons = 0;
				m_joysticks[t].status = GSJS_INVALID;
				SDL_Log("Index \'%i\' is not a compatible controller.", t);
			}

		}
	}
	return true;
}

unsigned int SDLJoystick::GetMaxJoysticks() const
{
	return 4;
}

unsigned int SDLJoystick::GetNumJoysticks() const
{
	return static_cast<unsigned int>(m_joysticks.size());
}

GS_JOYSTICK_STATUS SDLJoystick::GetJoystickStatus(const unsigned int id) const
{
	if (id >= m_joysticks.size())
		return GSJS_INVALID;
	return m_joysticks[id].status;
}

unsigned int SDLJoystick::GetNumJoyButtons(const unsigned int id) const
{
	if (GetJoystickStatus(id) != GSJS_DETECTED)
		return 0;
	return m_joysticks[id].nButtons;
}

GS_KEY_STATE SDLJoystick::GetJoystickButtonState(const unsigned int id, const GS_JOYSTICK_BUTTON key) const
{
	if (GetJoystickStatus(id) != GSJS_DETECTED)
		return GSKS_UP;
	return m_joysticks[id].state[key].GetCurrentState();
}

bool SDLJoystick::IsJoystickButtonDown(const unsigned int id, const GS_JOYSTICK_BUTTON key) const
{
	const GS_KEY_STATE state = GetJoystickButtonState(id, key);
	return (state == GSKS_DOWN || state == GSKS_HIT);
}

GS_JOYSTICK_BUTTON SDLJoystick::GetFirstButtonDown(const unsigned int id) const
{
	if (GetJoystickStatus(id) != GSJS_DETECTED)
		return GSB_NONE;

	for (unsigned int t = 0; t < m_joysticks[id].nButtons; t++)
	{
		if (IsJoystickButtonDown(id, (GS_JOYSTICK_BUTTON)t))
			return (GS_JOYSTICK_BUTTON)t;
	}
	return GSB_NONE;
}

math::Vector2 SDLJoystick::GetJoystickXY(const unsigned int id) const
{
	if (GetJoystickStatus(id) != GSJS_DETECTED)
		return math::Vector2();
	return m_joysticks[id].xy;
}

float SDLJoystick::GetJoystickZ(const unsigned int id) const
{
	if (GetJoystickStatus(id) != GSJS_DETECTED)
		return 0.0f;
	return m_joysticks[id].z;
}

float SDLJoystick::GetJoystickRudder(const unsigned int id) const
{
	if (GetJoystickStatus(id) != GSJS_DETECTED)
		return 0.0f;
	return m_joysticks[id].rudder;
}

math::Vector2 SDLJoystick::GetJoystickUV(const unsigned int id) const
{
	if (GetJoystickStatus(id) != GSJS_DETECTED)
		return math::Vector2();
	return m_joysticks[id].uv;
}

} // namespace gs2d
