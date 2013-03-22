/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#include "Application.h"

namespace gs2d {

using namespace math;

Platform::SharedDataManager Application::SharedData;

GS2D_API unsigned long ComputeElapsedTime(ApplicationPtr app)
{
	return static_cast<unsigned long>(ComputeElapsedTimeF(app));
}

GS2D_API float ComputeElapsedTimeF(ApplicationPtr app)
{
	static float lastTime = 0;
	const float currentTime = app->GetElapsedTimeF(Application::TU_SECONDS);
	float elapsedTime = currentTime - lastTime;

	// if the timer had been reset, the elapsed time will be negative,
	// since it can't be, lets set it to an acceptable value
	if (elapsedTime <= 0.0f)
	{
		elapsedTime = 1.0f / 60.0f;
	}

	lastTime = currentTime;
	return elapsedTime * 1000.0f;
}

void ShowMessage(const str_type::string& str, const GS_MESSAGE_TYPE type)
{
	str_type::stringstream ss;
	ss << str;
	ShowMessage(ss, type);
}

void Application::SetScreenSizeChangeListener(const ScreenSizeChangeListenerPtr& listener)
{
	m_screenSizeChangeListener = listener;
}

} // namespace gs2d

