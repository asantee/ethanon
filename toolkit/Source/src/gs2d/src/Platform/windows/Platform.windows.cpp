#include "../../Application.h"

#include "../Platform.h"
#include "../FileLogger.h"

#include <assert.h>

#include <windows.h>
#include <direct.h>

gs2d::str_type::string gs2d::Application::GetPlatformName()
{
	return GS_L("windows");
}

namespace Platform {

#ifdef _DEBUG
gs2d::str_type::string GetModuleDirectory()
{
	gs2d::str_type::char_t currentDirectoryBuffer[65536];

	#ifdef GS2D_STR_TYPE_WCHAR
		GetCurrentDirectoryW(65535, currentDirectoryBuffer);
	#else
		GetCurrentDirectoryA(65535, currentDirectoryBuffer);
	#endif

	return AddLastSlash(currentDirectoryBuffer);
}
#else
gs2d::str_type::string GetModuleDirectory()
{
	gs2d::str_type::char_t moduleFileName[65536];

	#ifdef GS2D_STR_TYPE_WCHAR
		GetModuleFileNameW(NULL, moduleFileName, 65535);
	#else
		GetModuleFileNameA(NULL, moduleFileName, 65535);
	#endif

	return AddLastSlash(GetFileDirectory(moduleFileName));
}
#endif

const int kilobyteSize = 1024;
const int maximumAllowedStackAllocation = 4 * kilobyteSize;
// Usually stack has size limit is 1024 Kb
// To change stack size limit for your application/dll change these:
//	 Visual C++ Project -> Properties -> Linker -> System -> Stack Reserve Size; Stack Commit Size

// alloca - fast stack implementation for short strings - not longer then 4096 bytes of symbols
// new wchar_t - not so fast heap implementation for long strings

std::wstring ConvertUtf8ToUnicode(const char * utf8String)
{
	std::size_t utf8StringLength = strlen(utf8String);
	int resultLength = MultiByteToWideChar(
		CP_UTF8, 0, utf8String, utf8StringLength, 0, 0);

	int resultAllocationSize = sizeof(wchar_t) * (resultLength + 1);
	bool useHeap = resultAllocationSize >= maximumAllowedStackAllocation;

	wchar_t * unicodeBuffer = useHeap ?  
		new wchar_t[ resultLength + 1 ] : 
		(wchar_t *)alloca( resultAllocationSize );
	MultiByteToWideChar(
		CP_UTF8, 0, 
		utf8String, utf8StringLength, 
		unicodeBuffer, resultLength
	);
	unicodeBuffer[resultLength] = 0;

	if (useHeap)
	{
		// TODO: optimize - avoid extra temporary object creation.
		std::wstring result(unicodeBuffer);
		delete [] unicodeBuffer;
		return result;
	}
	return unicodeBuffer;
}

std::string ConvertUnicodeToUtf8(const wchar_t * unicodeString)
{
	size_t unicodeStringLength = wcslen(unicodeString);
	int resultLength = WideCharToMultiByte(
		CP_UTF8, 0, 
		unicodeString, unicodeStringLength, 
		0, 0, 0, 0
	);
	int resultAllocationSize = sizeof(char) * (resultLength + 1);
	bool useHeap = resultAllocationSize >= maximumAllowedStackAllocation;

	// fast stack implementation for short strings - not longer then 2048 symbols
	char * utf8Buffer = useHeap ? 
		new char[ resultLength + 1 ] :
		(char *)alloca( resultAllocationSize );
	WideCharToMultiByte(
		CP_UTF8, 0, 
		unicodeString, unicodeStringLength, 
		utf8Buffer, resultLength, 0, 0
	);
	utf8Buffer[resultLength] = 0;

	if (useHeap)
	{
		// TODO: optimize - avoid extra temporary object creation.
		std::string result(utf8Buffer);
		delete [] utf8Buffer;
		return result;
	}
	return utf8Buffer;
}

inline bool IsAsciiCharacter(char character)
{
	return ( (unsigned char)character <= (unsigned char)0x7f );
}

std::wstring ConvertAsciiToUnicode(const char* asciiString)
{
	if (asciiString == 0)
		return L"";
	#ifdef _DEBUG
	for (size_t i = 0; i < strlen(asciiString); ++i)
	{
	  assert( IsAsciiCharacter( asciiString[i] ) );
	}
	#endif
	return ConvertUtf8ToUnicode(asciiString);
}

std::string ConvertUnicodeToAscii(const wchar_t* unicodeString)
{
	if (unicodeString == 0)
		return "";
	std::string result = ConvertUnicodeToUtf8(unicodeString);
	#ifdef _DEBUG
	for (std::string::size_type i = 0; i < result.size(); ++i)
	{
	  assert( IsAsciiCharacter( result[i] ) );
	}
	#endif
	return result;
}

gs2d::str_type::string FileLogger::GetLogDirectory()
{
	return GetModuleDirectory();
}

char GetDirectorySlashA()
{
	return '\\';
}

wchar_t GetDirectorySlashW()
{
	return L'\\';
}

// undefines window's Create directory original
#undef CreateDirectory

bool CreateDirectory(const std::string& path)
{
	return (_mkdir(path.c_str()) == 0);
}

} // namespace Platform
