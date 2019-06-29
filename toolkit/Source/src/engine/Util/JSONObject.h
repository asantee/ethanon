#ifndef JSON_OBJECT_H_
#define JSON_OBJECT_H_

#include "../../cjson/cJSON.h"

#include "../ETHTypes.h"

class JSONObject
{
	cJSON* m_cjson;
	bool m_isParent;

public:
	JSONObject(cJSON* cjson);
	JSONObject(const JSONObject& cjson);
	~JSONObject();

	bool Parse(const gs2d::str_type::string& value);
	gs2d::str_type::string GetError() const;

	JSONObject GetNext();
	JSONObject GetPrev();
	JSONObject GetChild();

	JSONObject GetObjectItem(const gs2d::str_type::string& name) const;

	gs2d::str_type::string GetStringValue() const;
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
