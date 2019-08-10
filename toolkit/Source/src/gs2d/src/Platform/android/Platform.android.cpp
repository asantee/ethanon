#include "Platform.android.h"
#include "../Platform.h"
#include "../FileLogger.h"
#include "../../Video/GLES2/GLES2Video.h"
#include "AndroidLog.h"

namespace gs2d {

str_type::string Application::GetPlatformName()
{
	return "android";
}

void ShowMessage(str_type::stringstream& stream, const GS_MESSAGE_TYPE type)
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

gs2d::str_type::string FileLogger::GetLogDirectory()
{
	return GS_L("/sdcard/.ethanon/gs2dlog/");
}

gs2d::str_type::string GetModuleDirectory()
{
	return GS_L("");
}

char GetDirectorySlashA()
{
	return '/';
}

} // namespace Platform
