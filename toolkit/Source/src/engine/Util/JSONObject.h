#ifndef JSON_OBJECT_H_
#define JSON_OBJECT_H_

#include "../../cjson/cJSON.h"

#include <string>

class JSONObject
{
	cJSON* m_cjson;
	bool m_isParent;

	void Destroy();
	
public:
	JSONObject(cJSON* cjson);
	JSONObject(const JSONObject& cjson);
	~JSONObject();

	bool Parse(const std::string& value);
	std::string GetError() const;

	JSONObject GetNext();
	JSONObject GetPrev();
	JSONObject GetChild();

	JSONObject GetObjectItem(const std::string& name) const;

	std::string GetStringValue() const;
	double GetDoubleValue() const;

	bool IsInvalid() const;
	bool IsFalse() const;
	bool IsTrue() const;
	bool IsBool() const;
	bool IsNull() const;
	bool IsNumber() const;
	bool IsString() const;
	bool IsArray() const;
	bool IsObject() const;
	bool IsRaw() const;
};

#endif
