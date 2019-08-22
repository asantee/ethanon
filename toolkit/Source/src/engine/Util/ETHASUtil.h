#ifndef ETH_AS_UTIL_H_
#define ETH_AS_UTIL_H_

#include "../../angelscript/include/angelscript.h"
#include "../ETHTypes.h"
#include "../Entity/ETHScriptEntity.h"

namespace ETHGlobal {
	void CheckFunctionSeekError(const int id, const std::string& function);
	void PrintException(asIScriptContext *pContext);
	void ExecuteContext(asIScriptContext *pContext, asIScriptFunction* func, const bool prepare = true);
	asIScriptFunction* FindCallbackFunction(asIScriptModule* pModule, const ETHScriptEntity* entity, const std::string& prefix, const Platform::Logger& logger);
	bool RunEntityCallback(asIScriptContext* pContext, ETHScriptEntity* entity, asIScriptFunction* func);
	bool IsTrue(const std::string& source);
	bool FileExists(const std::string& file, const Platform::FileManagerPtr& fileManager);
	std::string AppendExtensionIfNeeded(std::string source, const std::string& ext);
	std::string GetDataResourceFullPath(const std::string& path, const std::string& file);
	Vector2 ToScreenPos(const Vector3 &v3Pos, const Vector2 &zAxisDirection);
	Vector2 ToVector2(const Vector3 &v3);
	Vector2 V2iToV2F(const Vector2i &v2);
	bool ToBool(const ETH_BOOL b);
	bool IsSphereInScreen(const Vector3& v3Pos, const float radius, const Vector2& zAxisDir, const VideoPtr& video);

	bool IsValidUTF8(const std::string& str);
	unsigned int DistanceUTF8(const std::string& str);

	float ParseFloat(const char* str);
	double ParseDouble(const char* str);
	int ParseInt(const char* str);
	unsigned int ParseUInt(const char* str);
	long long ParseInt64(const char* str);
	unsigned long long ParseUInt64(const char* str);

	float ParseFloatStd(const std::string &str);
	double ParseDoubleStd(const std::string &str);
	int ParseIntStd(const std::string &str);
	unsigned int ParseUIntStd(const std::string &str);
	long long ParseInt64Std(const std::string &str);
	unsigned long long ParseUInt64Std(const std::string &str);
} // namespace ETHGlobal

#endif
