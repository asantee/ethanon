#include "SharedDataManager.h"

namespace Platform {

void SharedDataManager::Set(const std::string& key, const std::string& data)
{
	tsl::hopscotch_map<std::string, SharedDataPtr>::const_iterator iter = m_data.find(key);
	if (iter != m_data.end())
	{
		iter->second->Set(data);
	}
	else
	{
		SharedDataPtr newData(new SharedData());
		newData->Set(data);
		m_data[key] = newData;
	}
}

void SharedDataManager::SetSecured(const std::string& key, const std::string& data)
{
	// always overwrite secured data
	SharedDataPtr newData(new SharedDataSecured(data));
	m_data[key] = newData;
}

std::string SharedDataManager::Get(const std::string& key, const std::string& defaultValue) const
{
	tsl::hopscotch_map<std::string, SharedDataPtr>::const_iterator iter = m_data.find(key);
	if (iter != m_data.end())
		return iter->second->Get();
	else
		return defaultValue;
}

bool SharedDataManager::IsValid(const std::string& key) const
{
	tsl::hopscotch_map<std::string, SharedDataPtr>::const_iterator iter = m_data.find(key);
	if (iter != m_data.end())
		return iter->second->IsValid();
	else
		return true; // doesn't exist. Should default to true since it hasn't been compromised
}

bool SharedDataManager::Exists(const std::string& key) const
{
	return (m_data.find(key) != m_data.end());
}

bool SharedDataManager::Remove(const std::string& key)
{
	tsl::hopscotch_map<std::string, SharedDataPtr>::iterator iter = m_data.find(key);
	if (iter != m_data.end())
	{
		m_data.erase(iter);
		return true;
	}
	return false;
}

} // namespace Platform
