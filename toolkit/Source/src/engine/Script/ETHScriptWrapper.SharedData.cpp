#include "ETHScriptWrapper.h"

bool ETHScriptWrapper::SetSharedData(const gs2d::str_type::string& key, const gs2d::str_type::string& data)
{
	return Application::SharedData.Set(key, data);
}

bool ETHScriptWrapper::IsSharedDataConstant(const gs2d::str_type::string& key)
{
	return Application::SharedData.IsConstant(key);
}

gs2d::str_type::string ETHScriptWrapper::GetSharedData(const gs2d::str_type::string& key)
{
	return Application::SharedData.Get(key);
}

bool ETHScriptWrapper::SharedDataExists(const gs2d::str_type::string& key)
{
	return Application::SharedData.Exists(key);
}

bool ETHScriptWrapper::RemoveSharedData(const gs2d::str_type::string& key)
{
	return Application::SharedData.Remove(key);
}
