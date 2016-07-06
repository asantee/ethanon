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

#ifndef ETH_DATA_MANAGER_H_
#define ETH_DATA_MANAGER_H_

#include "../ETHTypes.h"
#include <map>
#include <string>


class ETHCustomData
{
public:
	enum DATA_TYPE
	{
		DT_NODATA = 0,
		DT_FLOAT = 1,
		DT_INT = 2,
		DT_UINT = 3,
		DT_STRING = 4,
		DT_VECTOR2 = 5,
		DT_VECTOR3 = 6,
		CUSTOM_DATA_TYPE_COUNT = 7
	};

	inline virtual float GetFloat() const { return 0.0f; }
	inline virtual int GetInt() const { return 0; }
	inline virtual unsigned int GetUInt() const { return 0; }
	inline virtual str_type::string GetString() const { return GS_L(""); }
	inline virtual Vector2 GetVector2() const { return Vector2(0,0); }
	inline virtual Vector3 GetVector3() const { return Vector3(0,0,0); }

	inline virtual bool Set(const float value) { GS2D_UNUSED_ARGUMENT(value); return false; }
	inline virtual bool Set(const int value) { GS2D_UNUSED_ARGUMENT(value); return false; }
	inline virtual bool Set(const unsigned int value) { GS2D_UNUSED_ARGUMENT(value); return false; }
	inline virtual bool Set(const str_type::string &sValue) { GS2D_UNUSED_ARGUMENT(sValue); return false; }
	inline virtual bool Set(const Vector2 &v) { GS2D_UNUSED_ARGUMENT(v); return false; }
	inline virtual bool Set(const Vector3 &v) { GS2D_UNUSED_ARGUMENT(v); return false; }

	virtual DATA_TYPE GetType() const = 0;
	virtual str_type::string GetValueAsString() const = 0;
};

typedef boost::shared_ptr<ETHCustomData> ETHCustomDataPtr;
typedef boost::shared_ptr<const ETHCustomData> ETHCustomDataConstPtr;

class ETHIntData : public ETHCustomData {
	int v;
public: inline ETHIntData(const int v) { this->v = v; }
		inline bool Set(const int value) { v = value; return true; }
		inline int GetInt() const { return v; }
		inline DATA_TYPE GetType() const { return DT_INT; }
		inline str_type::string GetValueAsString() const { str_type::stringstream ss; ss << v; return ss.str(); }
};

class ETHUIntData : public ETHCustomData {
	unsigned int v;
public: inline ETHUIntData(const unsigned int v) { this->v = v; }
		inline bool Set(const unsigned int value) { v = value; return true; }
		inline unsigned int GetUInt() const { return v; }
		inline DATA_TYPE GetType() const { return DT_UINT; }
		inline str_type::string GetValueAsString() const { str_type::stringstream ss; ss << v; return ss.str(); }
};

class ETHFloatData : public ETHCustomData {
	float v;
public: inline ETHFloatData(const float v) { this->v = v; }
		inline bool Set(const float value) { v = value; return true; }
		inline float GetFloat() const { return v; }
		inline DATA_TYPE GetType() const { return DT_FLOAT; }
		inline str_type::string GetValueAsString() const { str_type::stringstream ss; ss << v; return ss.str(); }
};

class ETHStringData : public ETHCustomData {
	str_type::string v;
public: inline ETHStringData(const str_type::string &v) { this->v = v; }
		inline bool Set(const str_type::string &value) { v = value; return true; }
		inline str_type::string GetString() const { return v; }
		inline DATA_TYPE GetType() const { return DT_STRING; }
		inline str_type::string GetValueAsString() const { return v; }
};

class ETHVector2Data : public ETHCustomData {
	Vector2 v;
public: inline ETHVector2Data(const Vector2 &v) { this->v = v; }
		inline bool Set(const Vector2 &value) { v = value; return true; }
		inline Vector2 GetVector2() const { return v; }
		inline DATA_TYPE GetType() const { return DT_VECTOR2; }
		inline str_type::string GetValueAsString() const { str_type::stringstream ss; ss << "(" << v.x << ", " << v.y << ")"; return ss.str(); }
};

class ETHVector3Data : public ETHCustomData {
	Vector3 v;
public: inline ETHVector3Data(const Vector3 &v) { this->v = v; }
		inline bool Set(const Vector3 &value) { v = value; return true; }
		inline Vector3 GetVector3() const { return v; }
		inline DATA_TYPE GetType() const { return DT_VECTOR3; }
		inline str_type::string GetValueAsString() const { str_type::stringstream ss; ss << "(" << v.x << ", " << v.y << ", " << v.z << ")"; return ss.str(); }
};

class ETHCustomDataManager
{
	static const str_type::string DATA_NAME[ETHCustomData::CUSTOM_DATA_TYPE_COUNT];

public:
	str_type::string GetDebugStringData() const;
	static const str_type::string &GetDataName(const unsigned int n);

	const ETHCustomDataManager& operator=(const ETHCustomDataManager& a);

	void AddData(const str_type::string &name, const ETHCustomDataConstPtr &dataIn);

	void SetFloat(const str_type::string &name, const float &value);
	void SetInt(const str_type::string &name, const int &value);
	void SetUInt(const str_type::string &name, const unsigned int &value);
	void SetString(const str_type::string &name, const str_type::string &sValue);
	void SetVector2(const str_type::string &name, const Vector2 &v);
	void SetVector3(const str_type::string &name, const Vector3 &v);

	bool GetFloat(const str_type::string &name, float &outValue) const;
	bool GetInt(const str_type::string &name, int &outValue) const;
	bool GetUInt(const str_type::string &name, unsigned int &outValue) const;
	bool GetString(const str_type::string &name, str_type::string &outValue) const;
	bool GetVector2(const str_type::string &name, Vector2 &outValue) const;
	bool GetVector3(const str_type::string &name, Vector3 &outValue) const;

	float AddToFloat(const str_type::string &name, const float &value);
	int AddToInt(const str_type::string &name, const int &value);
	unsigned int AddToUInt(const str_type::string &name, const unsigned int &value);
	Vector2 AddToVector2(const str_type::string &name, const Vector2 &v);
	Vector3 AddToVector3(const str_type::string &name, const Vector3 &v);

	float MultiplyFloat(const str_type::string &name, const float &value);
	int MultiplyInt(const str_type::string &name, const int &value);
	unsigned int MultiplyUInt(const str_type::string &name, const unsigned int &value);
	Vector2 MultiplyVector2(const str_type::string &name, const float &value);
	Vector3 MultiplyVector3(const str_type::string &name, const float &value);

	ETHCustomData::DATA_TYPE Check(const str_type::string &name) const;
	bool HasData() const;
	bool EraseData(const str_type::string &name);
	unsigned int GetNumVariables() const;
	void Clear();

	void InsertData(const ETHCustomDataManager &dataIn);
	void MoveData(ETHCustomDataManager &dataOut) const;
	void CopyMap(std::map<str_type::string, ETHCustomDataPtr> &inMap) const;
	str_type::string GetValueAsString(const str_type::string &name) const;

	bool ReadDataFromXMLFile(TiXmlElement *pRoot);
	bool WriteDataToFile(TiXmlElement *pHeadRoot) const;

private:
	std::map<str_type::string, ETHCustomDataPtr> m_data;
};

#endif