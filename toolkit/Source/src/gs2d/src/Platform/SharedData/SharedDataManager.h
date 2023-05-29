#ifndef SHARED_DATA_MANAGER_H_
#define SHARED_DATA_MANAGER_H_

#include "SharedData.h"

#include <hopscotch_map.h>

namespace Platform {

class SharedDataManager
{
	tsl::hopscotch_map<std::string, SharedDataPtr> m_data;

public:
	void Set(const std::string& key, const std::string& data);
	void SetSecured(const std::string& key, const std::string& data);
	std::string Get(const std::string& key, const std::string& defaultValue) const;
	bool Exists(const std::string& key) const;
	bool IsValid(const std::string& key) const;
	bool Remove(const std::string& key);
};

} // namespace Platform

#endif
