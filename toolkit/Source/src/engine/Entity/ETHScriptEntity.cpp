#include "ETHScriptEntity.h"

ETHScriptEntity::ETHScriptEntity() : 
	m_isAlive(true)
{
}

void ETHScriptEntity::Kill()
{
	m_isAlive = false;
}

bool ETHScriptEntity::IsAlive() const
{
	return m_isAlive;
}
