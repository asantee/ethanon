#ifndef ENML_H_
#define ENML_H_

#include <map>
#include <list>

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

std::string GetStringFromAnsiFileC(const char* file);
std::string GetStringFromAnsiFile(const std::string& file);
bool SaveStringToAnsiFileC(const char* fileName, const std::string& str);
bool SaveStringToAnsiFile(const std::string& file, const std::string& str);

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
	void Add(const std::string &key, std::string value);

	/// Returns the first constant iterator.
	std::map<std::string, std::string>::const_iterator Begin() const;

	/// Returns the last constant iterator.
	std::map<std::string, std::string>::const_iterator End() const;

	/// Returns the value of attribute named 'key'. Returns an empty string if there's no attribute with that name.
	std::string Get(const std::string &key) const;

	/// Fills the list passed as argument with all attributes names.
	void GetAttributeNameList(std::list<std::string> &attribs) const;

	// Returns a string containing all attributes in the entity separated by commas.
	std::string GetAttributeNames() const;

private:
	std::map<std::string, std::string> m_map;
	int m_ref; // reference counter
};

/// File class that holds many entities and writes all its content to string or parses a string to Entity objects.
class File
{
public:
	/// Default "do nothing" constructor.
	File();

	/// Reference counter constructor.
	File(const std::string &str);

	/// Adds a reference count.
	void AddRef();

	/// Releases a reference.
	void Release();

	/// Clear the whole object.
	void Clear();

	/// Returns a copy of the requested entity.
	Entity* GetEntity(const std::string &key);

	/// Adds an Entity object to the file. Overwrites existing entities.
	void AddEntity(const std::string &key, const Entity &entity);

	/// Generates a parseable/readable string of the entire file.
	std::string GenerateString() const;

	/// Generates a slightly different string for debuging. The difference is that it supresses the syntactic ; and {} symbols.
	std::string GenerateDebugString() const;

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
	void AddValue(const std::string& entity, const std::string& attrib, const std::string& value);

	/// Parses a string to a map of Entity objects. If an error occurs, it returns the line of the error.
	unsigned int ParseString(const std::string &str);

	/// Returns the ID of the last parsing error.
	ERROR_VALUE GetError() const;

	/// Returns the last error string.
	std::string GetErrorString() const;

	/// Returns true if the entity exists.
	bool Exists(const std::string &key) const;

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an empty string if there's no attribute or entity with that name.
	std::string Get(const std::string &entity, const std::string &attrib) const;

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an false if there's no attribute or entity with that name.
	bool GetDouble(const std::string &entity, const std::string &attrib,  double *p) const;

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an false if there's no attribute or entity with that name.
	bool GetInt(const std::string &entity, const std::string &attrib,  int *p) const;

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an false if there's no attribute or entity with that name.
	bool GetUInt(const std::string &entity, const std::string &attrib, unsigned int *p) const;

	/// Returns the value of the attribute named 'attrib' from the entity 'entity'. Returns an false if there's no attribute or entity with that name.
	bool GetFloat(const std::string &entity, const std::string &attrib,  float *p) const;

	/// Fills the list passed as argument with all entity names.
	void GetEntityNameList(std::list<std::string> &entities) const;

	/// Returns a string containing all entity names in the file separated by commas.
	std::string GetEntityNames() const;

	/// Returns a string containing all attributes in the entity. Separated by commas.
	std::string GetAttributeNames(const std::string &key) const;

	/// Saves the object's content to a file.
	void WriteToFile(const std::string &fileName) const;

	/// Reads a file and parses it to the object.
	bool ParseFromFile(const std::string &fileName);

	/// Adds an attribute to an entity.
	void Add(const std::string &entity, const std::string &attrib, const std::string &value);

	unsigned int GetNumEntities() const;

	const std::map<std::string, Entity>& GetEntities() const;

private:

	/// Reads and returns an entity or attribute name
	std::string ReadName(std::string str, const char nextValidToken, const char commentChar, std::size_t *pCursor);

	/// Reads an attribute value and erases their syntactic backslashes.
	std::string ReadValue(std::string str, std::size_t *pCursor);

	std::map<std::string, Entity> m_entities;
	int m_ref; // reference counter
	ERROR_VALUE m_error; // last parsing error
};

} // namespace enml
} // namespace gs2d

#endif
