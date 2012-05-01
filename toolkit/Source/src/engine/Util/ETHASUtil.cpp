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

#include "../ETHResourceProvider.h"

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
			#ifdef _DEBUG
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

} // namespace ETHGlobal