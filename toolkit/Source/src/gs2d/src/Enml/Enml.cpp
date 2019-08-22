#include "Enml.h"

#include <sstream>
#include <fstream>
#include <assert.h>
#include <stdio.h>

#ifdef _MSC_VER
  #define GS2D_SSCANF sscanf_s
#else
  #define GS2D_SSCANF sscanf
#endif

namespace gs2d {
namespace enml {

std::string GetStringFromAnsiFileC(const char* file)
{
	std::stringstream ss;
	std::ifstream f(file);
	if (f.is_open())
	{
		while(!f.eof())
		{
			char ch = ('\0');
			f.get(ch);
			if (ch != ('\0'))
			{
				ss << ch;
			}
		}
		f.close();
		return ss.str();
	}
	return ("");
}

std::string GetStringFromAnsiFile(const std::string &file)
{
	return GetStringFromAnsiFileC(file.c_str());
}

bool SaveStringToAnsiFileC(const char* fileName, const std::string &str)
{
	std::ofstream f(fileName);
	if (f.is_open())
	{
		f << str;
		f.close();
		return true;
	}
	return false;
}

bool SaveStringToAnsiFile(const std::string &file, const std::string &str)
{
	return SaveStringToAnsiFileC(file.c_str(), str);
}

/// Put a backslash before ';' and all single '\'.
static std::string FixBackslashes(std::string str)
{
	for (size_t t=0; t<str.size(); t++)
	{
		if (str[t] == (';') || str[t] == ('\\'))
		{
			str.insert(t, ("\\"));
			t++;
		}
	}
	return str;
}

/// Return TRUE if the character is neutral (if it represents no letter at all).
static RETURN_VALUE IsNeutral(const char c)
{
	return (c == 13 || c == (' ') || c == ('\n') || c == ('\r') || c == ('\t')) ? RV_TRUE : RV_FALSE;
}

/// Return TRUE if this character MAY be used in entity or attribute names.
static RETURN_VALUE IsValid(const char c)
{
	return ((c < ('a') || c > ('z')) && (c < ('A') || c > ('Z')) &&
		(c < ('0') || c > ('9')) && c != ('_')) ? RV_FALSE : RV_TRUE;
}

/// Return TRUE if the argument is a valid name.
static RETURN_VALUE IsValid(const std::string &str)
{
	const std::size_t size = str.size();
	for (unsigned int t = 0; t < size; t++)
	{
		const char c = str[t];
		if (IsNeutral(c) ||
			c == ('{') ||
			c == ('}'))
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

void Entity::Add(const std::string &key, std::string value)
{
	assert(IsValid(key) == RV_VALID);
	assert(value != (""));
	m_map[key] = value;
}

std::map<std::string, std::string>::const_iterator Entity::Begin() const
{
	return m_map.begin();
}

std::map<std::string, std::string>::const_iterator Entity::End() const
{
	return m_map.end();
}

std::string Entity::Get(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator iter = m_map.find(key);
	if (iter != m_map.end())
	{
		return iter->second;
	}
	return ("");
}

void Entity::GetAttributeNameList(std::list<std::string> &attribs) const
{
	if (!attribs.empty())
	{
		attribs.clear();
	}
	std::map<std::string, std::string>::const_iterator iter = Begin();
	for (; iter != End(); ++iter)
	{
		attribs.push_back(iter->first);
	}
}

std::string Entity::GetAttributeNames() const
{
	if (m_map.empty())
	{
		return ("");
	}
	std::stringstream ss;
	std::map<std::string, std::string>::const_iterator iter = Begin();
	while (iter != End())
	{
		ss << iter->first;
		if (++iter == End())
		{
			break;
		}
		else
		{
			ss << (",");
		}
	}
	return ss.str();
}

File::File() :
	m_ref(1)
{
}

File::File(const std::string &str)
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

Entity* File::GetEntity(const std::string &key)
{
	std::map<std::string, Entity>::iterator iter = m_entities.find(key);
	if (iter != m_entities.end())
	{
		return &(iter->second);
	}
	else
	{
		return 0;
	}
}

void File::AddEntity(const std::string &key, const Entity &entity)
{
	assert(IsValid(key) == RV_VALID);
	if (Exists(key))
	{
		m_entities[key].Clear();
	}
	std::map<std::string, std::string>::const_iterator iter;
	for (iter = entity.Begin(); iter != entity.End(); ++iter)
	{
		m_entities[key].Add(iter->first, iter->second);
	}
}

std::string File::GenerateString() const
{
	std::stringstream ss;
	std::map<std::string, Entity>::const_iterator entityIter = m_entities.begin();
	for (; entityIter != m_entities.end(); ++entityIter)
	{
		ss << entityIter->first << std::endl << ("{") << std::endl;
		std::map<std::string, std::string>::const_iterator iter;
		for (iter = entityIter->second.Begin(); iter != entityIter->second.End(); ++iter)
		{
			ss << ("\t") << iter->first << (" = ") << FixBackslashes(iter->second) << (";") << std::endl;
		}
		ss << ("}") << std::endl << std::endl;
	}
	return ss.str();
}

std::string File::GenerateDebugString() const
{
	std::stringstream ss;
	std::map<std::string, Entity>::const_iterator entityIter = m_entities.begin();
	unsigned int n = 0;
	for (; entityIter != m_entities.end(); ++entityIter)
	{
		n++;
		ss << ("-entity #") << n << (":") << std::endl << (" ") << entityIter->first << std::endl;
		std::map<std::string, std::string>::const_iterator iter;
		for (iter = entityIter->second.Begin(); iter != entityIter->second.End(); ++iter)
		{
			ss << ("\t") << iter->first << (": ") << iter->second << std::endl;
		}
		ss << std::endl << std::endl << std::endl;
	}
	return ss.str();
}

bool File::IsComment(const SEEK_STATUS& status)
{
	return (status == SS_COMMENT || status == SS_MULTILINE_COMMENT);
}

void File::AddValue(const std::string& entity, const std::string& attrib, const std::string& value)
{
	m_entities[entity].Add(attrib, value);
}

unsigned int File::ParseString(const std::string &str)
{
	Clear();
	m_error = RV_SUCCESS;
	SEEK_STATUS status = SS_ENTITY;
	SEEK_STATUS lastStatus = status;
	const std::size_t size = str.size();
	std::size_t line = 1;
	std::string entityName = (""), keyName = ("");
	Entity entity;

	for (std::size_t cursor = 0; cursor < size; cursor++)
	{
		if (str[cursor] == ('\n'))
		{
			line++;
			if (status == SS_COMMENT)
			{
				status = lastStatus;
			}
		} else if (status != SS_READ_VALUE && !IsComment(status) && str[cursor] == ('/'))
		{
			lastStatus = status;
			if (str.compare(cursor, 2, ("/*")) == 0)
				status = SS_MULTILINE_COMMENT;
			else
				status = SS_COMMENT;
		}
		if (status == SS_MULTILINE_COMMENT)
		{
			if (str.compare(cursor, 2, ("*/")) == 0)
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
				entityName = ReadName(&str[cursor], ('{'), ('/'), &cursor);
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
				if (str[cursor] == ('{'))
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
				
				if (str[cursor] == ('}'))
				{
					assert(entityName != (""));
					AddEntity(entityName, entity);
					entityName = ("");
					status = SS_ENTITY;
				}
				else
				{
					keyName = ReadName(&str[cursor], ('='), ('/'), &cursor);
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
				if (str[cursor] == ('='))
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
				std::string strValue = ReadValue(&str[cursor], &cursor);
				if (strValue != (""))
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

std::string File::GetErrorString() const
{
	switch (m_error)
	{
	case RV_SUCCESS:
		return ("Success");
	case RV_BRACKET_EXPECTED:
		return ("Expected '{' or '}'");
	case RV_INVALID_ENTITY_NAME:
		return ("The entity name is invalid");
	case RV_INVALID_ATTRIBUTE_NAME:
		return ("The attribute name is invalid");
	case RV_ASSIGN_OPERATOR_EXPECTED:
		return ("Expected '='");
	case RV_INVALID_VALUE:
		return ("Invalid value");
	}
	return ("");
}

bool File::Exists(const std::string &key) const
{
	std::map<std::string, Entity>::const_iterator iter = m_entities.find(key);
	if (iter == m_entities.end())
	{
		return false;
	}
	return true;
}

std::string File::Get(const std::string &entity, const std::string &attrib) const
{
	std::map<std::string, Entity>::const_iterator iter = m_entities.find(entity);
	if (iter == m_entities.end())
	{
		return ("");
	}
	return iter->second.Get(attrib);
}

bool File::GetDouble(const std::string &entity, const std::string &attrib,  double *p) const
{
	const std::string str = Get(entity, attrib);
	if (str == (""))
	{
		return false;
	}
	if (GS2D_SSCANF(str.c_str(), ("%lf"), p) < 1)
	{
		return false;
	}
	return true;
}

bool File::GetInt(const std::string &entity, const std::string &attrib,  int *p) const
{
	const std::string str = Get(entity, attrib);
	if (str == (""))
	{
		return false;
	}
	if (GS2D_SSCANF(str.c_str(), ("%d"), p) < 1)
	{
		return false;
	}
	return true;
}

bool File::GetUInt(const std::string &entity, const std::string &attrib, unsigned int *p) const
{
	const std::string str = Get(entity, attrib);
	if (str == (""))
	{
		return false;
	}
	if (GS2D_SSCANF(str.c_str(), ("%u"), p) < 1)
	{
		return false;
	}
	return true;
}

bool File::GetFloat(const std::string &entity, const std::string &attrib,  float *p) const
{
	const std::string str = Get(entity, attrib);
	if (str == (""))
	{
		return false;
	}
	if (GS2D_SSCANF(str.c_str(), ("%f"), p) < 1)
	{
		return false;
	}
	return true;
}

void File::GetEntityNameList(std::list<std::string> &entities) const
{
	if (!entities.empty())
	{
		entities.clear();
	}
	std::map<std::string, Entity>::const_iterator iter = m_entities.begin();
	for (; iter != m_entities.end(); ++iter)
	{
		entities.push_back(iter->first);
	}
}

std::string File::GetEntityNames() const
{
	if (m_entities.empty())
	{
		return ("");
	}
	std::stringstream ss;
	std::map<std::string, Entity>::const_iterator iter = m_entities.begin();
	while (iter != m_entities.end())
	{
		ss << iter->first;
		if (++iter == m_entities.end())
		{
			break;
		}
		else
		{
			ss << (",");
		}
	}
	return ss.str();
}

std::string File::GetAttributeNames(const std::string &key) const
{
	std::map<std::string, Entity>::const_iterator iter =
		m_entities.find(key);
	if (iter != m_entities.end())
	{
		return iter->second.GetAttributeNames();
	}
	return ("");
}

void File::WriteToFile(const std::string &fileName) const
{
	SaveStringToAnsiFile(fileName, GenerateString());
}

bool File::ParseFromFile(const std::string &fileName)
{
	return (ParseString(GetStringFromAnsiFile(fileName)) == RV_SUCCESS);

}

void File::Add(const std::string &entity, const std::string &attrib, const std::string &value)
{
	assert(IsValid(entity) == RV_VALID);
	assert(IsValid(attrib) == RV_VALID);
	m_entities[entity].Add(attrib, value);
}

unsigned int File::GetNumEntities() const
{
	return static_cast<unsigned int>(m_entities.size());
}

const std::map<std::string, Entity>& File::GetEntities() const
{
	return m_entities;
}

std::string File::ReadName(
	std::string str,
	const char nextValidToken,
	const char commentChar,
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
	return ("");
}

std::string File::ReadValue(std::string str, std::size_t *pCursor)
{
	for (std::size_t t = 0; t < str.size(); t++)
	{
		if (str[t] == ('\\'))
		{
			if (str[t+1] == (';'))
			{
				(*pCursor)++;
				str.erase(t, 1);
			} else if (str[t+1] == ('\\'))
			{
				(*pCursor)++;
				str.erase(t, 1);
			} else
			{
				return ("");
			}
		} else if (str[t] == (';'))
		{
			str.assign(str.c_str(), t);
			return str;
		}
		(*pCursor)++;
	}
	return ("");
}

} // enml
} // gs2d
