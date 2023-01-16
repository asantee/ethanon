#ifndef ETH_RESOURCE_LOADER_H_
#define ETH_RESOURCE_LOADER_H_

#include "ETHResourceContainer.h"

#include "ETHResourceProvider.h"

#include <list>

class ETHResourceLoader
{
	static std::list<ETHResourceContainer*> m_containers;
	static unsigned long m_desiredFPSWhileLoading;

public:
	static void EnqueueResource(ETHResourceContainer* container);
	static bool DoResourceRecoverStep(const ETHResourceProviderPtr& provider);
	static bool HasContainerEnqueued();
};

#endif
