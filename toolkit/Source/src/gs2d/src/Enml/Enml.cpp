#include "Enml.h"

#include <sstream>
#include <fstream>
#include <assert.h>
#include <stdio.h>

namespace gs2d {
namespace enml {

str_type::string GetStringFromAnsiFileC(const str_type::char_t *file)
{
	str_type::stringstream ss;
	str_type::ifstream f(file);
	if (f.is_open())
	{
		while(!f.eof())
		{
			str_type::char_t ch = GS_L('\0');
			f.get(ch);
			if (ch != GS_L('\0'))
			{
				ss << ch;
			}
		}
		f.close();
		return ss.str();
	}
	return GS_L("");
}

str_type::string GetStringFromAnsiFile(const str_type::string &file)
{
	return GetStringFromAnsiFileC(file.c_str());
}

bool SaveStringToAnsiFileC(const str_type::char_t *fileName, const str_type::string &str)
{
	str_type::ofstream f(fileName);
	if (f.is_open())
	{
		f << str;
		f.close();
		return true;
	}
	return false;
}

bool SaveStringToAnsiFile(const str_type::string &file, const str_type::string &str)
{
	return SaveStringToAnsiFileC(file.c_str(), str);
}

/// Put a backslash before ';' and all single '\'.
static str_type::string FixBackslashes(str_type::string str)
{
	for (size_t t=0; t<str.size(); t++)
	{
		if (str[t] == GS_L(';') || str[t] == GS_L('\\'))
		{
			str.insert(t, GS_L("\\"));
			t++;
		}
	}
	return str;
}

/// Return TRUE if the character is neutral (if it represents no letter at all).
static RETURN_VALUE IsNeutral(const str_type::char_t c)
{
	return (c == 13 || c == GS_L(' ') || c == GS_L('\n') || c == GS_L('\r') || c == GS_L('\t')) ? RV_TRUE : RV_FALSE;
}

/// Return TRUE if this character MAY be used in entity or attribute names.
static RETURN_VALUE IsValid(const str_type::char_t c)
{
	return ((c < GS_L('a') || c > GS_L('z')) && (c < GS_L('A') || c > GS_L('Z')) &&
		(c < GS_L('0') || c > GS_L('9')) && c != GS_L('_')) ? RV_FALSE : RV_TRUE;
}

/// Return TRUE if the argument is a valid name.
static RETURN_VALUE IsValid(const str_type::string &str)
{
	const std::size_t size = str.size();
	for (unsigned int t = 0; t < size; t++)
	{
		const str_type::char_t c = str[t];
		if (IsNeutral(c) ||
			c == GS_L('{') ||
			c == GS_L('}'))
		{
			return RV_STOP_AT_TOKEN;
		}

		if (!IsValid(c))
		{
			return RV_INVALID;
		}
	}
	return RV_VALID;
}

Entity::Entity() :
	m_ref(1)
{
}

void Entity::AddRef()
{
	m_ref++;
}

void Entity::Release()
{
	if(--m_ref == 0)
	{
		delete this;
	}
}

void Entity::Clear()
{
	m_map.clear();
}

void Entity::Add(const str_type::string &key, str_type::string value)
{
	assert(IsValid(key) == RV_VALID);
	assert(value != GS_L(""));
	m_map[key] = value;
}

std::map<str_type::string, str_type::string>::const_iterator Entity::Begin() const
{
	return m_map.begin();
}

std::map<str_type::string, str_type::string>::const_iterator Entity::End() const
{
	return m_map.end();
}

str_type::string Entity::Get(const str_type::string &key) const
{
	std::map<str_type::string, str_type::string>::const_iterator iter = m_map.find(key);
	if (iter != m_map.end())
	{
		return iter->second;
	}
	return GS_L("");
}

void Entity::GetAttributeNameList(std::list<str_type::string> &attribs) const
{
	if (!attribs.empty())
	{
		attribs.clear();
	}
	std::map<str_type::string, str_type::string>::const_iterator iter = Begin();
	for (; iter != End(); ++iter)
	{
		attribs.push_back(iter->first);
	}
}

str_type::string Entity::GetAttributeNames() const
{
	if (m_map.empty())
	{
		return GS_L("");
	}
	str_type::stringstream ss;
	std::map<str_type::string, str_type::string>::const_iterator iter = Begin();
	while (iter != End())
	{
		ss << iter->first;
		if (++iter == End())
		{
			break;
		}
		else
		{
			ss << GS_L(",");
		}
	}
	return ss.str();
}

File::File() :
	m_ref(1)
{
}

File::File(const str_type::string &str)
{
	m_ref = 1;
	ParseString(str);
}

void File::AddRef()
{
	m_ref++;
}

void File::Release()
{
	if(--m_ref == 0)
	{
		delete this;
	}
}

void File::Clear()
{
	m_entities.clear();
}

Entity* File::GetEntity(const str_type::string &key)
{
	std::map<str_type::string, Entity>::iterator iter = m_entities.find(key);
	if (iter != m_entities.end())
	{
		return &(iter->second);
	}
	else
	{
		return 0;
	}
}

void File::AddEntity(const str_type::string &key, const Entity &entity)
{
	assert(IsValid(key) == RV_VALID);
	if (Exists(key))
	{
		m_entities[key].Clear();
	}
	std::map<str_type::string, str_type::string>::const_iterator iter;
	for (iter = entity.Begin(); iter != entity.End(); ++iter)
	{
		m_entities[key].Add(iter->first, iter->second);
	}
}

str_type::string File::GenerateString() const
{
	str_type::stringstream ss;
	std::map<str_type::string, Entity>::const_iterator entityIter = m_entities.begin();
	for (; entityIter != m_entities.end(); ++entityIter)
	{
		ss << entityIter->first << std::endl << GS_L("{") << std::endl;
		std::map<str_type::string, str_type::string>::const_iterator iter;
		for (iter = entityIter->second.Begin(); iter != entityIter->second.End(); ++iter)
		{
			ss << GS_L("\t") << iter->first << GS_L(" = ") << FixBackslashes(iter->second) << GS_L(";") << std::endl;
		}
		ss << GS_L("}") << std::endl << std::endl;
	}
	return ss.str();
}

str_type::string File::GenerateDebugString() const
{
	str_type::stringstream ss;
	std::map<str_type::string, Entity>::const_iterator entityIter = m_entities.begin();
	unsigned int n = 0;
	for (; entityIter != m_entities.end(); ++entityIter)
	{
		n++;
		ss << GS_L("-entity #") << n << GS_L(":") << std::endl << GS_L(" ") << entityIter->first << std::endl;
		std::map<str_type::string, str_type::string>::const_iterator iter;
		for (iter = entityIter->second.Begin(); iter != entityIter->second.End(); ++iter)
		{
			ss << GS_L("\t") << iter->first << GS_L(": ") << iter->second << std::endl;
		}
		ss << std::endl << std::endl << std::endl;
	}
	return ss.str();
}

bool File::IsComment(const SEEK_STATUS& status)
{
	return (status == SS_COMMENT || status == SS_MULTILINE_COMMENT);
}

void File::AddValue(const str_type::string& entity, const str_type::string& attrib, const str_type::string& value)
{
	m_entities[entity].Add(attrib, value);
}

unsigned int File::ParseString(const str_type::string &str)
{
	Clear();
	m_error = RV_SUCCESS;
	SEEK_STATUS status = SS_ENTITY;
	SEEK_STATUS lastStatus = status;
	const std::size_t size = str.size();
	std::size_t line = 1;
	str_type::string entityName = GS_L(""), keyName = GS_L("");
	Entity entity;

	for (std::size_t cursor = 0; cursor < size; cursor++)
	{
		if (str[cursor] == GS_L('\n'))
		{
			line++;
			if (status == SS_COMMENT)
			{
				status = lastStatus;
			}
		} else if (status != SS_READ_VALUE && !IsComment(status) && str[cursor] == GS_L('/'))
		{
			lastStatus = status;
			if (str.compare(cursor, 2, GS_L("/*")) == 0)
				status = SS_MULTILINE_COMMENT;
			else
				status = SS_COMMENT;
		}
		if (status == SS_MULTILINE_COMMENT)
		{
			if (str.compare(cursor, 2, GS_L("*/")) == 0)
			{
				++cursor;
				status = lastStatus;
				continue;
			}
		}

		if (IsComment(status))
		{
			continue;
		} else if (status == SS_ENTITY)
		{
			if (!IsNeutral(str[cursor]))
			{
				entityName = ReadName(&str[cursor], GS_L('{'), GS_L('/'), &cursor);
				if (IsValid(entityName))
				{
					status = SS_BEGIN_ENTITY;
				}
				else
				{
					Clear();
					m_error = RV_INVALID_ENTITY_NAME;
					return static_cast<unsigned int>(line);
				}
			}
		} else if (status == SS_BEGIN_ENTITY)
		{
			if (!IsNeutral(str[cursor]))
			{
				if (str[cursor] == GS_L('{'))
				{
					status = SS_ATTRIBUTE_KEY;
					entity.Clear();
				}
				else
				{
					Clear();
					m_error = RV_BRACKET_EXPECTED;
					return static_cast<unsigned int>(line);
				}
			}
		} else if (status == SS_ATTRIBUTE_KEY)
		{
			if (!IsNeutral(str[cursor]))
			{
				
				if (str[cursor] == GS_L('}'))
				{
					assert(entityName != GS_L(""));
					AddEntity(entityName, entity);
					entityName = GS_L("");
					status = SS_ENTITY;
				}
				else
				{
					keyName = ReadName(&str[cursor], GS_L('='), GS_L('/'), &cursor);
					if (IsValid(keyName))
					{
						status = SS_ASSIGN;
					}
					else
					{
						Clear();
						m_error = RV_INVALID_ATTRIBUTE_NAME;
						return static_cast<unsigned int>(line);
					}
				}
			}
		} else if (status == SS_ASSIGN)
		{
			if (!IsNeutral(str[cursor]))
			{
				if (str[cursor] == GS_L('='))
				{
					status = SS_READ_VALUE;
				}
				else
				{
					Clear();
					m_error = RV_ASSIGN_OPERATOR_EXPECTED;
					return static_cast<unsigned int>(line);
				}
			}
		} else if (status == SS_READ_VALUE)
		{
			if (!IsNeutral(str[cursor]))
			{
				str_type::string strValue = ReadValue(&str[cursor], &cursor);
				if (strValue != GS_L(""))
				{
					status = SS_ATTRIBUTE_KEY;
					entity.Add(keyName, strValue);
				}
				else
				{
					Clear();
					m_error = RV_INVALID_VALUE;
					return static_cast<unsigned int>(line);
				}
			}
		}
	}
	return RV_SUCCESS; // no error: return anything
}

ERROR_VALUE File::GetError() const
{
	return m_error;
}

str_type::string File::GetErrorString() const
{
	switch (m_error)
	{
	case RV_SUCCESS:
		return GS_L("Success");
	case RV_BRACKET_EXPECTED:
		return GS_L("Expected '{' or '}'");
	case RV_INVALID_ENTITY_NAME:
		return GS_L("The entity name is invalid");
	case RV_INVALID_ATTRIBUTE_NAME:
		return GS_L("The attribute name is invalid");
	case RV_ASSIGN_OPERATOR_EXPECTED:
		return GS_L("Expected '='");
	case RV_INVALID_VALUE:
		return GS_L("Invalid value");
	}
	return GS_L("");
}

bool File::Exists(const str_type::string &key) const
{
	std::map<str_type::string, Entity>::const_iterator iter = m_entities.find(key);
	if (iter == m_entities.end())
	{
		return false;
	}
	return true;
}

str_type::string File::Get(const str_type::string &entity, const str_type::string &attrib) const
{
	std::map<str_type::string, Entity>::const_iterator iter = m_entities.find(entity);
	if (iter == m_entities.end())
	{
		return GS_L("");
	}
	return iter->second.Get(attrib);
}

bool File::GetDouble(const str_type::string &entity, const str_type::string &attrib,  double *p) const 
{
	const str_type::string str = Get(entity, attrib);
	if (str == GS_L(""))
	{
		return false;
	}
	if (GS2D_SSCANF(str.c_str(), GS_L("%lf"), p) < 1)
	{
		return false;
	}
	return true;
}

bool File::GetInt(const str_type::string &entity, const str_type::string &attrib,  int *p) const 
{
	const str_type::string str = Get(entity, attrib);
	if (str == GS_L(""))
	{
		return false;
	}
	if (GS2D_SSCANF(str.c_str(), GS_L("%d"), p) < 1)
	{
		return false;
	}
	return true;
}

bool File::GetUInt(const str_type::string &entity, const str_type::string &attrib, unsigned int *p) const 
{
	const str_type::string str = Get(entity, attrib);
	if (str == GS_L(""))
	{
		return false;
	}
	if (GS2D_SSCANF(str.c_str(), GS_L("%u"), p) < 1)
	{
		return false;
	}
	return true;
}

bool File::GetFloat(const str_type::string &entity, const str_type::string &attrib,  float *p) const 
{
	const str_type::string str = Get(entity, attrib);
	if (str == GS_L(""))
	{
		return false;
	}
	if (GS2D_SSCANF(str.c_str(), GS_L("%f"), p) < 1)
	{
		return false;
	}
	return true;
}

void File::GetEntityNameList(std::list<str_type::string> &entities) const
{
	if (!entities.empty())
	{
		entities.clear();
	}
	std::map<str_type::string, Entity>::const_iterator iter = m_entities.begin();
	for (; iter != m_entities.end(); ++iter)
	{
		entities.push_back(iter->first);
	}
}

str_type::string File::GetEntityNames() const
{
	if (m_entities.empty())
	{
		return GS_L("");
	}
	str_type::stringstream ss;
	std::map<str_type::string, Entity>::const_iterator iter = m_entities.begin();
	while (iter != m_entities.end())
	{
		ss << iter->first;
		if (++iter == m_entities.end())
		{
			break;
		}
		else
		{
			ss << GS_L(",");
		}
	}
	return ss.str();
}

str_type::string File::GetAttributeNames(const str_type::string &key) const
{
	std::map<str_type::string, Entity>::const_iterator iter =
		m_entities.find(key);
	if (iter != m_entities.end())
	{
		return iter->second.GetAttributeNames();
	}
	return GS_L("");
}

void File::WriteToFile(const str_type::string &fileName) const
{
	SaveStringToAnsiFile(fileName, GenerateString());
}

bool File::ParseFromFile(const str_type::string &fileName)
{
	return (ParseString(GetStringFromAnsiFile(fileName)) == RV_SUCCESS);

}

void File::Add(const str_type::string &entity, const str_type::string &attrib, const str_type::string &value)
{
	assert(IsValid(entity) == RV_VALID);
	assert(IsValid(attrib) == RV_VALID);
	m_entities[entity].Add(attrib, value);
}

unsigned int File::GetNumEntities() const
{
	return static_cast<unsigned int>(m_entities.size());
}

const std::map<str_type::string, Entity>& File::GetEntities() const
{
	return m_entities;
}

str_type::string File::ReadName(
	str_type::string str,
	const str_type::char_t nextValidToken,
	const str_type::char_t commentChar,
	std::size_t *pCursor)
{
	for (std::size_t t = 0; t < str.size(); t++)
	{
		if (str[t] == commentChar || str[t] == nextValidToken || IsNeutral(str[t]))
		{
			str.assign(str.c_str(), t);
			(*pCursor)--;
			return str;
		}
		(*pCursor)++;
	}
	return GS_L("");
}

str_type::string File::ReadValue(str_type::string str, std::size_t *pCursor)
{
	for (std::size_t t = 0; t < str.size(); t++)
	{
		if (str[t] == GS_L('\\'))
		{
			if (str[t+1] == GS_L(';'))
			{
				(*pCursor)++;
				str.erase(t, 1);
			} else if (str[t+1] == GS_L('\\'))
			{
				(*pCursor)++;
				str.erase(t, 1);
			} else
			{
				return GS_L("");
			}
		} else if (str[t] == GS_L(';'))
		{
			str.assign(str.c_str(), t);
			return str;
		}
		(*pCursor)++;
	}
	return GS_L("");
}

} // enml
} // gs2d
