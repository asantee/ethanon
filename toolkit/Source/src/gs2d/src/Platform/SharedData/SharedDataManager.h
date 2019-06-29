#ifndef SHARED_DATA_MANAGER_H_
#define SHARED_DATA_MANAGER_H_

#include "SharedData.h"

#include "../../../../tsl/hopscotch_map.h"

namespace Platform {

class SharedDataManager
{
	tsl::hopscotch_map<gs2d::str_type::string, SharedDataPtr> m_data;

	bool Set(const gs2d::str_type::string& key, const gs2d::str_type::string& data, const bool forceValue);

public:
	void Create(const gs2d::str_type::string& key, const gs2d::str_type::string& data, const bool constant);
	void Force(const gs2d::str_type::string& key, const gs2d::str_type::string& data);
	bool Set(const gs2d::str_type::string& key, const gs2d::str_type::string& data);
	bool IsConstant(const gs2d::str_type::string& key) const;
	gs2d::str_type::string Get(const gs2d::str_type::string& key) const;
	bool Exists(const gs2d::str_type::string& key) const;
	bool Remove(const gs2d::str_type::string& key);
};

} // namespace Platform

#endif
