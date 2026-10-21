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
	// double: a float counting seconds since start moves in 1 ms steps after 2.3 h and 4 ms steps after 9 h,
	// so shorter frames read as 0, and replacing those with 1/60 s made the game run at fps/60 times its speed
	static double lastTime = 0.0;
	const double currentTime = app->GetElapsedTimeD(Application::TU_SECONDS);
	double elapsedTime = currentTime - lastTime;

	// if the timer had been reset, the elapsed time will be negative,
	// since it can't be, lets set it to an acceptable value
	if (elapsedTime < 0.0)
	{
		elapsedTime = 1.0 / 60.0;
	}

	lastTime = currentTime;
	return static_cast<float>(elapsedTime * 1000.0);
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
