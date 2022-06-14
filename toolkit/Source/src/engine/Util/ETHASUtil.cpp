#include "ETHASUtil.h"

#include <iostream>

#include "../Resource/ETHResourceProvider.h"

#include "../addons/scriptbuilder.h"

#include "../../gs2d/src/Unicode/utf8/utf8.h"

#include <inttypes.h>

#ifdef _MSC_VER
  #define GS2D_SSCANF sscanf_s
#else
  #define GS2D_SSCANF sscanf
#endif

#define UNUSED_ARGUMENT(argument) ((void)(argument))

namespace ETHGlobal {

void ExecuteContext(asIScriptContext *pContext, asIScriptFunction* func, const bool prepare)
{
	if (prepare)
	{
		if (pContext->Prepare(func) < 0)
		{
			ETH_STREAM_DECL(ss) << ("(ExecuteContext) Couldn't prepare context for function  ID ") << func->GetId();
			ETHResourceProvider::Log(ss.str(), Platform::Logger::LT_ERROR);
			return;
		}
	}

	pContext->Execute();
}

void CheckFunctionSeekError(const int id, const std::string& function)
{
	std::stringstream ss;
	switch (id)
	{
	case asERROR:
		ss << ("Callback function seeking error - Invalid module (") << function << (").");
		ETHResourceProvider::Log(ss.str(), Platform::Logger::LT_ERROR);
		break;
	case asMULTIPLE_FUNCTIONS:
		ss << ("\n*Script error:\nCallback function seeking error - there are multiple functions with this name (") << function << (").");
		ETHResourceProvider::Log(ss.str(), Platform::Logger::LT_ERROR);
		break;
	};
}

asIScriptFunction* FindCallbackFunction(asIScriptModule* pModule, const ETHScriptEntity* entity, const std::string& prefix, const Platform::Logger& logger)
{
	const std::string entityName = Platform::RemoveExtension(entity->GetEntityName().c_str());
	std::stringstream funcName;
	funcName << prefix << entityName;
	asIScriptFunction* func = pModule->GetFunctionByName(funcName.str().c_str());

	// TODO/TO-DO: handle function overload ambiguity
	UNUSED_ARGUMENT(logger);
	/*if (id == asMULTIPLE_FUNCTIONS)
	{
		std::stringstream ss;
		ss << ("ETHScene::FindCallbackFunction: found multiple functions named (") << funcName.str() << (").");
		logger.Log(ss.str(), Platform::FileLogger::LT_ERROR);
	}*/
	return func;
}

void PrintException(asIScriptContext* pContext) {
	std::cout  << "Function " << pContext->GetExceptionFunction() << "at line " << pContext->GetLineNumber() << ": " << pContext->GetExceptionString() << std::endl;
	return;
}

bool RunEntityCallback(asIScriptContext* pContext, ETHScriptEntity* entity, asIScriptFunction* func)
{
	if (!func)
		return false;

	if (pContext->Prepare(func) < 0)
		return false;

	if (pContext->SetArgObject(0, entity) >= 0)
		ETHGlobal::ExecuteContext(pContext, func, false);
	else
		return false;
	return true;
}

bool IsTrue(const std::string& source)
{
	if (source == "true" || source == "TRUE" || source == "yes" || source == "YES" || source == "1")
		return true;
	else
		return false;
}

bool FileExists(const std::string& file, const Platform::FileManagerPtr& fileManager)
{
	return fileManager->FileExists(file);
}

std::string AppendExtensionIfNeeded(std::string source, const std::string& ext)
{
	if (source.rfind(ext) != (source.size() - ext.size()))
	{
		source.append(ext);
	}
	return source;
}

std::string GetDataResourceFullPath(const std::string& path, const std::string& file)
{
	return path + file;
}

Vector2 ToScreenPos(const Vector3 &v3Pos, const Vector2 &zAxisDirection)
{
	return Vector2(v3Pos.x, v3Pos.y)+(zAxisDirection*v3Pos.z);
}

Vector2 ToVector2(const Vector3 &v3)
{
	return Vector2(v3.x, v3.y);
}

Vector2 V2iToV2F(const Vector2i &v2)
{
	return Vector2(static_cast<float>(v2.x), static_cast<float>(v2.y));
}

bool ToBool(const ETH_BOOL b)
{
	return (b == ETH_TRUE);
}

float ParseFloat(const char* str)
{
	float f = 0.0f;
	GS2D_SSCANF(str, ("%f"), &f);
	return f;
}

double ParseDouble(const char* str)
{
	double d = 0.0;
	GS2D_SSCANF(str, ("%lf"), &d);
	return d;
}

int32_t ParseInt(const char* str)
{
	int32_t n = 0;
	GS2D_SSCANF(str, "%" SCNd32, &n);
	return n;
}

uint32_t ParseUInt(const char* str)
{
	uint32_t n = 0;
	GS2D_SSCANF(str, "%" SCNu32, &n);
	return n;
}

int64_t ParseInt64(const char* str)
{
	int64_t n = 0; SCNd64;
	GS2D_SSCANF(str, "%" SCNd64, &n);
	return n;
}

uint64_t ParseUInt64(const char* str)
{
	uint64_t n = 0;
	GS2D_SSCANF(str, "%" SCNu64, &n);
	return n;
}
	
float ParseFloatStd(const std::string& str)
{
	return ParseFloat(str.c_str());
}

double ParseDoubleStd(const std::string& str)
{
	return ParseDouble(str.c_str());
}

int32_t ParseIntStd(const std::string& str)
{
	return ParseInt(str.c_str());
}

uint32_t ParseUIntStd(const std::string& str)
{
	return ParseUInt(str.c_str());
}

int64_t ParseInt64Std(const std::string& str)
{
	return ParseInt64(str.c_str());
}

uint64_t ParseUInt64Std(const std::string& str)
{
	return ParseUInt64(str.c_str());
}
	
bool IsValidUTF8(const std::string& str)
{
	return utf8::is_valid(&str[0], &str[0] + str.length());
}

uint32_t DistanceUTF8(const std::string& str)
{
	return static_cast<uint32_t>(utf8::distance(&str[0], &str[0] + str.length()));
}

bool IsSphereInScreen(const Vector3& pos, const float radius, const Vector2& zAxisDir, const VideoPtr& video)
{
	const Vector2 v2Pos(ToScreenPos(pos, zAxisDir));
	const Vector2& v2Cam = video->GetCameraPos();
	const Vector2& v2Screen = video->GetScreenSizeF();
	const float minX = v2Cam.x - radius;
	const float maxX = v2Cam.x + v2Screen.x + radius;
	const float minY = v2Cam.y - radius;
	const float maxY = v2Cam.y + v2Screen.y + radius;
	if (v2Pos.x < minX || v2Pos.x > maxX || v2Pos.y < minY || v2Pos.y > maxY)
	{
		return false;
	}
	else
	{
		return true;
	}
}

} // namespace ETHGlobal
