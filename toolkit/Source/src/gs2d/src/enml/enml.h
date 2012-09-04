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

#ifndef ENML_H_
#define ENML_H_

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include <list>
#include "../gs2dtypes.h"

namespace gs2d {

/*
Things you must remember:
	-Use '\\' and '\;' to insert '\' and ';' characters into attribute values
	-Use '/' for comments

Things it can't do:
	-It may not have an entity inside another entity. All entities MUST be global.
	-Values starting with '\n'
	-Can't receive attributes with empty values
*/

// ENML stands for Easy Non-Markup Language
namespace enml {

#ifdef _MSC_VER
	#ifdef GS2D_STR_TYPE_ANSI
		#define ENML_SSCANF sscanf_s
	#else
		#define ENML_SSCANF swscanf_s
	#endif
#else
	#ifdef GS2D_STR_TYPE_ANSI
		#define ENML_SSCANF sscanf
	#else
		#define ENML_SSCANF swscanf
	#endif
#endif

/// Return values.
enum returnValue
{
	enmlrvFALSE = 0,
	enmlrvINVALID = 0,
	enmlrvTRUE = 1,
	enmlrvVALID = 1,
	enmlrvSTOP_AT_TOKEN = 2,
};

/// Parsing errors.
enum errorValue
{
	enmlevSUCCESS = 0,
	enmlevBRACKET_EXPECTED = 1, // The entity name has been declared but its body hasn't been started with {
	enmlevINVALID_ENTITY_NAME = 2, // The entity name is invalid (it probably uses invalid characters)
	enmlevINVALID_ATTRIBUTE_NAME = 3, // The attribute name is invalid
	enmlevASSIGN_OPERATOR_EXPECTED = 4, // No assing = operator has been found
	enmlevINVALID_VALUE = 5, // The value is not valid. Either it's empty or it uses the backslash incorrectly
};

/// Load a text file and return its content as a string.
inline str_type::string getStringFromAnsiFileC(const str_type::char_t *file)
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
inline str_type::string getStringFromAnsiFile(const str_type::string &file)
{
	return getStringFromAnsiFileC(file.c_str());
}

/// Saves a string to a text file.
inline bool saveStringToAnsiFileC(const str_type::char_t *fileName, const str_type::string &str)
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
inline bool saveStringToAnsiFile(const str_type::string &file, const str_type::string &str)
{
	return saveStringToAnsiFileC(file.c_str(), str);
}

/// Put a backslash before ';' and single '\'.
inline str_type::string fixBackslashes(str_type::string str)
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
inline returnValue isNeutral(const str_type::char_t c)
{
	return (c == 13 || c == GS_L(' ') || c == GS_L('\n') || c == GS_L('\r') || c == GS_L('\t')) ? enmlrvTRUE : enmlrvFALSE;
}

/// Return TRUE if this character MAY be used in entity or attribute names.
inline returnValue isValid(const str_type::char_t c)
{
	return ((c < GS_L('a') || c > GS_L('z')) && (c < GS_L('A') || c > GS_L('Z')) &&
		(c < GS_L('0') || c > GS_L('9')) && c != GS_L('_')) ? enmlrvFALSE : enmlrvTRUE;
}

/// Return TRUE if the argument is a valid name.
inline returnValue isValid(const str_type::string &str)
{
	const std::size_t size = str.size();
	for (unsigned int t = 0; t < size; t++)
	{
		const str_type::char_t c = str[t];
		if (isNeutral(c) ||
			c == GS_L('{') ||
			c == GS_L('}'))
		{
			return enmlrvSTOP_AT_TOKEN;
		}

		if (!isValid(c))
		{
			return enmlrvINVALID;
		}
	}
	return enmlrvVALID;
}

/// A data entity that holds attributes.
class Entity
{
public:
	/// Reference counter constructor.
	Entity()
	{
		m_ref = 1;
	}

	/// Adds a reference count.
	void addRef()
	{
		m_ref++;
	}

	/// Releases a reference.
	void release()
	{
		if(--m_ref == 0)
		{
			delete this;
		}
	}

	/// Clear the entity to start it all over again.
	void clear()
	{
		m_map.clear();
	}

	/// Add an attribute to the entity.
	void add(const str_type::string &key, str_type::string value)
	{
		assert(isValid(key) == enmlrvVALID);
		assert(value != GS_L(""));
		m_map[key] = value;
	}

	/// Returns the first constant iterator.
	std::map<str_type::string, str_type::string>::const_iterator begin() const
	{
		return m_map.begin();
	}

