#include "ETHScriptWrapper.h"

bool ETHScriptWrapper::SetSharedData(const std::string& key, const std::string& data)
{
	return Application::SharedData.Set(key, data);
}

bool ETHScriptWrapper::IsSharedDataConstant(const std::string& key)
{
	return Application::SharedData.IsConstant(key);
}

std::string ETHScriptWrapper::GetSharedData(const std::string& key)
{
	return Application::SharedData.Get(key);
}

bool ETHScriptWrapper::SharedDataExists(const std::string& key)
{
	return Application::SharedData.Exists(key);
}

bool ETHScriptWrapper::RemoveSharedData(const std::string& key)
{
	return Application::SharedData.Remove(key);
}
