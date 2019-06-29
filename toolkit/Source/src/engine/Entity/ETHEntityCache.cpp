#include "ETHEntityCache.h"

#include "ETHEntityProperties.h"

const ETHEntityProperties* ETHEntityCache::Get(
	const str_type::string& fileName,
	const str_type::string& filePath,
	const Platform::FileManagerPtr& fileManager)
{
	std::map<str_type::string, ETHEntityProperties>::iterator iter = m_props.find(fileName);
	if (iter != m_props.end())
	{
		// instantiates a new light source so changes won't be global
		ETHEntityProperties* props = &(iter->second);
		if (props->light)
		{
			ETHLight* newLight = new ETHLight(props->light->IsActive());
			*newLight = *(props->light.get());
			props->light = boost::shared_ptr<ETHLight>(newLight);
		}
		return props;
	}
	else
	{
		const str_type::string fullFilePath = filePath + fileName;
		
		if (fileManager->FileExists(fullFilePath))
		{
			ETHEntityProperties props(fullFilePath, fileManager);
			if (props.IsSuccessfullyLoaded())
			{
				m_props[fileName] = props;
				return &(m_props[fileName]);
			}
		}
	}
	return 0;
}
