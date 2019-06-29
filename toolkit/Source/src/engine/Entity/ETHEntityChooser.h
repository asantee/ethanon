#ifndef ETH_ENTITY_CHOOSER_H_
#define ETH_ENTITY_CHOOSER_H_

#include "ETHEntity.h"
#include <vector>

class ETHEntityChooser
{
public:
	virtual bool Choose(ETHEntity* entity) const = 0;
};

class ETHEntityDefaultChooser : public ETHEntityChooser
{
public:
	bool Choose(ETHEntity* entity) const;
};

class ETHEntityNameChooser : public ETHEntityChooser
{
	gs2d::str_type::string m_name;
public:
	ETHEntityNameChooser(const gs2d::str_type::string& name);
	bool Choose(ETHEntity* entity) const;
};

class ETHEntityNameArrayChooser : public ETHEntityChooser
{
	std::vector<gs2d::str_type::string> m_names;
	bool m_isIgnoreList;
public:
	ETHEntityNameArrayChooser(const std::vector<gs2d::str_type::string>& names, const bool isIgnoreList);
	ETHEntityNameArrayChooser(const gs2d::str_type::string& semicolonSeparatedNames, const bool isIgnoreList);
	bool Choose(ETHEntity* entity) const;
};

class ETHEntitySingleExceptionChooser : public ETHEntityChooser
{
	int m_id;
public:
	ETHEntitySingleExceptionChooser(const int id);
	bool Choose(ETHEntity* entity) const;
};

#endif
