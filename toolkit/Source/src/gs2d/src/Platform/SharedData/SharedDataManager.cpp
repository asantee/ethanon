/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

    Permission is hereby granted, free of charge, to any person obtaining a copy of this
    software and associated documentation files (the "Software"), to deal in the
    Software without restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so, subject to the
    following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
    PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

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
	std::map<gs2d::str_type::string, SharedDataPtr>::const_iterator iter = m_data.find(key);
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
	std::map<gs2d::str_type::string, SharedDataPtr>::const_iterator iter = m_data.find(key);
	if (iter != m_data.end())
		return iter->second->IsConstant();
	else
		return false;
}

gs2d::str_type::string SharedDataManager::Get(const gs2d::str_type::string& key) const
{
	std::map<gs2d::str_type::string, SharedDataPtr>::const_iterator iter = m_data.find(key);
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
	std::map<gs2d::str_type::string, SharedDataPtr>::iterator iter = m_data.find(key);
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
