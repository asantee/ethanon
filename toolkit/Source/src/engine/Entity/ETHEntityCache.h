#ifndef ETH_ENTITY_CACHE_H_
#define ETH_ENTITY_CACHE_H_

#include "ETHEntityProperties.h"

#include <map>

class ETHEntityCache
{
public:
	const ETHEntityProperties* Get(const std::string& fileName, const std::string& filePath, const Platform::FileManagerPtr& fileManager);
private:
	std::map<std::string, ETHEntityProperties> m_props;
};

#endif
