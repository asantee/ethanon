#include "ETHResourceLoader.h"

std::list<ETHResourceContainer*> ETHResourceLoader::m_containers;
unsigned long ETHResourceLoader::m_desiredFPSWhileLoading = 30;

void ETHResourceLoader::EnqueueResource(ETHResourceContainer* container)
{
	container->AddRef();
	m_containers.push_back(container);
}

bool ETHResourceLoader::DoResourceRecoverStep(const ETHResourceProviderPtr& provider)
{
	if (m_containers.empty())
		return false;
	
	provider->Log("STARTING DoResourceRecoverStep -----------------------------------------------------", Platform::Logger::LT_INFO);

	const unsigned long maxTimeToWork = 1000 / m_desiredFPSWhileLoading;

	VideoPtr video = provider->GetVideo();

	const unsigned long startTime = video->GetElapsedTime();
	while ((video->GetElapsedTime() - startTime) < maxTimeToWork)
	{
		std::list<ETHResourceContainer*>::iterator iter = m_containers.begin();
		if (iter != m_containers.end())
		{
			(*iter)->RecoverResources();
			(*iter)->Release();
			m_containers.pop_front();
		}
		else
		{
			break;
		}
	}

	provider->Log("-------------------------------- ENDED DoResourceRecoverStep", Platform::Logger::LT_INFO);

	const bool emptied = m_containers.empty();
	if (emptied)
	{
		provider->Log("-------------------------------- EMPTY DoResourceRecoverStep", Platform::Logger::LT_INFO);
	}

	return !emptied;
}

bool ETHResourceLoader::HasContainerEnqueued()
{
	return !m_containers.empty();
}
