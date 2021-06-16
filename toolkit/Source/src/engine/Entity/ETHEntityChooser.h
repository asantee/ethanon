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
	std::string m_name;
public:
	ETHEntityNameChooser(const std::string& name);
	bool Choose(ETHEntity* entity) const;
};

class ETHEntityNameArrayChooser : public ETHEntityChooser
{
	std::vector<std::string> m_names;
public:
	ETHEntityNameArrayChooser(const std::vector<std::string>& names);
	ETHEntityNameArrayChooser(const std::string& semicolonSeparatedNames);
	bool Choose(ETHEntity* entity) const;
};

class ETHEntityNameArrayIgnoreListChooser : public ETHEntityChooser
{
	std::vector<std::string> m_names;
public:
	ETHEntityNameArrayIgnoreListChooser(const std::vector<std::string>& names);
	ETHEntityNameArrayIgnoreListChooser(const std::string& semicolonSeparatedNames);
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
