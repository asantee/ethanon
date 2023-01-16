#include "ETHResourceContainer.h"

ETHSpriteContainer::ETHSpriteContainer(const ETHResourceProviderPtr& provider, const std::string& name) :
	m_ref(1),
	m_provider(provider),
	m_name(name)
{
}

void ETHSpriteContainer::AddRef()
{
	++m_ref;
}

void ETHSpriteContainer::Release()
{
	if (--m_ref == 0)
	{
		delete this;
	}
}

void ETHSpriteContainer::RecoverResources()
{
	const std::string resourceDirectory = m_provider->GetFileIOHub()->GetResourceDirectory();
	const std::string path = resourceDirectory + m_name;

	m_provider->GetGraphicResourceManager()->AddFile(
		m_provider->GetFileManager(),
		m_provider->GetVideo(),
		path,
		resourceDirectory,
		false /*temporary*/);
}
