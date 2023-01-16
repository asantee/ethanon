#ifndef ETH_RESOURCE_LOADER_H_
#define ETH_RESOURCE_LOADER_H_

#include "ETHResourceContainer.h"

#include <list>

class ETHResourceLoader
{
	static std::list<ETHResourceContainer*> m_containers;

public:
	static void EnqueueResource(ETHResourceContainer* container);
	static void RecoverAll();
};

#endif
