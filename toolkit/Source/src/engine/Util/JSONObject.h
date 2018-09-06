/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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
