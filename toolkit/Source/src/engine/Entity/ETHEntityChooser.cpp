#include "ETHEntityChooser.h"

#include <Platform/Platform.h>

#define GS2D_UNUSED_ARGUMENT(argument) ((void)(argument))

ETHEntityNameChooser::ETHEntityNameChooser(const std::string& name) :
	m_name(name)
{
}

bool ETHEntityNameChooser::Choose(ETHEntity* entity) const
{
	return (entity->GetEntityName() == m_name);
}

ETHEntityNameArrayChooser::ETHEntityNameArrayChooser(const std::vector<std::string>& names) :
	m_names(names)
{
}

ETHEntityNameArrayChooser::ETHEntityNameArrayChooser(const std::string& semicolonSeparatedNames)
{
	m_names = Platform::SplitString(semicolonSeparatedNames, (";"));
}

bool ETHEntityNameArrayChooser::Choose(ETHEntity* entity) const
{
	const std::string& entityName = entity->GetEntityName();
	for (std::size_t t = 0; t < m_names.size(); t++)
	{
		if (m_names[t] == entityName)
		{
			return true;
		}
	}
	return false;
}

ETHEntityNameArrayIgnoreListChooser::ETHEntityNameArrayIgnoreListChooser(const std::vector<std::string>& names) :
	m_names(names)
{
}

ETHEntityNameArrayIgnoreListChooser::ETHEntityNameArrayIgnoreListChooser(const std::string& semicolonSeparatedNames)
{
	m_names = Platform::SplitString(semicolonSeparatedNames, (";"));
}

bool ETHEntityNameArrayIgnoreListChooser::Choose(ETHEntity* entity) const
{
	const std::string& entityName = entity->GetEntityName();
	for (std::size_t t = 0; t < m_names.size(); t++)
	{
		if (m_names[t] == entityName)
		{
			return false;
		}
	}
	return true;
}

bool ETHEntityDefaultChooser::Choose(ETHEntity* entity) const
{
	UNUSED_ARGUMENT(entity);
	return true;
}

ETHEntitySingleExceptionChooser::ETHEntitySingleExceptionChooser(const int id) : m_id(id)
{
}

bool ETHEntitySingleExceptionChooser::Choose(ETHEntity* entity) const
{
	return (m_id != entity->GetID());
}
