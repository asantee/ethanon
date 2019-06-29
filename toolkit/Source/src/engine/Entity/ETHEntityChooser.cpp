#include "ETHEntityChooser.h"

#include <Platform/Platform.h>

ETHEntityNameChooser::ETHEntityNameChooser(const gs2d::str_type::string& name) :
	m_name(name)
{
}

bool ETHEntityNameChooser::Choose(ETHEntity* entity) const
{
	return (entity->GetEntityName() == m_name);
}

ETHEntityNameArrayChooser::ETHEntityNameArrayChooser(const std::vector<gs2d::str_type::string>& names, const bool isIgnoreList) :
	m_names(names),
	m_isIgnoreList(isIgnoreList)
{
}

ETHEntityNameArrayChooser::ETHEntityNameArrayChooser(const gs2d::str_type::string& semicolonSeparatedNames, const bool isIgnoreList) :
	m_isIgnoreList(isIgnoreList)
{
	m_names = Platform::SplitString(semicolonSeparatedNames, GS_L(";"));
}

bool ETHEntityNameArrayChooser::Choose(ETHEntity* entity) const
{
	const gs2d::str_type::string& entityName = entity->GetEntityName();
	for (std::size_t t = 0; t < m_names.size(); t++)
	{
		if (m_names[t] == entityName)
		{
			if (!m_isIgnoreList)
				return true;
			else
				return false;
		}
	}
	return m_isIgnoreList;
}

bool ETHEntityDefaultChooser::Choose(ETHEntity* entity) const
{
	GS2D_UNUSED_ARGUMENT(entity);
	return true;
}

ETHEntitySingleExceptionChooser::ETHEntitySingleExceptionChooser(const int id) : m_id(id)
{
}

bool ETHEntitySingleExceptionChooser::Choose(ETHEntity* entity) const
{
	return (m_id != entity->GetID());
}
