#include "ETHResourceLoader.h"

std::list<ETHResourceContainer*> ETHResourceLoader::m_containers;

void ETHResourceLoader::EnqueueResource(ETHResourceContainer* container)
{
	container->AddRef();
	m_containers.push_back(container);
}

void ETHResourceLoader::RecoverAll()
{
	for (std::list<ETHResourceContainer*>::iterator iter = m_containers.begin(); iter != m_containers.end(); ++iter)
	{
		(*iter)->RecoverResources();
		(*iter)->Release();
	}
	
	m_containers.clear();
}
