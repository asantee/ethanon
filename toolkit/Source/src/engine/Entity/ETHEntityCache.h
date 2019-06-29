#ifndef ETH_ENTITY_CACHE_H_
#define ETH_ENTITY_CACHE_H_

#include "ETHEntityProperties.h"

#include <map>

class ETHEntityCache
{
public:
	const ETHEntityProperties* Get(const str_type::string& fileName, const str_type::string& filePath, const Platform::FileManagerPtr& fileManager);
private:
	std::map<str_type::string, ETHEntityProperties> m_props;
};

#endif
