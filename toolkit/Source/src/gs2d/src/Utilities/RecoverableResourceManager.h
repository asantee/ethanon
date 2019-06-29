#ifndef GS2D_RECOVERABLE_RESOURCE_MANAGER_H_
#define GS2D_RECOVERABLE_RESOURCE_MANAGER_H_

#include "RecoverableResource.h"
#include <list>

namespace gs2d {

class RecoverableResourceManager
{
	std::list<RecoverableResource*> m_resources;
public:
	void InsertRecoverableResource(RecoverableResource* resource);
	bool RemoveRecoverableResource(const RecoverableResource* resource);
	void RecoverAll();
};

} // namespace gs2d

#endif
