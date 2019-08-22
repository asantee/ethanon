#include <iostream>

#include "Platform.linux.h"

#include "../Platform.h"

#include "../../Application.h"

void gs2d::ShowMessage(std::stringstream& stream, const GS_MESSAGE_TYPE type)
{
	#warning todo
}

std::string gs2d::Application::GetPlatformName()
{
	return "linux";
}

namespace Platform {

std::string ResourceDirectory()
{
	#warning todo
	return "";
}

std::string GlobalExternalStorageDirectory()
{
	#warning todo
	return "";
}

std::string ExternalStorageDirectory()
{
	#warning todo
	return "";
}

} // namespace
