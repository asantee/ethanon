#include "Platform.android.h"
#include "../Platform.h"
#include "../FileLogger.h"
#include "../../Video/GLES2/GLES2Video.h"
#include "AndroidLog.h"

namespace gs2d {

std::string Application::GetPlatformName()
{
	return "android";
}

void ShowMessage(std::stringstream& stream, const GS_MESSAGE_TYPE type)
{
	if (type == GSMT_INFO)
	{
		LOGI("%s", stream.str().c_str());
	}
	else if (type == GSMT_WARNING)
	{
		LOGI("%s", stream.str().c_str());
	}
	else if (type == GSMT_ERROR)
	{
		LOGE("%s", stream.str().c_str());
	}
}

} // namespace gs2d

namespace Platform {

std::string FileLogger::GetLogDirectory()
{
	return ("/sdcard/.ethanon/gs2dlog/");
}

std::string GetModuleDirectory()
{
	return ("");
}

char GetDirectorySlashA()
{
	return '/';
}

} // namespace Platform