	/// Returns the last constant iterator.
	std::map<str_type::string, str_type::string>::const_iterator end() const
	{
		return m_map.end();
	}

	/// Returns the value of attribute named 'key'. Returns an empty string if there's no attribute with that name.
	str_type::string get(const str_type::string &key) const
	{
		std::map<str_type::string, str_type::string>::const_iterator iter =
			m_map.find(key);
		if (iter != m_map.end())
		{
			return iter->second;
		}
		return GS_L("");
	}

	/// Fills the list passed as argument with all attributes names.
	void getAttributeNameList(std::list<str_type::string> &attribs) const
	{
		if (!attribs.empty())
		{
			attribs.clear();
		}
		std::map<str_type::string, str_type::string>::const_iterator iter = begin();
		for (; iter != end(); ++iter)
		{
			attribs.push_back(iter->first);
		}
	}

	// Returns a string containing all attributes in the entity separated by commas.
	str_type::string getAttributeNames() const
	{
		if (m_map.empty())
		{
			return GS_L("");
		}
		str_type::stringstream ss;
		std::map<str_type::string, str_type::string>::const_iterator iter = begin();
		while (iter != end())
		{
			ss << iter->first;
			if (++iter == end())
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

private:
	std::map<str_type::string, str_type::string> m_map;
	int m_ref; // reference counter
};

/// File class that holds many entities and writes all its content to string or parses a string to Entity objects.
class File
{
public:
	/// Default "do nothing" constructor.
	File()
	{
		m_ref = 1;
	}

	/// Reference counter constructor.
	File(const str_type::string &str)
	{
		m_ref = 1;
		parseString(str);
	}

	/// Adds a reference count.
	void addRef()
	{
		m_ref++;
	}

	/// Releases a reference.
	void release()
	{
		if(--m_ref == 0)
		{
			delete this;
		}
	}

	/// Clear the whole object.
	void clear()
	{
		m_entities.clear();
	}

	/// Returns a copy of the requested entity.
	Entity* getEntity(const str_type::string &key)
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

	/// Adds an Entity object to the file. Overwrites existing entities.
	void addEntity(const str_type::string &key, const Entity &entity)
	{
		assert(isValid(key) == enmlrvVALID);
		if (exists(key))
		{
			m_entities[key].clear();
		}
		std::map<str_type::string, str_type::string>::const_iterator iter;
		for (iter = entity.begin(); iter != entity.end(); ++iter)
		{
			m_entities[key].add(iter->first, iter->second);
		}
	}

	/// Generates a parseable/readable string of the entire file.
	str_type::string generateString() const
	{
		str_type::stringstream ss;
		std::map<str_type::string, Entity>::const_iterator entityIter = m_entities.begin();
		for (; entityIter != m_entities.end(); ++entityIter)
		{
			ss << entityIter->first << std::endl << GS_L("{") << std::endl;
			std::map<str_type::string, str_type::string>::const_iterator iter;
			for (iter = entityIter->second.begin(); iter != entityIter->second.end(); ++iter)
			{
				ss << GS_L("\t") << iter->first << GS_L(" = ") << fixBackslashes(iter->second) << GS_L(";") << std::endl;
			}
			ss << GS_L("}") << std::endl << std::endl;
		}
		return ss.str();
	}

	/// Generates a slightly different string for debuging. The difference is that it supresses the syntactic ; and {} symbols.
	str_type::string generateDebugString() const
	{
		str_type::stringstream ss;
		std::map<str_type::string, Entity>::const_iterator entityIter = m_entities.begin();
		unsigned int n = 0;
		for (; entityIter != m_entities.end(); ++entityIter)
		{
			n++;
			ss << GS_L("-entity #") << n << GS_L(":") << std::endl << GS_L(" ") << entityIter->first << std::endl;
			std::map<str_type::string, str_type::string>::const_iterator iter;
			for (iter = entityIter->second.begin(); iter != entityIter->second.end(); ++iter)
			{
				ss << GS_L("\t") << iter->first << GS_L(": ") << iter->second << std::endl;
			}
			ss << std::endl << std::endl << std::endl;
		}
		return ss.str();
	}

	/// Reading status for the parser.
	enum seekStatus
	{
		enmlssENTITY = 0,
		enmlssBEGIN_ENTITY = 1,
		enmlssATTRIBUTE_KEY = 2,
		enmlssASSIGN = 3,
		enmlssREAD_VALUE = 4,
		enmlssEND_ENTITY = 5,
		enmlssCOMMENT = 6,
		enmlssMULTILINE_COMMENT = 7
	};

	static bool isComment(const seekStatus& status)
	{
		return (status == enmlssCOMMENT || status == enmlssMULTILINE_COMMENT);
	}

	/// Add a value directly to the entity.
	void addValue(const str_type::string& entity, const str_type::string& attrib, const str_type::string& value)
	{
		m_entities[entity].add(attrib, value);
	}

	/// Parses a string to a map of Entity objects. If an error occurs, it returns the line of the error.
	unsigned int parseString(const str_type::string &str)
	{
		clear();
		m_error = enmlevSUCCESS;
		seekStatus status = enmlssENTITY;
		seekStatus lastStatus = status;
		const unsigned int size = str.size();
		unsigned int line = 1;
		str_type::string entityName = GS_L(""), keyName = GS_L("");
		Entity entity;

		for (unsigned int cursor=0; cursor<size; cursor++)
		{
			if (str[cursor] == GS_L('\n'))
			{
				line++;
				if (status == enmlssCOMMENT)
				{
					status = lastStatus;
				}
			} else if (status != enmlssREAD_VALUE && !isComment(status) && str[cursor] == GS_L('/'))
			{
				lastStatus = status;
				if (str.compare(cursor, 2, GS_L("/*")) == 0)
					status = enmlssMULTILINE_COMMENT;
				else
					status = enmlssCOMMENT;
			}
			if (status == enmlssMULTILINE_COMMENT)
			{
				if (str.compare(cursor, 2, GS_L("*/")) == 0)
				{
					++cursor;
					status = lastStatus;
					continue;
				}
			}

			if (isComment(status))
			{
				continue;
			} else if (status == enmlssENTITY)
			{
				if (!isNeutral(str[cursor]))
				{
					entityName = readName(&str[cursor], GS_L('{'), GS_L('/'), &cursor);
					if (isValid(entityName))
					{
						status = enmlssBEGIN_ENTITY;
					}
					else
					{
						clear();
						m_error = enmlevINVALID_ENTITY_NAME;
						return line;
					}
				}
			} else if (status == enmlssBEGIN_ENTITY)
			{
				if (!isNeutral(str[cursor]))
				{
					if (str[cursor] == GS_L('{'))
					{
						status = enmlssATTRIBUTE_KEY;
						entity.clear();
					}
					else
					{
						clear();
						m_error = enmlevBRACKET_EXPECTED;
						return line;
					}
				}
			} else if (status == enmlssATTRIBUTE_KEY)
			{
				if (!isNeutral(str[cursor]))
				{
					
					if (str[cursor] == GS_L('}'))
					{
						assert(entityName != GS_L(""));
						addEntity(entityName, entity);
						entityName = GS_L("");
						status = enmlssENTITY;
					}
					else
					{
						keyName = readName(&str[cursor], GS_L('='), GS_L('/'), &cursor);
						if (isValid(keyName))
						{
							status = enmlssASSIGN;
						}
						else
						{
							clear();
							m_error = enmlevINVALID_ATTRIBUTE_NAME;
							return line;
						}
					}
				}
			} else if (status == enmlssASSIGN)
			{
				if (!isNeutral(str[cursor]))
				{
					if (str[cursor] == GS_L('='))
					{
						status = enmlssREAD_VALUE;
					}
					else
					{
						clear();
						m_error = enmlevASSIGN_OPERATOR_EXPECTED;
						return line;
					}
				}
			} else if (status == enmlssREAD_VALUE)
			{
				if (!isNeutral(str[cursor]))
				{
					str_type::string strValue = readValue(&str[cursor], &cursor);
					if (strValue != GS_L(""))
					{
						status = enmlssATTRIBUTE_KEY;
						entity.add(keyName, strValue);
					}
					else
					{
						clear();
						m_error = enmlevINVALID_VALUE;
						return line;
					}
				}
			}
		}
		return enmlevSUCCESS; // no error: return anything
	}

	/// Returns the ID of the last parsing error.
	errorValue getError() const
	{
		return m_error;
	}

	/// Returns the last error string.
	str_type::string getErrorString() const
	{
		switch (m_error)
		{
		case enmlevSUCCESS:
			return GS_L("Success");
		case enmlevBRACKET_EXPECTED:
			return GS_L("Expected '{' or '}'");
		case enmlevINVALID_ENTITY_NAME:
			return GS_L("The entity name is invalid");
		case enmlevINVALID_ATTRIBUTE_NAME:
			return GS_L("The attribute name is invalid");
		case enmlevASSIGN_OPERATOR_EXPECTED:
			return GS_L("Expected '='");
		case enmlevINVALID_VALUE:
			return GS_L("Invalid value");
		}
		return GS_L("");
	}

	/// Returns true if the entity exists.
	bool exists(const str_type::string &key) const
	{
		std::map<str_type::string, Entity>::const_iterator iter = m_entities.find(key);
		if (iter == m_entities.end())
		{
			return false;
		}
		return true;
	}

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an empty string if there's no attribute or entity with that name.
	str_type::string get(const str_type::string &entity, const str_type::string &attrib) const
	{
		std::map<str_type::string, Entity>::const_iterator iter = m_entities.find(entity);
		if (iter == m_entities.end())
		{
			return GS_L("");
		}
		return iter->second.get(attrib);
	}

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an false if there's no attribute or entity with that name.
	bool getDouble(const str_type::string &entity, const str_type::string &attrib,  double *p) const 
	{
		const str_type::string str = get(entity, attrib);
		if (str == GS_L(""))
		{
			return false;
		}
		if (ENML_SSCANF(str.c_str(), GS_L("%lf"), p) < 1)
		{
			return false;
		}
		return true;
	}

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an false if there's no attribute or entity with that name.
	bool getInt(const str_type::string &entity, const str_type::string &attrib,  int *p) const 
	{
		const str_type::string str = get(entity, attrib);
		if (str == GS_L(""))
		{
			return false;
		}
		if (ENML_SSCANF(str.c_str(), GS_L("%d"), p) < 1)
		{
			return false;
		}
		return true;
	}

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an false if there's no attribute or entity with that name.
	bool getUInt(const str_type::string &entity, const str_type::string &attrib, unsigned int *p) const 
	{
		const str_type::string str = get(entity, attrib);
		if (str == GS_L(""))
		{
			return false;
		}
		if (ENML_SSCANF(str.c_str(), GS_L("%u"), p) < 1)
		{
			return false;
		}
		return true;
	}

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an false if there's no attribute or entity with that name.
	bool getFloat(const str_type::string &entity, const str_type::string &attrib,  float *p) const 
	{
		const str_type::string str = get(entity, attrib);
		if (str == GS_L(""))
		{
			return false;
		}
		if (ENML_SSCANF(str.c_str(), GS_L("%f"), p) < 1)
		{
			return false;
		}
		return true;
	}

	/// Fills the list passed as argument with all entity names.
	void getEntityNameList(std::list<str_type::string> &entities) const
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

	/// Returns a string containing all entity names in the file separated by commas.
	str_type::string getEntityNames() const
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

	/// Returns a string containing all attributes in the entity. Separated by commas.
	str_type::string getAttributeNames(const str_type::string &key) const
	{
		std::map<str_type::string, Entity>::const_iterator iter =
			m_entities.find(key);
		if (iter != m_entities.end())
		{
			return iter->second.getAttributeNames();
		}
		return GS_L("");
	}

	/// Saves the object's content to a file.
	void writeToFile(const str_type::string &fileName) const
	{
		saveStringToAnsiFile(fileName, generateString());
	}

	/// Reads a file and parses it to the object.
	bool parseFromFile(const str_type::string &fileName)
	{
		return (parseString(getStringFromAnsiFile(fileName)) == enmlevSUCCESS);

	}

	/// Adds an attribute to an entity.
	void add(const str_type::string &entity, const str_type::string &attrib, const str_type::string &value)
	{
		assert(isValid(entity) == enmlrvVALID);
		assert(isValid(attrib) == enmlrvVALID);
		m_entities[entity].add(attrib, value);
	}

	unsigned int getNumEntities() const
	{
		return m_entities.size();
	}

	const std::map<str_type::string, Entity>& getEntities() const
	{
		return m_entities;
	}

private:

	/// Reads and returns an entity or attribute name
	str_type::string readName(str_type::string str, const str_type::char_t nextValidToken, const str_type::char_t commentChar, unsigned int *pCursor)
	{
		for (unsigned int t=0; t<str.size(); t++)
		{
			if (str[t] == commentChar || str[t] == nextValidToken || isNeutral(str[t]))
			{
				str.assign(str.c_str(), t);
				(*pCursor)--;
				return str;
			}
			(*pCursor)++;
		}
		return GS_L("");
	}

	/// Reads an attribute value and erases their syntactic backslashes.
	str_type::string readValue(str_type::string str, unsigned int *pCursor)
	{
		for (unsigned int t=0; t<str.size(); t++)
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

	std::map<str_type::string, Entity> m_entities;
	int m_ref; // reference counter
	errorValue m_error; // last parsing error
};

} // namespace enml
} // namespace gs2d

#endif