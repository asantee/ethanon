#ifdef ABSTRACT_PLATFORM

#include "../Platform.h"
#include "../../../angelscript/include/angelscript.h"

namespace ETHPlatform 
{
	bool LoadDynamicLibrary(const str_type::char_t *filePath, asIScriptEngine *pASEngine)
	{
		// TODO: implement other platform versions
		return false;
	}
} 
// namespace ETHPlatform

#endif // ABSTRACT_PLATFORM
