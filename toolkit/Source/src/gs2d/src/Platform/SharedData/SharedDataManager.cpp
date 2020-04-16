#include "SharedDataManager.h"

namespace Platform {

void SharedDataManager::Create(const std::string& key, const std::string& data, const bool constant)
{
	SharedDataPtr newData(new SharedData(constant));
	newData->Set(data, true);
	m_data[key] = newData;
}

void SharedDataManager::Force(const std::string& key, const std::string& data)
{
	Set(key, data, true);
}

bool SharedDataManager::Set(const std::string& key, const std::string& data)
{
	return Set(key, data, false);
}

bool SharedDataManager::Set(const std::string& key, const std::string& data, const bool forceValue)
{
	tsl::hopscotch_map<std::string, SharedDataPtr>::const_iterator iter = m_data.find(key);
	if (iter != m_data.end())
	{
		return iter->second->Set(data, forceValue);
	}
	else
	{
		Create(key, data, false);
		return true;
	}
}

bool SharedDataManager::IsConstant(const std::string& key) const
{
	tsl::hopscotch_map<std::string, SharedDataPtr>::const_iterator iter = m_data.find(key);
	if (iter != m_data.end())
		return iter->second->IsConstant();
	else
		return false;
}

std::string SharedDataManager::Get(const std::string& key) const
{
	tsl::hopscotch_map<std::string, SharedDataPtr>::const_iterator iter = m_data.find(key);
	if (iter != m_data.end())
		return iter->second->Get();
	else
		return "";
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
		if (!iter->second->IsConstant())
		{
			m_data.erase(iter);
			return true;
		}
	}
	return false;
}

} // namespace Platform
