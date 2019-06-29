#include "RecoverableResourceManager.h"

namespace gs2d {

void RecoverableResourceManager::InsertRecoverableResource(RecoverableResource* resource)
{
	m_resources.push_back(resource);
}

bool RecoverableResourceManager::RemoveRecoverableResource(const RecoverableResource* resource)
{
	for (std::list<RecoverableResource*>::iterator iter = m_resources.begin(); iter != m_resources.end();)
	{
		if ((*iter) == resource)
			iter = m_resources.erase(iter);
		else
			++iter;
	}
	return true;
}

void RecoverableResourceManager::RecoverAll()
{
	for (std::list<RecoverableResource*>::iterator iter = m_resources.begin(); iter != m_resources.end(); ++iter)
	{
		(*iter)->Recover();
	}
}

} // namespace gs2d
