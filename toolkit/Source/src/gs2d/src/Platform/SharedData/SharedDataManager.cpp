#include "SharedDataManager.h"

namespace Platform {

void SharedDataManager::Create(const gs2d::str_type::string& key, const gs2d::str_type::string& data, const bool constant)
{
	SharedDataPtr newData(new SharedData(constant));
	newData->Set(data, true);
	m_data[key] = newData;
}

void SharedDataManager::Force(const gs2d::str_type::string& key, const gs2d::str_type::string& data)
{
	Set(key, data, true);
}

bool SharedDataManager::Set(const gs2d::str_type::string& key, const gs2d::str_type::string& data)
{
	return Set(key, data, false);
}

bool SharedDataManager::Set(const gs2d::str_type::string& key, const gs2d::str_type::string& data, const bool forceValue)
{
	tsl::hopscotch_map<gs2d::str_type::string, SharedDataPtr>::const_iterator iter = m_data.find(key);
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

bool SharedDataManager::IsConstant(const gs2d::str_type::string& key) const
{
	tsl::hopscotch_map<gs2d::str_type::string, SharedDataPtr>::const_iterator iter = m_data.find(key);
	if (iter != m_data.end())
		return iter->second->IsConstant();
	else
		return false;
}

gs2d::str_type::string SharedDataManager::Get(const gs2d::str_type::string& key) const
{
	tsl::hopscotch_map<gs2d::str_type::string, SharedDataPtr>::const_iterator iter = m_data.find(key);
	if (iter != m_data.end())
		return iter->second->Get();
	else
		return GS_L("");
}

bool SharedDataManager::Exists(const gs2d::str_type::string& key) const
{
	return (m_data.find(key) != m_data.end());
}

bool SharedDataManager::Remove(const gs2d::str_type::string& key)
{
	tsl::hopscotch_map<gs2d::str_type::string, SharedDataPtr>::iterator iter = m_data.find(key);
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
