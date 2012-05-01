/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#include "ETHEntityChooser.h"


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
	m_names = ETHGlobal::SplitString(semicolonSeparatedNames, GS_L(";"));
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
