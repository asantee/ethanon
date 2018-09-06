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

#include "JSONObject.h"

JSONObject::JSONObject(cJSON* cjson) :
	m_cjson(cjson),
	m_isParent(false)
{
}

JSONObject::JSONObject(const JSONObject& cjson) :
	m_cjson(cjson.m_cjson),
	m_isParent(false)
{
}

JSONObject::~JSONObject()
{
	if (m_cjson != NULL)
	{
		if (m_isParent)
		{
			cJSON_Delete(m_cjson);
		}
		m_cjson = NULL;
	}
}

JSONObject JSONObject::GetNext()
{
	if (m_cjson != NULL)
	{
		return JSONObject(m_cjson->next);
	}
	return JSONObject(NULL);
}

JSONObject JSONObject::GetPrev()
{
	if (m_cjson != NULL)
	{
		return JSONObject(m_cjson->prev);
	}
	return JSONObject(NULL);
}

JSONObject JSONObject::GetChild()
{
	if (m_cjson != NULL)
	{
		return JSONObject(m_cjson->child);
	}
	return JSONObject(NULL);
}

bool JSONObject::Parse(const gs2d::str_type::string& value)
{
	m_cjson = cJSON_Parse(value.c_str());
	m_isParent = true;
	return (m_cjson != NULL);
}

gs2d::str_type::string JSONObject::GetError() const
{
	return cJSON_GetErrorPtr();
}

JSONObject JSONObject::GetObjectItem(const gs2d::str_type::string& name) const
{
	return JSONObject(cJSON_GetObjectItem(m_cjson, name.c_str()));
}

gs2d::str_type::string JSONObject::GetStringValue() const
{
	if (m_cjson != NULL)
	{
		if ((cJSON_IsRaw(m_cjson) || cJSON_IsString(m_cjson)) && (m_cjson->valuestring != NULL))
		{
			return gs2d::str_type::string(m_cjson->valuestring);
		}
	}
	return gs2d::str_type::string("");
}

double JSONObject::GetDoubleValue() const
{
	if (m_cjson != NULL)
	{
		if (cJSON_IsNumber(m_cjson))
		{
			return m_cjson->valuedouble;
		}
	}
	return 0.0;
}

bool JSONObject::IsInvalid() const
{
	return cJSON_IsInvalid(m_cjson);
}

bool JSONObject::IsFalse() const
{
	return cJSON_IsFalse(m_cjson);
}

bool JSONObject::IsTrue() const
{
	return cJSON_IsTrue(m_cjson);
}

bool JSONObject::IsBool() const
{
	return cJSON_IsBool(m_cjson);
}

bool JSONObject::IsNull() const
{
	if (m_cjson == NULL)
		return true;
	else
		return cJSON_IsNull(m_cjson);
}

bool JSONObject::IsNumber() const
{
	return cJSON_IsNumber(m_cjson);
}

bool JSONObject::IsString() const
{
	return cJSON_IsString(m_cjson);
}

bool JSONObject::IsArray() const
{
	return cJSON_IsArray(m_cjson);
}

bool JSONObject::IsObject() const
{
	return cJSON_IsObject(m_cjson);
}

bool JSONObject::IsRaw() const
{
	return cJSON_IsRaw(m_cjson);
}
