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

#include "ETHASUtil.h"
#include <iostream>

#include "../Resource/ETHResourceProvider.h"

#ifdef GS2D_STR_TYPE_WCHAR
#	include "../addons/utf16/scriptbuilder.h"
#else
#	include "../addons/ansi/scriptbuilder.h"
#endif

namespace ETHGlobal {
void ExecuteContext(asIScriptContext *pContext, const int id, const bool prepare)
{
	if (prepare)
	{
		if (pContext->Prepare(id) < 0)
		{
			ETH_STREAM_DECL(ss) << GS_L("(ExecuteContext) Couldn't prepare context for function  ID ") << id;
			ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
			return;
		}
	}

	const int r = pContext->Execute();
	if (r != asEXECUTION_FINISHED)
	{
		if (r == asEXECUTION_EXCEPTION)
		{
			PrintException(pContext);
			ETH_STREAM_DECL(ss) << GS_L("Exception: ") << pContext->GetExceptionString();
			#if defined(_DEBUG) || defined(DEBUG)
			ss << static_cast<str_type::char_t>(0x07);
			#endif
			ShowMessage(ss.str(), ETH_ERROR);
		}
	}
}

void CheckFunctionSeekError(const int id, const str_type::string& function)
{
	str_type::stringstream ss;
	switch (id)
	{
	case asERROR:
		ss << GS_L("Callback function seeking error - Invalid module (") << function << GS_L(").");
		ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		break;
	case asMULTIPLE_FUNCTIONS:
		ss << GS_L("\n*Script error:\nCallback function seeking error - there are multiple functions with this name (") << function << GS_L(").");
		ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		break;
	};
}

void ShowMessage(str_type::string message, const ETH_MESSAGE type)
{
	str_type::stringstream ss;
	switch (type)
	{
	case ETH_ERROR:
		ss << GS_L("ERROR - ") << message;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		break;
	case ETH_WARNING:
		ss << GS_L("Warning - ") << message;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		break;
	case ETH_INFO:
		ss << message;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::INFO);
		break;
	};
	#ifndef ANDROID
	std::wcout << std::endl;
	#endif
}

void PrintException(asIScriptContext *pContext)
{
	asIScriptEngine *m_pASEngine = pContext->GetEngine();
	const int funcId = pContext->GetExceptionFunction();
	const asIScriptFunction *pFunction = m_pASEngine->GetFunctionDescriptorById(funcId);
	ETH_STREAM_DECL(ss) << GS_L("Function: ") << pFunction->GetDeclaration() << std::endl
						<< GS_L("Section: ") << pFunction->GetScriptSectionName() << std::endl
						<< GS_L("Line: ") << pContext->GetExceptionLineNumber() << std::endl
						<< GS_L("Description: ") << pContext->GetExceptionString() << std::endl;
	ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
}

int FindCallbackFunction(asIScriptModule* pModule, const ETHScriptEntity* entity, const str_type::string& prefix, const Platform::Logger& logger)
{
	const str_type::string entityName = Platform::RemoveExtension(entity->GetEntityName().c_str());
	str_type::stringstream funcName;
	funcName << prefix << entityName;
	const int id = CScriptBuilder::GetFunctionIdByName(pModule, funcName.str());

	if (id == asMULTIPLE_FUNCTIONS)
	{
		str_type::stringstream ss;
		ss << GS_L("ETHScene::FindCallbackFunction: found multiple functions named (") << funcName.str() << GS_L(").");
		logger.Log(ss.str(), Platform::FileLogger::ERROR);
	}
	return id;
}

bool RunEntityCallback(asIScriptContext* pContext, ETHScriptEntity* entity, const int id)
{
	if (id < 0)
		return false;

	if (pContext->Prepare(id) < 0)
		return false;

	if (pContext->SetArgObject(0, entity) >= 0)
		ETHGlobal::ExecuteContext(pContext, id, false);
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

bool FileExists(const str_type::string& file, const Platform::FileManagerPtr& fileManager)
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

std::vector<str_type::string> SplitString(str_type::string str, const str_type::string& c)
{
	std::vector<str_type::string> v; // I know...
	std::size_t pos;
	while ((pos = str.find(c)) != str_type::string::npos)
	{
		v.push_back(str.substr(0, pos));
		str = str.substr(pos + c.length(), str_type::string::npos);
	}
	v.push_back(str);
	return v;
}

str_type::string GetDataResourceFullPath(const str_type::string& path, const str_type::string& file)
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

float ParseFloat(const str_type::char_t* str)
{
	float f = 0.0f;
	GS2D_SSCANF(str, GS_L("%f"), &f);
	return f;
}

int ParseInt(const str_type::char_t* str)
{
	int n = 0;
	GS2D_SSCANF(str, GS_L("%d"), &n);
	return n;
}

unsigned int ParseUInt(const str_type::char_t* str)
{
	unsigned int n = 0;
	GS2D_SSCANF(str, GS_L("%u"), &n);
	return n;
}

float ParseFloatStd(const str_type::string& str)
{
	return ParseFloat(str.c_str());
}

int ParseIntStd(const str_type::string& str)
{
	return ParseInt(str.c_str());
}

unsigned int ParseUIntStd(const str_type::string& str)
{
	return ParseUInt(str.c_str());
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

#ifdef GS2D_STR_TYPE_WCHAR
std::wstring AppendExtensionIfNeeded(std::wstring source, const std::wstring& ext)
{
	if (source.rfind(ext) != (source.size() - ext.size()))
	{
		source.append(ext);
	}
	return source;
}

bool IsTrue(const std::wstring& source)
{
	if (source == L"true" || source == L"TRUE" || source == L"yes" || source == L"YES" || source == L"1")
		return true;
	else
		return false;
}
#endif

} // namespace ETHGlobal