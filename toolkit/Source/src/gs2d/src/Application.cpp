#include "Application.h"

namespace gs2d {

using namespace math;

Platform::SharedDataManager Application::SharedData;

unsigned long ComputeElapsedTime(ApplicationPtr app)
{
	return static_cast<unsigned long>(ComputeElapsedTimeF(app));
}

float ComputeElapsedTimeF(ApplicationPtr app)
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

void ShowMessage(const std::string& str, const GS_MESSAGE_TYPE type)
{
	std::stringstream ss;
	ss << str;
	ShowMessage(ss, type);
}

void Application::SetScreenSizeChangeListener(const ScreenSizeChangeListenerPtr& listener)
{
	m_screenSizeChangeListener = listener;
}

void Application::SetFileOpenListener(const FileOpenListenerPtr& listener)
{
	m_fileOpenListener = listener;
}

} // namespace gs2d
