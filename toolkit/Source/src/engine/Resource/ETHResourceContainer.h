#ifndef ETH_RESOURCE_CONTAINER_H_
#define ETH_RESOURCE_CONTAINER_H_

#include <cstdint>

#include "ETHResourceProvider.h"

class ETHResourceContainer
{
public:
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	
	virtual void RecoverResources() = 0;
};

class ETHSpriteContainer : public ETHResourceContainer
{
	mutable int32_t m_ref;
	ETHResourceProviderPtr m_provider;
	std::string m_name;
	
public:
	ETHSpriteContainer(const ETHResourceProviderPtr& provider, const std::string& name);
	
	void AddRef() override;
	void Release() override;
	void RecoverResources() override;
};

#endif
