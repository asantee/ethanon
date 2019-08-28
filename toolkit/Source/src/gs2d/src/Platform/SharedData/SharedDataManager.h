#ifndef SHARED_DATA_MANAGER_H_
#define SHARED_DATA_MANAGER_H_

#include "SharedData.h"

#include <hopscotch_map.h>

namespace Platform {

class SharedDataManager
{
	tsl::hopscotch_map<std::string, SharedDataPtr> m_data;

	bool Set(const std::string& key, const std::string& data, const bool forceValue);

public:
	void Create(const std::string& key, const std::string& data, const bool constant);
	void Force(const std::string& key, const std::string& data);
	bool Set(const std::string& key, const std::string& data);
	bool IsConstant(const std::string& key) const;
	std::string Get(const std::string& key) const;
	bool Exists(const std::string& key) const;
	bool Remove(const std::string& key);
};

} // namespace Platform

#endif
