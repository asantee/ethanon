#ifndef ETH_RESOURCE_CONTAINER_H_
#define ETH_RESOURCE_CONTAINER_H_

class ETHResourceContainer
{
public:
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	
	virtual void RecoverResources() = 0;
};

#endif
