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
#include <list>
#include "../Types.h"

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

/// Return values.
enum RETURN_VALUE
{
	RV_FALSE = 0,
	RV_INVALID = 0,
	RV_TRUE = 1,
	RV_VALID = 1,
	RV_STOP_AT_TOKEN = 2,
};

/// Parsing errors.
enum ERROR_VALUE
{
	RV_SUCCESS = 0,
	RV_BRACKET_EXPECTED = 1, // The entity name has been declared but its body hasn't started with {
	RV_INVALID_ENTITY_NAME = 2, // The entity name is invalid (it probably uses invalid characters)
	RV_INVALID_ATTRIBUTE_NAME = 3, // The attribute name is invalid
	RV_ASSIGN_OPERATOR_EXPECTED = 4, // No assignment = operator has been found
	RV_INVALID_VALUE = 5, // The value is not valid. Either it's empty or it uses the backslash incorrectly
};

str_type::string GetStringFromAnsiFileC(const str_type::char_t *file);
str_type::string GetStringFromAnsiFile(const str_type::string &file);
bool SaveStringToAnsiFileC(const str_type::char_t *fileName, const str_type::string &str);
bool SaveStringToAnsiFile(const str_type::string &file, const str_type::string &str);

/// A data entity that holds attributes.
class Entity
{
public:
	/// Reference counter constructor.
	Entity();

	/// Adds a reference count.
	void AddRef();

	/// Releases a reference.
	void Release();

	/// Clear the entity to start it all over again.
	void Clear();

	/// Add an attribute to the entity.
	void Add(const str_type::string &key, str_type::string value);

	/// Returns the first constant iterator.
	std::map<str_type::string, str_type::string>::const_iterator Begin() const;

	/// Returns the last constant iterator.
	std::map<str_type::string, str_type::string>::const_iterator End() const;

	/// Returns the value of attribute named 'key'. Returns an empty string if there's no attribute with that name.
	str_type::string Get(const str_type::string &key) const;

	/// Fills the list passed as argument with all attributes names.
	void GetAttributeNameList(std::list<str_type::string> &attribs) const;

	// Returns a string containing all attributes in the entity separated by commas.
	str_type::string GetAttributeNames() const;

private:
	std::map<str_type::string, str_type::string> m_map;
	int m_ref; // reference counter
};

/// File class that holds many entities and writes all its content to string or parses a string to Entity objects.
class File
{
public:
	/// Default "do nothing" constructor.
	File();

	/// Reference counter constructor.
	File(const str_type::string &str);

	/// Adds a reference count.
	void AddRef();

	/// Releases a reference.
	void Release();

	/// Clear the whole object.
	void Clear();

	/// Returns a copy of the requested entity.
	Entity* GetEntity(const str_type::string &key);

	/// Adds an Entity object to the file. Overwrites existing entities.
	void AddEntity(const str_type::string &key, const Entity &entity);

	/// Generates a parseable/readable string of the entire file.
	str_type::string GenerateString() const;

	/// Generates a slightly different string for debuging. The difference is that it supresses the syntactic ; and {} symbols.
	str_type::string GenerateDebugString() const;

	/// Reading status for the parser.
	enum SEEK_STATUS
	{
		SS_ENTITY = 0,
		SS_BEGIN_ENTITY = 1,
		SS_ATTRIBUTE_KEY = 2,
		SS_ASSIGN = 3,
		SS_READ_VALUE = 4,
		SS_END_ENTITY = 5,
		SS_COMMENT = 6,
		SS_MULTILINE_COMMENT = 7
	};

	static bool IsComment(const SEEK_STATUS& status);

	/// Add a value directly to the entity.
	void AddValue(const str_type::string& entity, const str_type::string& attrib, const str_type::string& value);

	/// Parses a string to a map of Entity objects. If an error occurs, it returns the line of the error.
	unsigned int ParseString(const str_type::string &str);

	/// Returns the ID of the last parsing error.
	ERROR_VALUE GetError() const;

	/// Returns the last error string.
	str_type::string GetErrorString() const;

	/// Returns true if the entity exists.
	bool Exists(const str_type::string &key) const;

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an empty string if there's no attribute or entity with that name.
	str_type::string Get(const str_type::string &entity, const str_type::string &attrib) const;

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an false if there's no attribute or entity with that name.
	bool GetDouble(const str_type::string &entity, const str_type::string &attrib,  double *p) const;

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an false if there's no attribute or entity with that name.
	bool GetInt(const str_type::string &entity, const str_type::string &attrib,  int *p) const;

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an false if there's no attribute or entity with that name.
	bool GetUInt(const str_type::string &entity, const str_type::string &attrib, unsigned int *p) const;

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an false if there's no attribute or entity with that name.
	bool GetFloat(const str_type::string &entity, const str_type::string &attrib,  float *p) const;

	/// Fills the list passed as argument with all entity names.
	void GetEntityNameList(std::list<str_type::string> &entities) const;

	/// Returns a string containing all entity names in the file separated by commas.
	str_type::string GetEntityNames() const;

	/// Returns a string containing all attributes in the entity. Separated by commas.
	str_type::string GetAttributeNames(const str_type::string &key) const;

	/// Saves the object's content to a file.
	void WriteToFile(const str_type::string &fileName) const;

	/// Reads a file and parses it to the object.
	bool ParseFromFile(const str_type::string &fileName);

	/// Adds an attribute to an entity.
	void Add(const str_type::string &entity, const str_type::string &attrib, const str_type::string &value);

	unsigned int GetNumEntities() const;

	const std::map<str_type::string, Entity>& GetEntities() const;

private:

	/// Reads and returns an entity or attribute name
	str_type::string ReadName(str_type::string str, const str_type::char_t nextValidToken, const str_type::char_t commentChar, std::size_t *pCursor);

	/// Reads an attribute value and erases their syntactic backslashes.
	str_type::string ReadValue(str_type::string str, std::size_t *pCursor);

	std::map<str_type::string, Entity> m_entities;
	int m_ref; // reference counter
	ERROR_VALUE m_error; // last parsing error
};

} // namespace enml
} // namespace gs2d

#endif
