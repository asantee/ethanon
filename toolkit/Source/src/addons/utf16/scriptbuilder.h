#ifndef SCRIPTBUILDER_H
#define SCRIPTBUILDER_H

//---------------------------
// Compilation settings
//

// Set this flag to turn on/off metadata processing
//  0 = off
//  1 = on
#ifndef AS_PROCESS_METADATA
#define AS_PROCESS_METADATA 1
#endif

// TODO: Implement flags for turning on/off include directives and conditional programming



//---------------------------
// Declaration
//

#include "../../angelscript/include/angelscript.h"
#include "../../engine/Resource/ETHResourceProvider.h"

#if defined(_MSC_VER) && _MSC_VER <= 1200 
// disable the annoying warnings on MSVC 6
#pragma warning (disable:4786)
#endif

#include <string>
#include <map>
#include <set>
#include <vector>

BEGIN_AS_NAMESPACE

// TODO: Need a callback routine for resolving include directives
//       When the builder encounters an include directive, it should call the callback with the current section name and the include directive.
//       The application should respond by calling AddScriptFromFile or AddScriptFromMemory (or give an error if the include is invalid).
//       The AddScriptFromFile/Memory should put the scripts on the queue to be built

// TODO: Should process metadata for class/interface members as well

class CScriptBuilder;

// This callback will be called for each #include directive encountered by the
// builder. The callback should call the AddSectionFromFile or AddSectionFromMemory
// to add the included section to the script. If the include cannot be resolved
// then the function should return a negative value to abort the compilation.
typedef int (*INCLUDECALLBACK_t)(const char *include, const char *from, CScriptBuilder *builder, void *userParam);

// Helper class for loading and pre-processing script files to 
// support include directives and metadata declarations
class CScriptBuilder
{
public:
	CScriptBuilder(const ETHResourceProviderPtr& provider);

	// Start a new module
	int StartNewModule(asIScriptEngine *engine, const wchar_t *moduleName);

	// Load a script section from a file on disk
	int AddSectionFromFile(const wchar_t *filename);

	// Load a script section from memory
	int AddSectionFromMemory(const char *scriptCode, 
							 const wchar_t *sectionName = L"");

	// Build the added script sections
	int BuildModule();

	// Register the callback for resolving include directive
	void SetIncludeCallback(INCLUDECALLBACK_t callback, void *userParam);

	// Add a pre-processor define for conditional compilation
	void DefineWord(const char *word);
	void DefineWord(const wchar_t *word);

#if AS_PROCESS_METADATA == 1
	// Get metadata declared for class types and interfaces
	const char *GetMetadataStringForType(int typeId);

	// Get metadata declared for functions
	const char *GetMetadataStringForFunc(int funcId);

	// Get metadata declared for global variables
	const char *GetMetadataStringForVar(int varIdx);
#endif

	static asIScriptModule * GetModule(asIScriptEngine * targetEngine, const std::wstring& module, asEGMFlags flag = asGM_ONLY_IF_EXISTS);

protected:
	void ClearAll();
	int  Build();
	int  ProcessScriptSection(const char *script, const wchar_t *sectionname);
	int  LoadScriptSection(const wchar_t *filename);
	bool IncludeIfNotAlreadyIncluded(const wchar_t *filename);

	int  SkipStatement(int pos);

	int  ExcludeCode(int start);
	void OverwriteCode(int start, int len);

	asIScriptEngine           *engine;
	asIScriptModule           *module;
	std::string                modifiedScript;

	ETHResourceProviderPtr m_provider;

	INCLUDECALLBACK_t  includeCallback;
	void              *callbackParam;

#if AS_PROCESS_METADATA == 1
	int  ExtractMetadataString(int pos, std::string &outMetadata);
	int  ExtractDeclaration(int pos, std::string &outDeclaration, int &outType);

	// Temporary structure for storing metadata and declaration
	struct SMetadataDecl
	{
		SMetadataDecl(std::string m, std::string d, int t) : metadata(m), declaration(d), type(t) {}
		std::string metadata;
		std::string declaration;
		int         type;
	};

	std::vector<SMetadataDecl> foundDeclarations;

	std::map<int, std::string> typeMetadataMap;
	std::map<int, std::string> funcMetadataMap;
	std::map<int, std::string> varMetadataMap;
#endif

	std::set<std::wstring> includedScripts;

	std::set<std::string> definedWords;
};

END_AS_NAMESPACE

#endif
