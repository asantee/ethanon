#include "ETHScriptWrapper.h"

void ETHScriptWrapper::SetSharedData(const std::string& key, const std::string& data)
{
	Application::SharedData.Set(key, data);
}

std::string ETHScriptWrapper::GetSharedData(const std::string& key, const std::string& defaultValue)
{
	return Application::SharedData.Get(key, defaultValue);
}

void ETHScriptWrapper::SetSecuredSharedData(const std::string& key, const std::string& data)
{
	Application::SharedData.SetSecured(key, data);
}

bool ETHScriptWrapper::IsSharedDataValid(const std::string& key)
{
	return Application::SharedData.IsValid(key);
}

bool ETHScriptWrapper::SharedDataExists(const std::string& key)
{
	return Application::SharedData.Exists(key);
}

bool ETHScriptWrapper::RemoveSharedData(const std::string& key)
{
	return Application::SharedData.Remove(key);
}
