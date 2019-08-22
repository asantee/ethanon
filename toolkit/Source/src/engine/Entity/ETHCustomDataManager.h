#ifndef ETH_DATA_MANAGER_H_
#define ETH_DATA_MANAGER_H_

#include <Math/Vector2.h>
#include <Math/Vector3.h>

#include "../ETHTypes.h"

#include "../../tsl/hopscotch_map.h"

#include <string>

#define UNUSED_ARGUMENT(argument) ((void)(argument))

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
	inline virtual std::string GetString() const { return (""); }
	inline virtual Vector2 GetVector2() const { return Vector2(0,0); }
	inline virtual Vector3 GetVector3() const { return Vector3(0,0,0); }

	inline virtual bool Set(const float value) { UNUSED_ARGUMENT(value); return false; }
	inline virtual bool Set(const int value) { UNUSED_ARGUMENT(value); return false; }
	inline virtual bool Set(const unsigned int value) { UNUSED_ARGUMENT(value); return false; }
	inline virtual bool Set(const std::string &sValue) { UNUSED_ARGUMENT(sValue); return false; }
	inline virtual bool Set(const Vector2 &v) { UNUSED_ARGUMENT(v); return false; }
	inline virtual bool Set(const Vector3 &v) { UNUSED_ARGUMENT(v); return false; }

	virtual DATA_TYPE GetType() const = 0;
	virtual std::string GetValueAsString() const = 0;
};

typedef boost::shared_ptr<ETHCustomData> ETHCustomDataPtr;
typedef boost::shared_ptr<const ETHCustomData> ETHCustomDataConstPtr;

class ETHIntData : public ETHCustomData {
	int v;
public: inline ETHIntData(const int v) { this->v = v; }
		inline bool Set(const int value) { v = value; return true; }
		inline int GetInt() const { return v; }
		inline DATA_TYPE GetType() const { return DT_INT; }
		inline std::string GetValueAsString() const { std::stringstream ss; ss << v; return ss.str(); }
};

class ETHUIntData : public ETHCustomData {
	unsigned int v;
public: inline ETHUIntData(const unsigned int v) { this->v = v; }
		inline bool Set(const unsigned int value) { v = value; return true; }
		inline unsigned int GetUInt() const { return v; }
		inline DATA_TYPE GetType() const { return DT_UINT; }
		inline std::string GetValueAsString() const { std::stringstream ss; ss << v; return ss.str(); }
};

class ETHFloatData : public ETHCustomData {
	float v;
public: inline ETHFloatData(const float v) { this->v = v; }
		inline bool Set(const float value) { v = value; return true; }
		inline float GetFloat() const { return v; }
		inline DATA_TYPE GetType() const { return DT_FLOAT; }
		inline std::string GetValueAsString() const { std::stringstream ss; ss << v; return ss.str(); }
};

class ETHStringData : public ETHCustomData {
	std::string v;
public: inline ETHStringData(const std::string &v) { this->v = v; }
		inline bool Set(const std::string &value) { v = value; return true; }
		inline std::string GetString() const { return v; }
		inline DATA_TYPE GetType() const { return DT_STRING; }
		inline std::string GetValueAsString() const { return v; }
};

class ETHVector2Data : public ETHCustomData {
	Vector2 v;
public: inline ETHVector2Data(const Vector2 &v) { this->v = v; }
		inline bool Set(const Vector2 &value) { v = value; return true; }
		inline Vector2 GetVector2() const { return v; }
		inline DATA_TYPE GetType() const { return DT_VECTOR2; }
		inline std::string GetValueAsString() const { std::stringstream ss; ss << "(" << v.x << ", " << v.y << ")"; return ss.str(); }
};

class ETHVector3Data : public ETHCustomData {
	Vector3 v;
public: inline ETHVector3Data(const Vector3 &v) { this->v = v; }
		inline bool Set(const Vector3 &value) { v = value; return true; }
		inline Vector3 GetVector3() const { return v; }
		inline DATA_TYPE GetType() const { return DT_VECTOR3; }
		inline std::string GetValueAsString() const { std::stringstream ss; ss << "(" << v.x << ", " << v.y << ", " << v.z << ")"; return ss.str(); }
};

class ETHCustomDataManager
{
	static const std::string DATA_NAME[ETHCustomData::CUSTOM_DATA_TYPE_COUNT];

public:
	std::string GetDebugStringData() const;
	static const std::string &GetDataName(const unsigned int n);

	const ETHCustomDataManager& operator=(const ETHCustomDataManager& a);

	void AddData(const std::string &name, const ETHCustomDataConstPtr &dataIn);

	void SetFloat(const std::string &name, const float &value);
	void SetInt(const std::string &name, const int &value);
	void SetUInt(const std::string &name, const unsigned int &value);
	void SetString(const std::string &name, const std::string &sValue);
	void SetVector2(const std::string &name, const Vector2 &v);
	void SetVector3(const std::string &name, const Vector3 &v);

	bool GetFloat(const std::string &name, float &outValue) const;
	bool GetInt(const std::string &name, int &outValue) const;
	bool GetUInt(const std::string &name, unsigned int &outValue) const;
	bool GetString(const std::string &name, std::string &outValue) const;
	bool GetVector2(const std::string &name, Vector2 &outValue) const;
	bool GetVector3(const std::string &name, Vector3 &outValue) const;

	float AddToFloat(const std::string &name, const float &value);
	int AddToInt(const std::string &name, const int &value);
	unsigned int AddToUInt(const std::string &name, const unsigned int &value);
	Vector2 AddToVector2(const std::string &name, const Vector2 &v);
	Vector3 AddToVector3(const std::string &name, const Vector3 &v);

	float MultiplyFloat(const std::string &name, const float &value);
	int MultiplyInt(const std::string &name, const int &value);
	unsigned int MultiplyUInt(const std::string &name, const unsigned int &value);
	Vector2 MultiplyVector2(const std::string &name, const float &value);
	Vector3 MultiplyVector3(const std::string &name, const float &value);

	ETHCustomData::DATA_TYPE Check(const std::string &name) const;
	bool HasData() const;
	bool EraseData(const std::string &name);
	unsigned int GetNumVariables() const;
	void Clear();

	void InsertData(const ETHCustomDataManager &dataIn);
	void MoveData(ETHCustomDataManager &dataOut) const;
	void CopyMap(tsl::hopscotch_map<std::string, ETHCustomDataPtr> &inMap) const;
	std::string GetValueAsString(const std::string &name) const;

	bool ReadDataFromXMLFile(TiXmlElement *pRoot);
	bool WriteDataToFile(TiXmlElement *pHeadRoot) const;

private:
	tsl::hopscotch_map<std::string, ETHCustomDataPtr> m_data;
};

#endif
