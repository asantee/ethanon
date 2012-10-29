/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/
 
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

#include "SDLInput.h"

namespace gs2d {

bool SDLInput::IsKeyDown(const GS_KEY key) const
{
	return false;
}

GS_KEY_STATE SDLInput::GetKeyState(const GS_KEY key) const
{
	return GSKS_UP;
}

GS_KEY_STATE SDLInput::GetLeftClickState() const
{
	return GSKS_UP;
}

GS_KEY_STATE SDLInput::GetRightClickState() const
{
	return GSKS_UP;
}

GS_KEY_STATE SDLInput::GetMiddleClickState() const
{
	return GSKS_UP;
}

math::Vector2i SDLInput::GetMouseMove() const
{
	return math::Vector2i();
}

math::Vector2 SDLInput::GetMouseMoveF() const
{
	return math::Vector2();
}

math::Vector2 SDLInput::GetTouchPos(const unsigned int n, WindowPtr pWindow) const
{
	return math::Vector2();
}

GS_KEY_STATE  SDLInput::GetTouchState(const unsigned int n, WindowPtr pWindow) const
{
	return GSKS_UP;
}

unsigned int SDLInput::GetMaxTouchCount() const
{
	return 0;
}

math::Vector2 SDLInput::GetTouchMove(const unsigned int n) const
{
	return math::Vector2();
}

bool SDLInput::SetCursorPosition(math::Vector2i v2Pos)
{
	return false;
}

bool SDLInput::SetCursorPositionF(math::Vector2 v2Pos)
{
	return false;
}

math::Vector2i SDLInput::GetCursorPosition(WindowPtr pWindow) const
{
	return math::Vector2i();
}

math::Vector2 SDLInput::GetCursorPositionF(WindowPtr pWindow) const
{
	return math::Vector2();
}

unsigned int SDLInput::GetMaxJoysticks() const
{
	return 0;
}

float SDLInput::GetWheelState() const
{
	return 0.0f;
}

bool SDLInput::Update()
{
	return false;
}

void SDLInput::ShowJoystickWarnings(const bool enable)
{
}

bool SDLInput::IsShowingJoystickWarnings() const
{
	return false;
}

str_type::char_t SDLInput::GetLastCharInput() const
{
	return '\0';
}

GS_KEY_STATE SDLInput::GetJoystickButtonState(const unsigned int id, const GS_JOYSTICK_BUTTON key) const
{
	return GSKS_UP;
}

bool SDLInput::IsJoystickButtonDown(const unsigned int id, const GS_JOYSTICK_BUTTON key) const
{
	return false;
}

bool SDLInput::DetectJoysticks()
{
	return false;
}

GS_JOYSTICK_STATUS SDLInput::GetJoystickStatus(const unsigned int id) const
{
	return GSJS_NOTDETECTED;
}

unsigned int SDLInput::GetNumJoyButtons(const unsigned int id) const
{
	return 0;
}

math::Vector2 SDLInput::GetJoystickXY(const unsigned int id) const
{
	return math::Vector2();
}

float SDLInput::GetJoystickZ(const unsigned int id) const
{
	return 0.0f;
}

float SDLInput::GetJoystickRudder(const unsigned int id) const
{
	return 0.0f;
}

math::Vector2 SDLInput::GetJoystickUV(const unsigned int id) const
{
	return math::Vector2();
}

GS_JOYSTICK_BUTTON SDLInput::GetFirstButtonDown(const unsigned int id) const
{
	return GSB_NONE;
}

unsigned int SDLInput::GetNumJoysticks() const
{
	return 0;
}

math::Vector3 SDLInput::GetAccelerometerData() const
{
	return math::Vector3();
}

} // namespace gs2d
