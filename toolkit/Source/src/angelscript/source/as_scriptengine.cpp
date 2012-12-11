/*
   AngelCode Scripting Library
   Copyright (c) 2003-2012 Andreas Jonsson

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any
   damages arising from the use of this software.

   Permission is granted to anyone to use this software for any
   purpose, including commercial applications, and to alter it and
   redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product
      documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and
      must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
      distribution.

   The original version of this library can be located at:
   http://www.angelcode.com/angelscript/

   Andreas Jonsson
   andreas@angelcode.com
*/


//
// as_scriptengine.cpp
//
// The implementation of the script engine interface
//


#include <stdlib.h>

#include "as_config.h"
#include "as_scriptengine.h"
#include "as_builder.h"
#include "as_context.h"
#include "as_string_util.h"
#include "as_tokenizer.h"
#include "as_texts.h"
#include "as_module.h"
#include "as_callfunc.h"
#include "as_generic.h"
#include "as_scriptobject.h"
#include "as_compiler.h"
#include "as_bytecode.h"
#include "as_debug.h"

BEGIN_AS_NAMESPACE



#ifdef AS_PROFILE
// Instanciate the profiler once
CProfiler g_profiler;
#endif




extern "C"
{

AS_API const char * asGetLibraryVersion()
{
#ifdef _DEBUG
	return ANGELSCRIPT_VERSION_STRING " DEBUG";
#else
	return ANGELSCRIPT_VERSION_STRING;
#endif
}

AS_API const char * asGetLibraryOptions()
{
	const char *string = " "

	// Options
#ifdef AS_MAX_PORTABILITY
		"AS_MAX_PORTABILITY "
#endif
#ifdef AS_DEBUG
		"AS_DEBUG "
#endif
#ifdef AS_NO_CLASS_METHODS
		"AS_NO_CLASS_METHODS "
#endif
#ifdef AS_USE_DOUBLE_AS_FLOAT
		"AS_USE_DOUBLE_AS_FLOAT "
#endif
#ifdef AS_64BIT_PTR
		"AS_64BIT_PTR "
#endif
#ifdef AS_NO_THREADS
		"AS_NO_THREADS "
#endif
#ifdef AS_NO_ATOMIC
		"AS_NO_ATOMIC "
#endif
#ifdef AS_NO_COMPILER
		"AS_NO_COMPILER "
#endif

	// Target system
#ifdef AS_WIN
		"AS_WIN "
#endif
#ifdef AS_LINUX
		"AS_LINUX "
#endif
#ifdef AS_MAC
		"AS_MAC "
#endif
#ifdef AS_BSD
		"AS_BSD "
#endif
#ifdef AS_XBOX
		"AS_XBOX "
#endif
#ifdef AS_XBOX360
		"AS_XBOX360 "
#endif
#ifdef AS_PSP
		"AS_PSP "
#endif
#ifdef AS_PS2
		"AS_PS2 "
#endif
#ifdef AS_PS3
		"AS_PS3 "
#endif
#ifdef AS_DC
		"AS_DC "
#endif
#ifdef AS_GC
		"AS_GC "
#endif
#ifdef AS_WII
		"AS_WII "
#endif
#ifdef AS_IPHONE
		"AS_IPHONE "
#endif
#ifdef AS_ANDROID
		"AS_ANDROID "
#endif
#ifdef AS_HAIKU
		"AS_HAIKU "
#endif
#ifdef AS_ILLUMOS
		"AS_ILLUMOS "
#endif
#ifdef AS_MARMALADE
		"AS_MARMALADE "
#endif


	// CPU family
#ifdef AS_PPC
		"AS_PPC "
#endif
#ifdef AS_PPC_64
		"AS_PPC_64 "
#endif
#ifdef AS_X86
		"AS_X86 "
#endif
#ifdef AS_MIPS
		"AS_MIPS "
#endif
#ifdef AS_SH4
		"AS_SH4 "
#endif
#ifdef AS_XENON
		"AS_XENON "
#endif
#ifdef AS_ARM
		"AS_ARM "
#endif
#ifdef AS_X64_GCC
		"AS_X64_GCC "
#endif
#ifdef AS_X64_MSVC
		"AS_X64_MSVC "
#endif
	;

	return string;
}

AS_API asIScriptEngine *asCreateScriptEngine(asDWORD version)
{
	// Verify the version that the application expects
	if( (version/10000) != (ANGELSCRIPT_VERSION/10000) )
		return 0;

	if( (version/100)%100 != (ANGELSCRIPT_VERSION/100)%100 )
		return 0;

	if( (version%100) > (ANGELSCRIPT_VERSION%100) )
		return 0;

	// Verify the size of the types
	asASSERT( sizeof(asBYTE)  == 1 );
	asASSERT( sizeof(asWORD)  == 2 );
	asASSERT( sizeof(asDWORD) == 4 );
	asASSERT( sizeof(asQWORD) == 8 );
	asASSERT( sizeof(asPWORD) == sizeof(void*) );

	// Verify the boolean type
	asASSERT( sizeof(bool) == AS_SIZEOF_BOOL );
	asASSERT( true == VALUE_OF_BOOLEAN_TRUE );

	// Verify endianess
#ifdef AS_BIG_ENDIAN
	asASSERT( *(asDWORD*)"\x00\x01\x02\x03" == 0x00010203 );
	asASSERT( *(asQWORD*)"\x00\x01\x02\x03\x04\x05\x06\x07" == ((asQWORD(0x00010203)<<32)|asQWORD(0x04050607)) );
#else
	asASSERT( *(asDWORD*)"\x00\x01\x02\x03" == 0x03020100 );
	// C++ didn't have a standard way of declaring 64bit literal constants until C++11, so
	// I'm forced to do it like this to avoid compilers warnings when compiling with the full
	// C++ compliance.
	asASSERT( *(asQWORD*)"\x00\x01\x02\x03\x04\x05\x06\x07" == ((asQWORD(0x07060504)<<32)|asQWORD(0x03020100)) );
#endif

	return asNEW(asCScriptEngine)();
}

} // extern "C"


int asCScriptEngine::SetEngineProperty(asEEngineProp property, asPWORD value)
{
	switch( property )
	{
	case asEP_ALLOW_UNSAFE_REFERENCES:
		ep.allowUnsafeReferences = value ? true : false;
		break;

	case asEP_OPTIMIZE_BYTECODE:
		ep.optimizeByteCode = value ? true : false;
		break;

	case asEP_COPY_SCRIPT_SECTIONS:
		ep.copyScriptSections = value ? true : false;
		break;

	case asEP_MAX_STACK_SIZE:
		if( value == 0 )
		{
			// Restore default: no limit and initially size 4KB
			ep.maximumContextStackSize = 0;
			initialContextStackSize    = 1024;
		}
		else
		{
			// The size is given in bytes, but we only store dwords
			ep.maximumContextStackSize = (asUINT)value/4;
			if( initialContextStackSize > ep.maximumContextStackSize )
			{
				initialContextStackSize = ep.maximumContextStackSize;
				if( initialContextStackSize == 0 )
					initialContextStackSize = 1;
			}
		}
		break;

	case asEP_USE_CHARACTER_LITERALS:
		ep.useCharacterLiterals = value ? true : false;
		break;

	case asEP_ALLOW_MULTILINE_STRINGS:
		ep.allowMultilineStrings = value ? true : false;
		break;

	case asEP_ALLOW_IMPLICIT_HANDLE_TYPES:
		ep.allowImplicitHandleTypes = value ? true : false;
		break;

	case asEP_BUILD_WITHOUT_LINE_CUES:
		ep.buildWithoutLineCues = value ? true : false;
		break;

	case asEP_INIT_GLOBAL_VARS_AFTER_BUILD:
		ep.initGlobalVarsAfterBuild = value ? true : false;
		break;

	case asEP_REQUIRE_ENUM_SCOPE:
		ep.requireEnumScope = value ? true : false;
		break;

	case asEP_SCRIPT_SCANNER:
		if( value <= 1 )
			ep.scanner = (int)value;
		else
			return asINVALID_ARG;
		break;

	case asEP_INCLUDE_JIT_INSTRUCTIONS:
		ep.includeJitInstructions = value ? true : false;
		break;

	case asEP_STRING_ENCODING:
		if( value <= 1 )
			ep.stringEncoding = (int)value;
		else
			return asINVALID_ARG;
		break;

	case asEP_PROPERTY_ACCESSOR_MODE:
		if( value <= 2 )
			ep.propertyAccessorMode = (int)value;
		else
			return asINVALID_ARG;
		break;

	case asEP_EXPAND_DEF_ARRAY_TO_TMPL:
		ep.expandDefaultArrayToTemplate = value ? true : false;
		break;

	case asEP_AUTO_GARBAGE_COLLECT:
		ep.autoGarbageCollect = value ? true : false;
		break;

	case asEP_DISALLOW_GLOBAL_VARS:
		ep.disallowGlobalVars = value ? true : false;
		break;

	case asEP_ALWAYS_IMPL_DEFAULT_CONSTRUCT:
		ep.alwaysImplDefaultConstruct = value ? true : false;
		break;

	default:
		return asINVALID_ARG;
	}

	return asSUCCESS;
}

asPWORD asCScriptEngine::GetEngineProperty(asEEngineProp property) const
{
	switch( property )
	{
	case asEP_ALLOW_UNSAFE_REFERENCES:
		return ep.allowUnsafeReferences;

	case asEP_OPTIMIZE_BYTECODE:
		return ep.optimizeByteCode;

	case asEP_COPY_SCRIPT_SECTIONS:
		return ep.copyScriptSections;

	case asEP_MAX_STACK_SIZE:
		return ep.maximumContextStackSize*4;

	case asEP_USE_CHARACTER_LITERALS:
		return ep.useCharacterLiterals;

	case asEP_ALLOW_MULTILINE_STRINGS:
		return ep.allowMultilineStrings;

	case asEP_ALLOW_IMPLICIT_HANDLE_TYPES:
		return ep.allowImplicitHandleTypes;

	case asEP_BUILD_WITHOUT_LINE_CUES:
		return ep.buildWithoutLineCues;

	case asEP_INIT_GLOBAL_VARS_AFTER_BUILD:
		return ep.initGlobalVarsAfterBuild;

	case asEP_REQUIRE_ENUM_SCOPE:
		return ep.requireEnumScope;

	case asEP_SCRIPT_SCANNER:
		return ep.scanner;

	case asEP_INCLUDE_JIT_INSTRUCTIONS:
		return ep.includeJitInstructions;

	case asEP_STRING_ENCODING:
		return ep.stringEncoding;

	case asEP_PROPERTY_ACCESSOR_MODE:
		return ep.propertyAccessorMode;

	case asEP_EXPAND_DEF_ARRAY_TO_TMPL:
		return ep.expandDefaultArrayToTemplate;

	case asEP_AUTO_GARBAGE_COLLECT:
		return ep.autoGarbageCollect;

	case asEP_DISALLOW_GLOBAL_VARS:
		return ep.disallowGlobalVars;

	case asEP_ALWAYS_IMPL_DEFAULT_CONSTRUCT:
		return ep.alwaysImplDefaultConstruct;
	}

	return 0;
}

asCScriptEngine::asCScriptEngine()
{
	asCThreadManager::Prepare();

	// Engine properties
	{
		ep.allowUnsafeReferences        = false;
		ep.optimizeByteCode             = true;
		ep.copyScriptSections           = true;
		ep.maximumContextStackSize      = 0;         // no limit
		ep.useCharacterLiterals         = false;
		ep.allowMultilineStrings        = false;
		ep.allowImplicitHandleTypes     = false;
		// TODO: optimize: Maybe this should be turned off by default? If a debugger is not used
		//                 then this is just slowing down the execution. The exception handler
		//                 should still be able to determine the line number from the bytecode
		//                 position.
		ep.buildWithoutLineCues         = false;
		ep.initGlobalVarsAfterBuild     = true;
		ep.requireEnumScope             = false;
		ep.scanner                      = 1;         // utf8. 0 = ascii
		ep.includeJitInstructions       = false;
		ep.stringEncoding               = 0;         // utf8. 1 = utf16
		ep.propertyAccessorMode         = 2;         // 0 = disable, 1 = app registered only, 2 = app and script created
		ep.expandDefaultArrayToTemplate = false;
		ep.autoGarbageCollect           = true;
		ep.disallowGlobalVars           = false;
		ep.alwaysImplDefaultConstruct   = false;
	}

	gc.engine = this;
	tok.engine = this;

	refCount.set(1);
	stringFactory = 0;
	configFailed = false;
	isPrepared = false;
	isBuilding = false;
	deferValidationOfTemplateTypes = false;
	lastModule = 0;

	// User data
	cleanModuleFunc     = 0;
	cleanContextFunc    = 0;
	cleanFunctionFunc   = 0;


	initialContextStackSize = 1024;      // 4 KB (1024 * sizeof(asDWORD)


	typeIdSeqNbr      = 0;
	currentGroup      = &defaultGroup;
	defaultAccessMask = 1;

	msgCallback = 0;
    jitCompiler = 0;

	// Create the global namespace
	defaultNamespace = AddNameSpace("");

	// We must set the namespace in the built-in types explicitly as
	// this wasn't done by the default constructor. If we do not do
	// this we will get null pointer access in other parts of the code
	scriptTypeBehaviours.nameSpace     = defaultNamespace;
	functionBehaviours.nameSpace       = defaultNamespace;
	objectTypeBehaviours.nameSpace     = defaultNamespace;
	globalPropertyBehaviours.nameSpace = defaultNamespace;

	// Reserve function id 0 for no function
	scriptFunctions.PushLast(0);

	// Make sure typeId for the built-in primitives are defined according to asETypeIdFlags
	int id = 0;
	UNUSED_VAR(id); // It is only used in debug mode
	id = GetTypeIdFromDataType(asCDataType::CreatePrimitive(ttVoid,   false)); asASSERT( id == asTYPEID_VOID   );
	id = GetTypeIdFromDataType(asCDataType::CreatePrimitive(ttBool,   false)); asASSERT( id == asTYPEID_BOOL   );
	id = GetTypeIdFromDataType(asCDataType::CreatePrimitive(ttInt8,   false)); asASSERT( id == asTYPEID_INT8   );
	id = GetTypeIdFromDataType(asCDataType::CreatePrimitive(ttInt16,  false)); asASSERT( id == asTYPEID_INT16  );
	id = GetTypeIdFromDataType(asCDataType::CreatePrimitive(ttInt,    false)); asASSERT( id == asTYPEID_INT32  );
	id = GetTypeIdFromDataType(asCDataType::CreatePrimitive(ttInt64,  false)); asASSERT( id == asTYPEID_INT64  );
	id = GetTypeIdFromDataType(asCDataType::CreatePrimitive(ttUInt8,  false)); asASSERT( id == asTYPEID_UINT8  );
	id = GetTypeIdFromDataType(asCDataType::CreatePrimitive(ttUInt16, false)); asASSERT( id == asTYPEID_UINT16 );
	id = GetTypeIdFromDataType(asCDataType::CreatePrimitive(ttUInt,   false)); asASSERT( id == asTYPEID_UINT32 );
	id = GetTypeIdFromDataType(asCDataType::CreatePrimitive(ttUInt64, false)); asASSERT( id == asTYPEID_UINT64 );
	id = GetTypeIdFromDataType(asCDataType::CreatePrimitive(ttFloat,  false)); asASSERT( id == asTYPEID_FLOAT  );
	id = GetTypeIdFromDataType(asCDataType::CreatePrimitive(ttDouble, false)); asASSERT( id == asTYPEID_DOUBLE );

	defaultArrayObjectType = 0;

	RegisterScriptObject(this);
	RegisterScriptFunction(this);
	RegisterObjectTypeGCBehaviours(this);
	asCGlobalProperty::RegisterGCBehaviours(this);
}

asCScriptEngine::~asCScriptEngine()
{
	asASSERT(refCount.get() == 0);
	asUINT n;

	// The modules must be deleted first, as they may use
	// object types from the config groups
	for( n = (asUINT)scriptModules.GetLength(); n-- > 0; )
		if( scriptModules[n] )
			asDELETE(scriptModules[n],asCModule);
	scriptModules.SetLength(0);

	GarbageCollect(asGC_FULL_CYCLE);

	// Delete the functions for template types that may references object types
	for( n = 0; n < templateTypes.GetLength(); n++ )
	{
		if( templateTypes[n] )
		{
			asUINT f;

			// Delete the factory stubs first
			for( f = 0; f < templateTypes[n]->beh.factories.GetLength(); f++ )
				scriptFunctions[templateTypes[n]->beh.factories[f]]->Release();
			templateTypes[n]->beh.factories.Allocate(0, false);

			// The list factory is not stored in the list with the rest of the factories
			if( templateTypes[n]->beh.listFactory )
			{
				scriptFunctions[templateTypes[n]->beh.listFactory]->Release();
				templateTypes[n]->beh.listFactory = 0;
			}

			// Delete the specialized functions
			for( f = 1; f < templateTypes[n]->beh.operators.GetLength(); f += 2 )
			{
				if( scriptFunctions[templateTypes[n]->beh.operators[f]]->objectType == templateTypes[n] )
				{
					scriptFunctions[templateTypes[n]->beh.operators[f]]->Release();
					templateTypes[n]->beh.operators[f] = 0;
				}
			}
			for( f = 0; f < templateTypes[n]->methods.GetLength(); f++ )
			{
				if( scriptFunctions[templateTypes[n]->methods[f]]->objectType == templateTypes[n] )
				{
					scriptFunctions[templateTypes[n]->methods[f]]->Release();
					templateTypes[n]->methods[f] = 0;
				}
			}
		}
	}

	// Do one more garbage collect to free gc objects that were global variables
	GarbageCollect(asGC_FULL_CYCLE);
	FreeUnusedGlobalProperties();
	ClearUnusedTypes();

	// Break all relationship between remaining class types and functions
	for( n = 0; n < classTypes.GetLength(); n++ )
	{
		if( classTypes[n] )
			classTypes[n]->ReleaseAllFunctions();

		if( classTypes[n]->derivedFrom )
		{
			classTypes[n]->derivedFrom->Release();
			classTypes[n]->derivedFrom = 0;
		}
	}

	GarbageCollect(asGC_FULL_CYCLE);
	FreeUnusedGlobalProperties();
	ClearUnusedTypes();

	// Destroy internals of script functions that may still be kept alive outside of engine
	for( n = 0; n < scriptFunctions.GetLength(); n++ )
		if( scriptFunctions[n] && scriptFunctions[n]->funcType == asFUNC_SCRIPT )
			scriptFunctions[n]->DestroyInternal();

	// There may be instances where one more gc cycle must be run
	GarbageCollect(asGC_FULL_CYCLE);
	ClearUnusedTypes();

	// If the application hasn't registered GC behaviours for all types
	// that can form circular references with script types, then there
	// may still be objects in the GC.
	if( gc.ReportAndReleaseUndestroyedObjects() > 0 )
	{
		// Some items cannot be destroyed because the application is still holding on to them

		// Make sure the script functions won't attempt to access the engine if they are destroyed later on
		for( n = 0; n < scriptFunctions.GetLength(); n++ )
			if( scriptFunctions[n] && scriptFunctions[n]->funcType == asFUNC_SCRIPT )
				scriptFunctions[n]->engine = 0;
	}

	asSMapNode<int,asCDataType*> *cursor = 0;
	while( mapTypeIdToDataType.MoveFirst(&cursor) )
	{
		asDELETE(mapTypeIdToDataType.GetValue(cursor),asCDataType);
		mapTypeIdToDataType.Erase(cursor);
	}

	// First remove what is not used, so that other groups can be deleted safely
	defaultGroup.RemoveConfiguration(this, true);
	while( configGroups.GetLength() )
	{
		// Delete config groups in the right order
		asCConfigGroup *grp = configGroups.PopLast();
		if( grp )
		{
			grp->RemoveConfiguration(this);
			asDELETE(grp,asCConfigGroup);
		}
	}
	// Remove what is remaining
	defaultGroup.RemoveConfiguration(this);

	asCSymbolTable<asCGlobalProperty>::iterator it = registeredGlobalProps.List();
	for( ; it; it++ )
		(*it)->Release();
	registeredGlobalProps.Clear();
	FreeUnusedGlobalProperties();

	for( n = 0; n < templateTypes.GetLength(); n++ )
	{
		if( templateTypes[n] )
		{
			// Clear the sub type before deleting the template type so that the sub type isn't freed to soon
			templateTypes[n]->templateSubType = asCDataType::CreateNullHandle();
			asDELETE(templateTypes[n],asCObjectType);
		}
	}
	templateTypes.SetLength(0);

	for( n = 0; n < objectTypes.GetLength(); n++ )
	{
		if( objectTypes[n] )
		{
			// Clear the sub type before deleting the template type so that the sub type isn't freed to soon
			objectTypes[n]->templateSubType = asCDataType::CreateNullHandle();
			asDELETE(objectTypes[n],asCObjectType);
		}
	}
	objectTypes.SetLength(0);
	for( n = 0; n < templateSubTypes.GetLength(); n++ )
	{
		if( templateSubTypes[n] )
			asDELETE(templateSubTypes[n], asCObjectType);
	}
	templateSubTypes.SetLength(0);
	registeredTypeDefs.SetLength(0);
	registeredEnums.SetLength(0);
	registeredObjTypes.SetLength(0);

	for( n = 0; n < registeredGlobalFuncs.GetLength(); n++ )
	{
		if( registeredGlobalFuncs[n] )
			registeredGlobalFuncs[n]->Release();
	}
	registeredGlobalFuncs.SetLength(0);

	scriptTypeBehaviours.ReleaseAllFunctions();
	functionBehaviours.ReleaseAllFunctions();
	objectTypeBehaviours.ReleaseAllFunctions();
	globalPropertyBehaviours.ReleaseAllFunctions();

	// Free string constants
	for( n = 0; n < stringConstants.GetLength(); n++ )
		asDELETE(stringConstants[n],asCString);
	stringConstants.SetLength(0);
	stringToIdMap.EraseAll();

	// Free the script section names
	for( n = 0; n < scriptSectionNames.GetLength(); n++ )
		asDELETE(scriptSectionNames[n],asCString);
	scriptSectionNames.SetLength(0);

	// Clean the user data
	for( n = 0; n < userData.GetLength(); n += 2 )
	{
		if( userData[n+1] )
		{
			for( asUINT c = 0; c < cleanEngineFuncs.GetLength(); c++ )
				if( cleanEngineFuncs[c].type == userData[n] )
					cleanEngineFuncs[c].cleanFunc(this);
		}
	}

	// Free namespaces
	for( n = 0; n < nameSpaces.GetLength(); n++ )
		asDELETE(nameSpaces[n], asSNameSpace);
	nameSpaces.SetLength(0);

	asCThreadManager::Unprepare();
}

// interface
int asCScriptEngine::AddRef() const
{
	return refCount.atomicInc();
}

// interface
int asCScriptEngine::Release() const
{
	int r = refCount.atomicDec();

	if( r == 0 )
	{
		asDELETE(const_cast<asCScriptEngine*>(this),asCScriptEngine);
		return 0;
	}

	return r;
}

// internal
asSNameSpace *asCScriptEngine::AddNameSpace(const char *name)
{
	// First check if it doesn't exist already
	asSNameSpace *ns = FindNameSpace(name);
	if( ns ) return ns;

	ns = asNEW(asSNameSpace);
	if( ns == 0 )
	{
		// Out of memory
		return 0;
	}
	ns->name = name;

	nameSpaces.PushLast(ns);

	return ns;
}

// internal
asSNameSpace *asCScriptEngine::FindNameSpace(const char *name)
{
	// TODO: optimize: Improve linear search
	for( asUINT n = 0; n < nameSpaces.GetLength(); n++ )
		if( nameSpaces[n]->name == name )
			return nameSpaces[n];

	return 0;
}

// interface
int asCScriptEngine::SetDefaultNamespace(const char *nameSpace)
{
	if( nameSpace == 0 )
		return ConfigError(asINVALID_ARG, "SetDefaultNamespace", nameSpace, 0);

	asCString ns = nameSpace;
	if( ns != "" )
	{
		// Make sure the namespace is composed of alternating identifier and ::
		size_t pos = 0;
		bool expectIdentifier = true;
		size_t len;
		eTokenType t = ttIdentifier;

		for( ; pos < ns.GetLength(); pos += len)
		{
			t = tok.GetToken(ns.AddressOf() + pos, ns.GetLength() - pos, &len);
			if( (expectIdentifier && t != ttIdentifier) || (!expectIdentifier && t != ttScope) )
				return ConfigError(asINVALID_DECLARATION, "SetDefaultNamespace", nameSpace, 0);

			expectIdentifier = !expectIdentifier;
		}

		// If the namespace ends with :: then strip it off
		if( t == ttScope )
			ns.SetLength(ns.GetLength()-2);
	}

	defaultNamespace = AddNameSpace(ns.AddressOf());

	return 0;
}

// interface
void *asCScriptEngine::SetUserData(void *data, asPWORD type)
{
	// As a thread might add a new new user data at the same time as another
	// it is necessary to protect both read and write access to the userData member
	ACQUIREEXCLUSIVE(engineRWLock);

	// It is not intended to store a lot of different types of userdata,
	// so a more complex structure like a associative map would just have
	// more overhead than a simple array.
	for( asUINT n = 0; n < userData.GetLength(); n += 2 )
	{
		if( userData[n] == type )
		{
			void *oldData = reinterpret_cast<void*>(userData[n+1]);
			userData[n+1] = reinterpret_cast<asPWORD>(data);

			RELEASEEXCLUSIVE(engineRWLock);

			return oldData;
		}
	}

	userData.PushLast(type);
	userData.PushLast(reinterpret_cast<asPWORD>(data));

	RELEASEEXCLUSIVE(engineRWLock);

	return 0;
}

// interface
void *asCScriptEngine::GetUserData(asPWORD type) const
{
	// There may be multiple threads reading, but when
	// setting the user data nobody must be reading.
	ACQUIRESHARED(engineRWLock);

	for( asUINT n = 0; n < userData.GetLength(); n += 2 )
	{
		if( userData[n] == type )
		{
			RELEASESHARED(engineRWLock);
			return reinterpret_cast<void*>(userData[n+1]);
		}
	}

	RELEASESHARED(engineRWLock);

	return 0;
}

// interface
int asCScriptEngine::SetMessageCallback(const asSFuncPtr &callback, void *obj, asDWORD callConv)
{
	msgCallback = true;
	msgCallbackObj = obj;
	bool isObj = false;
	if( (unsigned)callConv == asCALL_GENERIC )
	{
		msgCallback = false;
		return asNOT_SUPPORTED;
	}
	if( (unsigned)callConv >= asCALL_THISCALL )
	{
		isObj = true;
		if( obj == 0 )
		{
			msgCallback = false;
			return asINVALID_ARG;
		}
	}
	int r = DetectCallingConvention(isObj, callback, callConv, 0, &msgCallbackFunc);
	if( r < 0 ) msgCallback = false;
	return r;
}

// interface
int asCScriptEngine::ClearMessageCallback()
{
	msgCallback = false;
	return 0;
}

// interface
int asCScriptEngine::WriteMessage(const char *section, int row, int col, asEMsgType type, const char *message)
{
	// Validate input parameters
	if( section == 0 ||
		message == 0 )
		return asINVALID_ARG;

	// If there is no callback then there's nothing to do
	if( !msgCallback )
		return 0;

	asSMessageInfo msg;
	msg.section = section;
	msg.row     = row;
	msg.col     = col;
	msg.type    = type;
	msg.message = message;

	if( msgCallbackFunc.callConv < ICC_THISCALL )
		CallGlobalFunction(&msg, msgCallbackObj, &msgCallbackFunc, 0);
	else
		CallObjectMethod(msgCallbackObj, &msg, &msgCallbackFunc, 0);

	return 0;
}

int asCScriptEngine::SetJITCompiler(asIJITCompiler *compiler)
{
    jitCompiler = compiler;
    return asSUCCESS;
}

asIJITCompiler *asCScriptEngine::GetJITCompiler() const
{
    return jitCompiler;
}

// interface
asETokenClass asCScriptEngine::ParseToken(const char *string, size_t stringLength, int *tokenLength) const
{
	if( stringLength == 0 )
		stringLength = strlen(string);

	size_t len;
	asETokenClass tc;
	tok.GetToken(string, stringLength, &len, &tc);

	if( tokenLength )
		*tokenLength = (int)len;

	return tc;
}

// interface
asIScriptModule *asCScriptEngine::GetModule(const char *module, asEGMFlags flag)
{
	asCModule *mod = GetModule(module, false);

	if( flag == asGM_ALWAYS_CREATE )
	{
		if( mod != 0 )
		{
			asDELETE(mod, asCModule);
		}
		return GetModule(module, true);
	}

	if( mod == 0 && flag == asGM_CREATE_IF_NOT_EXISTS )
	{
		return GetModule(module, true);
	}

	return mod;
}

// interface
int asCScriptEngine::DiscardModule(const char *module)
{
	asCModule *mod = GetModule(module, false);
	if( mod == 0 ) return asNO_MODULE;

	asDELETE(mod, asCModule);

	FreeUnusedGlobalProperties();
	ClearUnusedTypes();

	if( ep.autoGarbageCollect )
		GarbageCollect();

	return 0;
}

// internal
void asCScriptEngine::ClearUnusedTypes()
{
	// Build a list of all types to check for
	asCArray<asCObjectType*> types;
	types = classTypes;
	types.Concatenate(templateInstanceTypes);

	// Go through all modules
	asUINT n;
	for( n = 0; n < scriptModules.GetLength() && types.GetLength(); n++ )
	{
		asCModule *mod = scriptModules[n];
		if( mod )
		{
			// Functions/Methods/Globals are handled after this

			// Go through all type declarations
			asUINT m;
			for( m = 0; m < mod->classTypes.GetLength() && types.GetLength(); m++ )
				RemoveTypeAndRelatedFromList(types, mod->classTypes[m]);
			for( m = 0; m < mod->enumTypes.GetLength() && types.GetLength(); m++ )
				RemoveTypeAndRelatedFromList(types, mod->enumTypes[m]);
			for( m = 0; m < mod->typeDefs.GetLength() && types.GetLength(); m++ )
				RemoveTypeAndRelatedFromList(types, mod->typeDefs[m]);
		}
	}

	// Go through all function parameters and remove used types
	for( n = 0; n < scriptFunctions.GetLength() && types.GetLength(); n++ )
	{
		asCScriptFunction *func = scriptFunctions[n];
		if( func )
		{
			// Ignore factory stubs
			if( func->name == "factstub" )
				continue;

			asCObjectType *ot = func->returnType.GetObjectType();
			if( ot != 0 && ot != func->objectType )
				if( func->name != ot->name )
					RemoveTypeAndRelatedFromList(types, ot);

			for( asUINT p = 0; p < func->parameterTypes.GetLength(); p++ )
			{
				ot = func->parameterTypes[p].GetObjectType();
				if( ot != 0 && ot != func->objectType )
					if( func->name != ot->name )
						RemoveTypeAndRelatedFromList(types, ot);
			}
		}
	}

	// Go through all global properties
	for( n = 0; n < globalProperties.GetLength() && types.GetLength(); n++ )
	{
		if( globalProperties[n] && globalProperties[n]->type.GetObjectType() )
			RemoveTypeAndRelatedFromList(types, globalProperties[n]->type.GetObjectType());
	}

	// All that remains in the list after this can be discarded, since they are no longer used
	for(;;)
	{
		bool didClearTemplateInstanceType = false;

		for( n = 0; n < types.GetLength(); n++ )
		{
			int refCount = 0;
			asCObjectType *type = types[n];

			// Template types and script classes will have two references for each factory stub
			if( (type->flags & asOBJ_TEMPLATE) )
			{
				refCount = 2*(int)type->beh.factories.GetLength();
				if( type->beh.listFactory )
					refCount += 2;
			}

			if( type->GetRefCount() == refCount )
			{
				if( type->flags & asOBJ_TEMPLATE )
				{
					didClearTemplateInstanceType = true;
					RemoveTemplateInstanceType(type);
				}
				else
				{
					RemoveFromTypeIdMap(type);
					asDELETE(type,asCObjectType);

					classTypes.RemoveIndexUnordered(classTypes.IndexOf(type));
				}

				// Remove the type from the array
				types.RemoveIndexUnordered(n);
				n--;
			}
		}

		if( didClearTemplateInstanceType == false )
			break;
	}
}

// internal
void asCScriptEngine::RemoveTypeAndRelatedFromList(asCArray<asCObjectType*> &types, asCObjectType *ot)
{
	// Remove the type from the list
	int i = types.IndexOf(ot);
	if( i == -1 ) return;

	types.RemoveIndexUnordered(i);

	// If the type is an template type, then remove all sub types as well
	if( ot->templateSubType.GetObjectType() )
	{
		while( ot->templateSubType.GetObjectType() )
		{
			ot = ot->templateSubType.GetObjectType();
			RemoveTypeAndRelatedFromList(types, ot);
		}
		return;
	}

	// If the type is a class, then remove all properties types as well
	if( ot->properties.GetLength() )
	{
		for( asUINT n = 0; n < ot->properties.GetLength(); n++ )
			RemoveTypeAndRelatedFromList(types, ot->properties[n]->type.GetObjectType());
	}
}


// internal
int asCScriptEngine::GetFactoryIdByDecl(const asCObjectType *ot, const char *decl)
{
	asCModule *mod = 0;

	// Is this a script class?
	if( ot->flags & asOBJ_SCRIPT_OBJECT && ot->size > 0 )
		mod = scriptFunctions[ot->beh.factories[0]]->module;

	asCBuilder bld(this, mod);

	asCScriptFunction func(this, mod, asFUNC_DUMMY);
	int r = bld.ParseFunctionDeclaration(0, decl, &func, false, 0, 0, defaultNamespace);
	if( r < 0 )
		return asINVALID_DECLARATION;

	// Search for matching factory function
	int id = -1;
	for( size_t n = 0; n < ot->beh.factories.GetLength(); n++ )
	{
		asCScriptFunction *f = scriptFunctions[ot->beh.factories[n]];
		if( f->IsSignatureEqual(&func) )
		{
			id = ot->beh.factories[n];
			break;
		}
	}

	if( id == -1 ) return asNO_FUNCTION;

	return id;
}


// internal
int asCScriptEngine::GetMethodIdByDecl(const asCObjectType *ot, const char *decl, asCModule *mod)
{
	asCBuilder bld(this, mod);

	asCScriptFunction func(this, mod, asFUNC_DUMMY);

	// Set the object type so that the signature can be properly compared
	// This cast is OK, it will only be used for comparison
	func.objectType = const_cast<asCObjectType*>(ot);

	int r = bld.ParseFunctionDeclaration(func.objectType, decl, &func, false);
	if( r < 0 )
		return asINVALID_DECLARATION;

	// Search script functions for matching interface
	int id = -1;
	for( size_t n = 0; n < ot->methods.GetLength(); ++n )
	{
		if( func.IsSignatureEqual(scriptFunctions[ot->methods[n]]) )
		{
			if( id == -1 )
				id = ot->methods[n];
			else
				return asMULTIPLE_FUNCTIONS;
		}
	}

	if( id == -1 ) return asNO_FUNCTION;

	return id;
}


// internal
asCString asCScriptEngine::GetFunctionDeclaration(int funcId)
{
	asCString str;
	asCScriptFunction *func = GetScriptFunction(funcId);
	if( func )
		str = func->GetDeclarationStr();

	return str;
}

// internal
asCScriptFunction *asCScriptEngine::GetScriptFunction(int funcId) const
{
	if( funcId < 0 || funcId >= (int)scriptFunctions.GetLength() )
		return 0;

	return scriptFunctions[funcId];
}


// interface
asIScriptContext *asCScriptEngine::CreateContext()
{
	asIScriptContext *ctx = 0;
	CreateContext(&ctx, false);
	return ctx;
}

// internal
int asCScriptEngine::CreateContext(asIScriptContext **context, bool isInternal)
{
	*context = asNEW(asCContext)(this, !isInternal);
	if( *context == 0 )
		return asOUT_OF_MEMORY;

	// We need to make sure the engine has been
	// prepared before any context is executed
	PrepareEngine();

	return 0;
}

// interface
int asCScriptEngine::RegisterObjectProperty(const char *obj, const char *declaration, int byteOffset)
{
	int r;
	asCDataType dt;
	asCBuilder bld(this, 0);
	r = bld.ParseDataType(obj, &dt, defaultNamespace);
	if( r < 0 )
		return ConfigError(r, "RegisterObjectProperty", obj, declaration);

	// Verify that the correct config group is used
	if( currentGroup->FindType(dt.GetObjectType()->name.AddressOf()) == 0 )
		return ConfigError(asWRONG_CONFIG_GROUP, "RegisterObjectProperty", obj, declaration);

	asCDataType type;
	asCString name;

	if( (r = bld.VerifyProperty(&dt, declaration, name, type, 0)) < 0 )
		return ConfigError(r, "RegisterObjectProperty", obj, declaration);

	// Store the property info
	if( dt.GetObjectType() == 0 )
		return ConfigError(asINVALID_OBJECT, "RegisterObjectProperty", obj, declaration);

	asCObjectProperty *prop = asNEW(asCObjectProperty);
	if( prop == 0 )
		return ConfigError(asOUT_OF_MEMORY, "RegisterObjectProperty", obj, declaration);

	prop->name       = name;
	prop->type       = type;
	prop->byteOffset = byteOffset;
	prop->isPrivate  = false;
	prop->accessMask = defaultAccessMask;

	dt.GetObjectType()->properties.PushLast(prop);

	currentGroup->RefConfigGroup(FindConfigGroupForObjectType(type.GetObjectType()));

	return asSUCCESS;
}

// interface
int asCScriptEngine::RegisterInterface(const char *name)
{
	if( name == 0 ) return ConfigError(asINVALID_NAME, "RegisterInterface", 0, 0);

	// Verify if the name has been registered as a type already
	asUINT n;
	for( n = 0; n < objectTypes.GetLength(); n++ )
	{
		if( objectTypes[n] && objectTypes[n]->name == name && objectTypes[n]->nameSpace == defaultNamespace )
			return asALREADY_REGISTERED;
	}

	// Use builder to parse the datatype
	asCDataType dt;
	asCBuilder bld(this, 0);
	bool oldMsgCallback = msgCallback; msgCallback = false;
	int r = bld.ParseDataType(name, &dt, defaultNamespace);
	msgCallback = oldMsgCallback;
	if( r >= 0 ) return ConfigError(asERROR, "RegisterInterface", name, 0);

	// Make sure the name is not a reserved keyword
	size_t tokenLen;
	int token = tok.GetToken(name, strlen(name), &tokenLen);
	if( token != ttIdentifier || strlen(name) != tokenLen )
		return ConfigError(asINVALID_NAME, "RegisterInterface", name, 0);

	r = bld.CheckNameConflict(name, 0, 0, defaultNamespace);
	if( r < 0 )
		return ConfigError(asNAME_TAKEN, "RegisterInterface", name, 0);

	// Don't have to check against members of object
	// types as they are allowed to use the names

	// Register the object type for the interface
	asCObjectType *st = asNEW(asCObjectType)(this);
	if( st == 0 )
		return ConfigError(asOUT_OF_MEMORY, "RegisterInterface", name, 0);

	st->flags = asOBJ_REF | asOBJ_SCRIPT_OBJECT | asOBJ_SHARED;
	st->size = 0; // Cannot be instanciated
	st->name = name;
	st->nameSpace = defaultNamespace;

	// Use the default script class behaviours
	st->beh.factory = 0;
	st->beh.addref = scriptTypeBehaviours.beh.addref;
	scriptFunctions[st->beh.addref]->AddRef();
	st->beh.release = scriptTypeBehaviours.beh.release;
	scriptFunctions[st->beh.release]->AddRef();
	st->beh.copy = 0;

	objectTypes.PushLast(st);
	registeredObjTypes.PushLast(st);

	currentGroup->objTypes.PushLast(st);

	return asSUCCESS;
}

// interface
int asCScriptEngine::RegisterInterfaceMethod(const char *intf, const char *declaration)
{
	// Verify that the correct config group is set.
	if( currentGroup->FindType(intf) == 0 )
		return ConfigError(asWRONG_CONFIG_GROUP, "RegisterInterfaceMethod", intf, declaration);

	asCDataType dt;
	asCBuilder bld(this, 0);
	int r = bld.ParseDataType(intf, &dt, defaultNamespace);
	if( r < 0 )
		return ConfigError(r, "RegisterInterfaceMethod", intf, declaration);

	asCScriptFunction *func = asNEW(asCScriptFunction)(this, 0, asFUNC_INTERFACE);
	if( func == 0 )
		return ConfigError(asOUT_OF_MEMORY, "RegisterInterfaceMethod", intf, declaration);

	func->objectType = dt.GetObjectType();

	r = bld.ParseFunctionDeclaration(func->objectType, declaration, func, false);
	if( r < 0 )
	{
		asDELETE(func,asCScriptFunction);
		return ConfigError(asINVALID_DECLARATION, "RegisterInterfaceMethod", intf, declaration);
	}

	// Check name conflicts
	r = bld.CheckNameConflictMember(dt.GetObjectType(), func->name.AddressOf(), 0, 0, false);
	if( r < 0 )
	{
		asDELETE(func,asCScriptFunction);
		return ConfigError(asNAME_TAKEN, "RegisterInterfaceMethod", intf, declaration);
	}

	func->id = GetNextScriptFunctionId();
	SetScriptFunction(func);
	func->objectType->methods.PushLast(func->id);
	// The refCount was already set to 1

	func->ComputeSignatureId();

	// If parameter type from other groups are used, add references
	// TODO: The code for adding references to config groups is repeated in a lot of places
	if( func->returnType.GetObjectType() )
	{
		asCConfigGroup *group = FindConfigGroupForObjectType(func->returnType.GetObjectType());
		currentGroup->RefConfigGroup(group);
	}
	for( asUINT n = 0; n < func->parameterTypes.GetLength(); n++ )
	{
		if( func->parameterTypes[n].GetObjectType() )
		{
			asCConfigGroup *group = FindConfigGroupForObjectType(func->parameterTypes[n].GetObjectType());
			currentGroup->RefConfigGroup(group);
		}
	}

	// Return function id as success
	return func->id;
}

int asCScriptEngine::RegisterObjectType(const char *name, int byteSize, asDWORD flags)
{
	int r;

	isPrepared = false;

	// Verify flags
	//   Must have either asOBJ_REF or asOBJ_VALUE
	if( flags & asOBJ_REF )
	{
		// Can optionally have the asOBJ_GC, asOBJ_NOHANDLE, asOBJ_SCOPED, or asOBJ_TEMPLATE flag set, but nothing else
		if( flags & ~(asOBJ_REF | asOBJ_GC | asOBJ_NOHANDLE | asOBJ_SCOPED | asOBJ_TEMPLATE | asOBJ_NOCOUNT) )
			return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);

		// flags are exclusive
		if( (flags & asOBJ_GC) && (flags & (asOBJ_NOHANDLE|asOBJ_SCOPED|asOBJ_NOCOUNT)) )
			return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);
		if( (flags & asOBJ_NOHANDLE) && (flags & (asOBJ_GC|asOBJ_SCOPED|asOBJ_NOCOUNT)) )
			return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);
		if( (flags & asOBJ_SCOPED) && (flags & (asOBJ_GC|asOBJ_NOHANDLE|asOBJ_NOCOUNT)) )
			return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);
		if( (flags & asOBJ_NOCOUNT) && (flags & (asOBJ_GC|asOBJ_NOHANDLE|asOBJ_SCOPED)) )
			return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);
	}
	else if( flags & asOBJ_VALUE )
	{
		// Cannot use reference flags
		// TODO: template: Should be possible to register a value type as template type
		if( flags & (asOBJ_REF | asOBJ_GC | asOBJ_SCOPED | asOBJ_TEMPLATE | asOBJ_NOCOUNT) )
			return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);

		// flags are exclusive
		if( (flags & asOBJ_POD) && (flags & asOBJ_ASHANDLE) )
			return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);

		// If the app type is given, we must validate the flags
		if( flags & asOBJ_APP_CLASS )
		{
			// Must not set the primitive or float flag
			if( flags & (asOBJ_APP_PRIMITIVE | asOBJ_APP_FLOAT) )
				return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);
		}
		else if( flags & asOBJ_APP_PRIMITIVE )
		{
			// Must not set the class flags nor the float flag
			if( flags & (asOBJ_APP_CLASS                  |
				         asOBJ_APP_CLASS_CONSTRUCTOR      |
						 asOBJ_APP_CLASS_DESTRUCTOR       |
						 asOBJ_APP_CLASS_ASSIGNMENT       |
						 asOBJ_APP_CLASS_COPY_CONSTRUCTOR |
						 asOBJ_APP_FLOAT                  |
						 asOBJ_APP_CLASS_ALLINTS          |
						 asOBJ_APP_CLASS_ALLFLOATS) )
				return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);
		}
		else if( flags & asOBJ_APP_FLOAT )
		{
			// Must not set the class flags nor the primitive flag
			if( flags & (asOBJ_APP_CLASS                  |
				         asOBJ_APP_CLASS_CONSTRUCTOR      |
						 asOBJ_APP_CLASS_DESTRUCTOR       |
						 asOBJ_APP_CLASS_ASSIGNMENT       |
						 asOBJ_APP_CLASS_COPY_CONSTRUCTOR |
						 asOBJ_APP_PRIMITIVE              |
						 asOBJ_APP_CLASS_ALLINTS          |
						 asOBJ_APP_CLASS_ALLFLOATS) )
				return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);
		}
		else if( flags & (asOBJ_APP_CLASS_CONSTRUCTOR      |
		                  asOBJ_APP_CLASS_DESTRUCTOR       |
		                  asOBJ_APP_CLASS_ASSIGNMENT       |
						  asOBJ_APP_CLASS_COPY_CONSTRUCTOR |
						  asOBJ_APP_CLASS_ALLINTS          |
						  asOBJ_APP_CLASS_ALLFLOATS) )
		{
			// Must not set the class properties, without the class flag
			return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);
		}
	}
	else
		return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);

	// Don't allow anything else than the defined flags
	if( flags - (flags & asOBJ_MASK_VALID_FLAGS) )
		return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);

	// Value types must have a defined size
	if( (flags & asOBJ_VALUE) && byteSize == 0 )
	{
		WriteMessage("", 0, 0, asMSGTYPE_ERROR, TXT_VALUE_TYPE_MUST_HAVE_SIZE);
		return ConfigError(asINVALID_ARG, "RegisterObjectType", name, 0);
	}

	// Verify type name
	if( name == 0 )
		return ConfigError(asINVALID_NAME, "RegisterObjectType", name, 0);

	asCString typeName;
	asCBuilder bld(this, 0);
	if( flags & asOBJ_TEMPLATE )
	{
		asCString subtypeName;
		r = bld.ParseTemplateDecl(name, &typeName, &subtypeName);
		if( r < 0 )
			return ConfigError(r, "RegisterObjectType", name, 0);

		// Verify that the template name hasn't been registered as a type already
		asUINT n;
		for( n = 0; n < objectTypes.GetLength(); n++ )
		{
			if( objectTypes[n] && objectTypes[n]->name == typeName && objectTypes[n]->nameSpace == defaultNamespace )
				// This is not an irrepairable error, as it may just be that the same type is registered twice
				return asALREADY_REGISTERED;
		}

		asCObjectType *type = asNEW(asCObjectType)(this);
		if( type == 0 )
			return ConfigError(asOUT_OF_MEMORY, "RegisterObjectType", name, 0);

		type->name       = typeName;
		type->nameSpace  = defaultNamespace;
		type->size       = byteSize;
		type->flags      = flags;
		type->accessMask = defaultAccessMask;

		// Store it in the object types
		objectTypes.PushLast(type);

		// Define a template subtype
		asCObjectType *subtype = 0;
		for( n = 0; n < templateSubTypes.GetLength(); n++ )
		{
			if( templateSubTypes[n]->name == subtypeName )
			{
				subtype = templateSubTypes[n];
				break;
			}
		}
		if( subtype == 0 )
		{
			// Create the new subtype if not already existing
			subtype = asNEW(asCObjectType)(this);
			if( subtype == 0 )
				return ConfigError(asOUT_OF_MEMORY, "RegisterObjectType", name, 0);

			subtype->name      = subtypeName;
			subtype->size      = 0;
			subtype->flags     = asOBJ_TEMPLATE_SUBTYPE;
			templateSubTypes.PushLast(subtype);
			subtype->AddRef();
		}
		type->templateSubType = asCDataType::CreateObject(subtype, false);
		subtype->AddRef();

		currentGroup->objTypes.PushLast(type);

		registeredObjTypes.PushLast(type);
	}
	else
	{
		typeName = name;

		// Verify if the name has been registered as a type already
		asUINT n;
		for( n = 0; n < objectTypes.GetLength(); n++ )
		{
			if( objectTypes[n] &&
				objectTypes[n]->name == typeName &&
				objectTypes[n]->nameSpace == defaultNamespace )
				// This is not an irrepairable error, as it may just be that the same type is registered twice
				return asALREADY_REGISTERED;
		}

		for( n = 0; n < templateTypes.GetLength(); n++ )
		{
			if( templateTypes[n] &&
				templateTypes[n]->name == typeName &&
				templateTypes[n]->nameSpace == defaultNamespace )
				// This is not an irrepairable error, as it may just be that the same type is registered twice
				return asALREADY_REGISTERED;
		}

		// Verify the most recently created template instance type
		asCObjectType *mostRecentTemplateInstanceType = 0;
		if( templateInstanceTypes.GetLength() )
			mostRecentTemplateInstanceType = templateInstanceTypes[templateInstanceTypes.GetLength()-1];

		// Use builder to parse the datatype
		asCDataType dt;
		bool oldMsgCallback = msgCallback; msgCallback = false;
		r = bld.ParseDataType(name, &dt, defaultNamespace);
		msgCallback = oldMsgCallback;

		// If the builder fails, then the type name
		// is new and it should be registered
		if( r < 0 )
		{
			// Make sure the name is not a reserved keyword
			size_t tokenLen;
			int token = tok.GetToken(name, typeName.GetLength(), &tokenLen);
			if( token != ttIdentifier || typeName.GetLength() != tokenLen )
				return ConfigError(asINVALID_NAME, "RegisterObjectType", name, 0);

			int r = bld.CheckNameConflict(name, 0, 0, defaultNamespace);
			if( r < 0 )
				return ConfigError(asNAME_TAKEN, "RegisterObjectType", name, 0);

			// Don't have to check against members of object
			// types as they are allowed to use the names

			// Put the data type in the list
			asCObjectType *type = asNEW(asCObjectType)(this);
			if( type == 0 )
				return ConfigError(asOUT_OF_MEMORY, "RegisterObjectType", name, 0);

			type->name       = typeName;
			type->nameSpace  = defaultNamespace;
			type->size       = byteSize;
			type->flags      = flags;
			type->accessMask = defaultAccessMask;

			objectTypes.PushLast(type);
			registeredObjTypes.PushLast(type);

			currentGroup->objTypes.PushLast(type);
		}
		else
		{
			// The application is registering a template specialization so we
			// need to replace the template instance type with the new type.

			// TODO: Template: We don't require the lower dimensions to be registered first for registered template types
			// int[][] must not be allowed to be registered
			// if int[] hasn't been registered first
			if( dt.GetSubType().IsTemplate() )
				return ConfigError(asLOWER_ARRAY_DIMENSION_NOT_REGISTERED, "RegisterObjectType", name, 0);

			if( dt.IsReadOnly() ||
				dt.IsReference() )
				return ConfigError(asINVALID_TYPE, "RegisterObjectType", name, 0);

			// Was the template instance type created before?
			if( templateInstanceTypes[templateInstanceTypes.GetLength()-1] == mostRecentTemplateInstanceType ||
				mostRecentTemplateInstanceType == dt.GetObjectType() )
				// TODO: Should have a better error message
				return ConfigError(asNOT_SUPPORTED, "RegisterObjectType", name, 0);

			// TODO: Add this again. The type is used by the factory stubs so we need to discount that
			// Is the template instance type already being used?
//			if( dt.GetObjectType()->GetRefCount() > 1 )
//				return ConfigError(asNOT_SUPPORTED, "RegisterObjectType", name, 0);

			// Put the data type in the list
			asCObjectType *type = asNEW(asCObjectType)(this);
			if( type == 0 )
				return ConfigError(asOUT_OF_MEMORY, "RegisterObjectType", name, 0);

			type->name       = dt.GetObjectType()->name;
			// The namespace will be the same as the original template type
			type->nameSpace  = dt.GetObjectType()->nameSpace;
			type->templateSubType = dt.GetSubType();
			if( type->templateSubType.GetObjectType() ) type->templateSubType.GetObjectType()->AddRef();
			type->size       = byteSize;
			type->flags      = flags;
			type->accessMask = defaultAccessMask;

			templateTypes.PushLast(type);

			currentGroup->objTypes.PushLast(type);

			// Remove the template instance type, which will no longer be used.
			RemoveTemplateInstanceType(dt.GetObjectType());
		}
	}

	// Return the type id as the success (except for template types)
	if( flags & asOBJ_TEMPLATE )
		return asSUCCESS;

	return GetTypeIdByDecl(name);
}

// interface
int asCScriptEngine::RegisterObjectBehaviour(const char *datatype, asEBehaviours behaviour, const char *decl, const asSFuncPtr &funcPointer, asDWORD callConv)
{
	if( datatype == 0 ) return ConfigError(asINVALID_ARG, "RegisterObjectBehaviour", datatype, decl);

	// Determine the object type
	asCBuilder bld(this, 0);
	asCDataType type;
	int r = bld.ParseDataType(datatype, &type, defaultNamespace);
	if( r < 0 )
		return ConfigError(r, "RegisterObjectBehaviour", datatype, decl);

	if( type.GetObjectType() == 0 )
		return ConfigError(asINVALID_TYPE, "RegisterObjectBehaviour", datatype, decl);

	if( type.IsReadOnly() || type.IsReference() )
		return ConfigError(asINVALID_TYPE, "RegisterObjectBehaviour", datatype, decl);

	return RegisterBehaviourToObjectType(type.GetObjectType(), behaviour, decl, funcPointer, callConv);
}

// internal
int asCScriptEngine::RegisterBehaviourToObjectType(asCObjectType *objectType, asEBehaviours behaviour, const char *decl, const asSFuncPtr &funcPointer, asDWORD callConv)
{
	asSSystemFunctionInterface internal;
	if( behaviour == asBEHAVE_FACTORY ||
		behaviour == asBEHAVE_LIST_FACTORY ||
		behaviour == asBEHAVE_TEMPLATE_CALLBACK )
	{
#ifdef AS_MAX_PORTABILITY
		if( callConv != asCALL_GENERIC )
			return ConfigError(asNOT_SUPPORTED, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
#endif
		int r = DetectCallingConvention(false, funcPointer, callConv, 0, &internal);
		if( r < 0 )
			return ConfigError(r, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
	}
	else
	{
#ifdef AS_MAX_PORTABILITY
		if( callConv != asCALL_GENERIC )
			return ConfigError(asNOT_SUPPORTED, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
#else
		if( callConv != asCALL_THISCALL &&
			callConv != asCALL_CDECL_OBJLAST &&
			callConv != asCALL_CDECL_OBJFIRST &&
			callConv != asCALL_GENERIC )
			return ConfigError(asNOT_SUPPORTED, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
#endif

		int r = DetectCallingConvention(true, funcPointer, callConv, 0, &internal);
		if( r < 0 )
			return ConfigError(r, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
	}

	isPrepared = false;

	asSTypeBehaviour *beh = &objectType->beh;

	// Verify function declaration
	asCScriptFunction func(this, 0, asFUNC_DUMMY);

	asCBuilder bld(this, 0);
	int r = bld.ParseFunctionDeclaration(objectType, decl, &func, true, &internal.paramAutoHandles, &internal.returnAutoHandle);
	if( r < 0 )
		return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
	func.name.Format("_beh_%d_", behaviour);

	if( behaviour != asBEHAVE_FACTORY && behaviour != asBEHAVE_LIST_FACTORY )
		func.objectType = objectType;

	// Check if the method restricts that use of the template to value types or reference types
	if( objectType->flags & asOBJ_TEMPLATE )
	{
		if( func.returnType.GetObjectType() == objectType->templateSubType.GetObjectType() )
		{
			if( func.returnType.IsObjectHandle() )
				objectType->acceptValueSubType = false;
			else if( !func.returnType.IsReference() )
				objectType->acceptRefSubType = false;
		}

		for( asUINT n = 0; n < func.parameterTypes.GetLength(); n++ )
		{
			if( func.parameterTypes[n].GetObjectType() == objectType->templateSubType.GetObjectType() )
			{
				// TODO: If unsafe references are allowed, then inout references allow value types
				if( func.parameterTypes[n].IsObjectHandle() || (func.parameterTypes[n].IsReference() && func.inOutFlags[n] == asTM_INOUTREF) )
					objectType->acceptValueSubType = false;
				else if( !func.parameterTypes[n].IsReference() )
					objectType->acceptRefSubType = false;
			}
		}
	}

	if( behaviour == asBEHAVE_CONSTRUCT )
	{
		// TODO: Add asBEHAVE_IMPLICIT_CONSTRUCT

		// Verify that the return type is void
		if( func.returnType != asCDataType::CreatePrimitive(ttVoid, false) )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		if( objectType->flags & asOBJ_SCRIPT_OBJECT )
		{
			// The script object is a special case
			asASSERT(func.parameterTypes.GetLength() == 1);

			beh->construct = AddBehaviourFunction(func, internal);
			beh->factory   = beh->construct;
			scriptFunctions[beh->factory]->AddRef();
			beh->constructors.PushLast(beh->construct);
			beh->factories.PushLast(beh->factory);
			func.id = beh->construct;
		}
		else
		{
			// Verify that it is a value type
			if( !(func.objectType->flags & asOBJ_VALUE) )
			{
				WriteMessage("", 0, 0, asMSGTYPE_ERROR, TXT_ILLEGAL_BEHAVIOUR_FOR_TYPE);
				return ConfigError(asILLEGAL_BEHAVIOUR_FOR_TYPE, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
			}

			// TODO: Add support for implicit constructors

			// TODO: Verify that the same constructor hasn't been registered already

			// Store all constructors in a list
			func.id = AddBehaviourFunction(func, internal);
			beh->constructors.PushLast(func.id);
			if( func.parameterTypes.GetLength() == 0 )
			{
				beh->construct = func.id;
			}
			else if( func.parameterTypes.GetLength() == 1 )
			{
				// Is this the copy constructor?
				asCDataType paramType = func.parameterTypes[0];

				// If the parameter is object, and const reference for input,
				// and same type as this class, then this is a copy constructor.
				if( paramType.IsObject() && paramType.IsReference() && paramType.IsReadOnly() && func.inOutFlags[0] == asTM_INREF && paramType.GetObjectType() == objectType )
					beh->copyconstruct = func.id;
			}
		}
	}
	else if( behaviour == asBEHAVE_DESTRUCT )
	{
		// Must be a value type
		if( !(func.objectType->flags & asOBJ_VALUE) )
		{
			WriteMessage("", 0, 0, asMSGTYPE_ERROR, TXT_ILLEGAL_BEHAVIOUR_FOR_TYPE);
			return ConfigError(asILLEGAL_BEHAVIOUR_FOR_TYPE, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
		}

		if( beh->destruct )
			return ConfigError(asALREADY_REGISTERED, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// Verify that the return type is void
		if( func.returnType != asCDataType::CreatePrimitive(ttVoid, false) )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// Verify that there are no parameters
		if( func.parameterTypes.GetLength() > 0 )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		func.id = beh->destruct = AddBehaviourFunction(func, internal);
	}
	else if( behaviour == asBEHAVE_FACTORY || behaviour == asBEHAVE_LIST_FACTORY )
	{
		// Must be a ref type and must not have asOBJ_NOHANDLE
		if( !(objectType->flags & asOBJ_REF) || (objectType->flags & asOBJ_NOHANDLE) )
		{
			WriteMessage("", 0, 0, asMSGTYPE_ERROR, TXT_ILLEGAL_BEHAVIOUR_FOR_TYPE);
			return ConfigError(asILLEGAL_BEHAVIOUR_FOR_TYPE, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
		}

		// Verify that the return type is a handle to the type
		if( func.returnType != asCDataType::CreateObjectHandle(objectType, false) )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// TODO: Add support for implicit factories

		// TODO: Verify that the same factory function hasn't been registered already

		// The templates take a hidden parameter with the object type
		if( (objectType->flags & asOBJ_TEMPLATE) &&
			(func.parameterTypes.GetLength() == 0 ||
			 !func.parameterTypes[0].IsReference()) )
		{
			// TODO: Give proper error message that explain that the first parameter is expected to be a reference
			//       The library should try to avoid having to read the manual as much as possible.
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
		}

		// Store all factory functions in a list
		func.id = AddBehaviourFunction(func, internal);

		// The list factory is a special factory and isn't stored together with the rest
		if( behaviour != asBEHAVE_LIST_FACTORY )
			beh->factories.PushLast(func.id);

		if( (func.parameterTypes.GetLength() == 0) ||
			(func.parameterTypes.GetLength() == 1 && (objectType->flags & asOBJ_TEMPLATE)) )
		{
			beh->factory = func.id;
		}
		else if( (func.parameterTypes.GetLength() == 1) ||
				 (func.parameterTypes.GetLength() == 2 && (objectType->flags & asOBJ_TEMPLATE)) )
		{
			if( behaviour == asBEHAVE_LIST_FACTORY )
				beh->listFactory = func.id;
			else
			{
				// Is this the copy factory?
				asCDataType paramType = func.parameterTypes[func.parameterTypes.GetLength()-1];

				// If the parameter is object, and const reference for input,
				// and same type as this class, then this is a copy constructor.
				if( paramType.IsObject() && paramType.IsReference() && paramType.IsReadOnly() && func.inOutFlags[func.parameterTypes.GetLength()-1] == asTM_INREF && paramType.GetObjectType() == objectType )
					beh->copyfactory = func.id;
			}
		}
	}
	else if( behaviour == asBEHAVE_ADDREF )
	{
		// Must be a ref type and must not have asOBJ_NOHANDLE, nor asOBJ_SCOPED
		if( !(func.objectType->flags & asOBJ_REF) ||
			(func.objectType->flags & asOBJ_NOHANDLE) ||
			(func.objectType->flags & asOBJ_SCOPED) ||
			(func.objectType->flags & asOBJ_NOCOUNT) )
		{
			WriteMessage("", 0, 0, asMSGTYPE_ERROR, TXT_ILLEGAL_BEHAVIOUR_FOR_TYPE);
			return ConfigError(asILLEGAL_BEHAVIOUR_FOR_TYPE, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
		}

		if( beh->addref )
			return ConfigError(asALREADY_REGISTERED, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// Verify that the return type is void
		if( func.returnType != asCDataType::CreatePrimitive(ttVoid, false) )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// Verify that there are no parameters
		if( func.parameterTypes.GetLength() > 0 )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		func.id = beh->addref = AddBehaviourFunction(func, internal);
	}
	else if( behaviour == asBEHAVE_RELEASE )
	{
		// Must be a ref type and must not have asOBJ_NOHANDLE
		if( !(func.objectType->flags & asOBJ_REF) ||
			(func.objectType->flags & asOBJ_NOHANDLE) ||
			(func.objectType->flags & asOBJ_NOCOUNT) )
		{
			WriteMessage("", 0, 0, asMSGTYPE_ERROR, TXT_ILLEGAL_BEHAVIOUR_FOR_TYPE);
			return ConfigError(asILLEGAL_BEHAVIOUR_FOR_TYPE, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
		}

		if( beh->release )
			return ConfigError(asALREADY_REGISTERED, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// Verify that the return type is void
		if( func.returnType != asCDataType::CreatePrimitive(ttVoid, false) )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// Verify that there are no parameters
		if( func.parameterTypes.GetLength() > 0 )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		func.id = beh->release = AddBehaviourFunction(func, internal);
	}
	else if( behaviour == asBEHAVE_TEMPLATE_CALLBACK )
	{
		// Must be a template type
		if( !(func.objectType->flags & asOBJ_TEMPLATE) )
		{
			WriteMessage("", 0, 0, asMSGTYPE_ERROR, TXT_ILLEGAL_BEHAVIOUR_FOR_TYPE);
			return ConfigError(asILLEGAL_BEHAVIOUR_FOR_TYPE, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
		}

		if( beh->templateCallback )
			return ConfigError(asALREADY_REGISTERED, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// Verify that the return type is bool
		if( func.returnType != asCDataType::CreatePrimitive(ttBool, false) )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// Verify that there are two parameters
		if( func.parameterTypes.GetLength() != 2 )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// The first parameter must be an inref (to receive the object type), and
		// the second must be a bool out ref (to return if the type should or shouldn't be garbage collected)
		if( func.inOutFlags[0] != asTM_INREF || func.inOutFlags[1] != asTM_OUTREF || !func.parameterTypes[1].IsEqualExceptRef(asCDataType::CreatePrimitive(ttBool, false)) )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		func.id = beh->templateCallback = AddBehaviourFunction(func, internal);
	}
	else if( behaviour >= asBEHAVE_FIRST_GC &&
		     behaviour <= asBEHAVE_LAST_GC )
	{
		// Only allow GC behaviours for types registered to be garbage collected
		if( !(func.objectType->flags & asOBJ_GC) )
		{
			WriteMessage("", 0, 0, asMSGTYPE_ERROR, TXT_ILLEGAL_BEHAVIOUR_FOR_TYPE);
			return ConfigError(asILLEGAL_BEHAVIOUR_FOR_TYPE, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
		}

		// Verify parameter count
		if( (behaviour == asBEHAVE_GETREFCOUNT ||
			 behaviour == asBEHAVE_SETGCFLAG   ||
			 behaviour == asBEHAVE_GETGCFLAG) &&
			func.parameterTypes.GetLength() != 0 )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		if( (behaviour == asBEHAVE_ENUMREFS ||
			 behaviour == asBEHAVE_RELEASEREFS) &&
			func.parameterTypes.GetLength() != 1 )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// Verify return type
		if( behaviour == asBEHAVE_GETREFCOUNT &&
			func.returnType != asCDataType::CreatePrimitive(ttInt, false) )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		if( behaviour == asBEHAVE_GETGCFLAG &&
			func.returnType != asCDataType::CreatePrimitive(ttBool, false) )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		if( (behaviour == asBEHAVE_SETGCFLAG ||
			 behaviour == asBEHAVE_ENUMREFS  ||
			 behaviour == asBEHAVE_RELEASEREFS) &&
			func.returnType != asCDataType::CreatePrimitive(ttVoid, false) )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		if( behaviour == asBEHAVE_GETREFCOUNT )
			func.id = beh->gcGetRefCount = AddBehaviourFunction(func, internal);
		else if( behaviour == asBEHAVE_SETGCFLAG )
			func.id = beh->gcSetFlag = AddBehaviourFunction(func, internal);
		else if( behaviour == asBEHAVE_GETGCFLAG )
			func.id = beh->gcGetFlag = AddBehaviourFunction(func, internal);
		else if( behaviour == asBEHAVE_ENUMREFS )
			func.id = beh->gcEnumReferences = AddBehaviourFunction(func, internal);
		else if( behaviour == asBEHAVE_RELEASEREFS )
			func.id = beh->gcReleaseAllReferences = AddBehaviourFunction(func, internal);
	}
	else if( behaviour == asBEHAVE_IMPLICIT_VALUE_CAST ||
		     behaviour == asBEHAVE_VALUE_CAST )
	{
		// Verify parameter count
		if( func.parameterTypes.GetLength() != 0 )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// Verify return type
		if( func.returnType.IsEqualExceptRefAndConst(asCDataType::CreatePrimitive(ttBool, false)) )
			return ConfigError(asNOT_SUPPORTED, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		if( func.returnType.IsEqualExceptRefAndConst(asCDataType::CreatePrimitive(ttVoid, false)) )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// TODO: verify that the same cast is not registered already (const or non-const is treated the same for the return type)

		beh->operators.PushLast(behaviour);
		func.id = AddBehaviourFunction(func, internal);
		beh->operators.PushLast(func.id);
	}
	else if( behaviour == asBEHAVE_REF_CAST ||
	         behaviour == asBEHAVE_IMPLICIT_REF_CAST )
	{
		// There are two allowed signatures
		//  1. obj @f()
		//  2. void f(?&out)

		if( !(func.parameterTypes.GetLength() == 0 && func.returnType.IsObjectHandle()) &&
			!(func.parameterTypes.GetLength() == 1 && func.parameterTypes[0].GetTokenType() == ttQuestion && func.inOutFlags[0] == asTM_OUTREF && func.returnType.GetTokenType() == ttVoid) )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// Currently it is not supported to register const overloads for the ref cast behaviour
		if( func.IsReadOnly() )
			return ConfigError(asINVALID_DECLARATION, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

		// Verify that the same cast is not registered already
		// (const or non-const is treated the same for the return type)
		if( func.parameterTypes.GetLength() == 1 )
		{
			// Check for existing behaviour with ?&out
			for( asUINT n = 0; n < beh->operators.GetLength(); n+= 2 )
			{
				if( beh->operators[n] == asBEHAVE_REF_CAST ||
					beh->operators[n] == asBEHAVE_IMPLICIT_REF_CAST )
				{
					asCScriptFunction *f = scriptFunctions[beh->operators[n+1]];
					if( f->parameterTypes.GetLength() == 1 )
						return ConfigError(asALREADY_REGISTERED, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
				}
			}
		}
		else
		{
			// Check for existing behaviour with same return type
			for( asUINT n = 0; n < beh->operators.GetLength(); n+= 2 )
			{
				if( beh->operators[n] == asBEHAVE_REF_CAST ||
					beh->operators[n] == asBEHAVE_IMPLICIT_REF_CAST )
				{
					asCScriptFunction *f = scriptFunctions[beh->operators[n+1]];
					if( f->returnType.GetObjectType() == func.returnType.GetObjectType() )
						return ConfigError(asALREADY_REGISTERED, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
				}
			}
		}

		beh->operators.PushLast(behaviour);
		func.id = AddBehaviourFunction(func, internal);
		beh->operators.PushLast(func.id);
	}
	else
	{
		asASSERT(false);

		return ConfigError(asINVALID_ARG, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);
	}

	if( func.id < 0 )
		return ConfigError(func.id, "RegisterObjectBehaviour", objectType->name.AddressOf(), decl);

	// Return function id as success
	return func.id;
}


int asCScriptEngine::VerifyVarTypeNotInFunction(asCScriptFunction *func)
{
	// Don't allow var type in this function
	if( func->returnType.GetTokenType() == ttQuestion )
		return asINVALID_DECLARATION;

	for( unsigned int n = 0; n < func->parameterTypes.GetLength(); n++ )
		if( func->parameterTypes[n].GetTokenType() == ttQuestion )
			return asINVALID_DECLARATION;

	return 0;
}

int asCScriptEngine::AddBehaviourFunction(asCScriptFunction &func, asSSystemFunctionInterface &internal)
{
	asUINT n;

	int id = GetNextScriptFunctionId();

	asSSystemFunctionInterface *newInterface = asNEW(asSSystemFunctionInterface)(internal);
	if( newInterface == 0 )
		return asOUT_OF_MEMORY;

	asCScriptFunction *f = asNEW(asCScriptFunction)(this, 0, asFUNC_SYSTEM);
	if( f == 0 )
	{
		asDELETE(newInterface, asSSystemFunctionInterface);
		return asOUT_OF_MEMORY;
	}

	asASSERT(func.name != "" && func.name != "f");
	f->name           = func.name;
	f->sysFuncIntf    = newInterface;
	f->returnType     = func.returnType;
	f->objectType     = func.objectType;
	f->id             = id;
	f->isReadOnly     = func.isReadOnly;
	f->accessMask     = defaultAccessMask;
	f->parameterTypes = func.parameterTypes;
	f->inOutFlags     = func.inOutFlags;
	for( n = 0; n < func.defaultArgs.GetLength(); n++ )
		if( func.defaultArgs[n] )
			f->defaultArgs.PushLast(asNEW(asCString)(*func.defaultArgs[n]));
		else
			f->defaultArgs.PushLast(0);

	SetScriptFunction(f);

	// If parameter type from other groups are used, add references
	if( f->returnType.GetObjectType() )
	{
		asCConfigGroup *group = FindConfigGroupForObjectType(f->returnType.GetObjectType());
		currentGroup->RefConfigGroup(group);
	}
	for( n = 0; n < f->parameterTypes.GetLength(); n++ )
	{
		if( f->parameterTypes[n].GetObjectType() )
		{
			asCConfigGroup *group = FindConfigGroupForObjectType(f->parameterTypes[n].GetObjectType());
			currentGroup->RefConfigGroup(group);
		}
	}

	return id;
}

// interface
int asCScriptEngine::RegisterGlobalProperty(const char *declaration, void *pointer)
{
	// Don't accept a null pointer
	if( pointer == 0 )
		return ConfigError(asINVALID_ARG, "RegisterGlobalProperty", declaration, 0);

	asCDataType type;
	asCString name;

	int r;
	asCBuilder bld(this, 0);
	if( (r = bld.VerifyProperty(0, declaration, name, type, defaultNamespace)) < 0 )
		return ConfigError(r, "RegisterGlobalProperty", declaration, 0);

	// Don't allow registering references as global properties
	if( type.IsReference() )
		return ConfigError(asINVALID_TYPE, "RegisterGlobalProperty", declaration, 0);

	// Store the property info
	asCGlobalProperty *prop = AllocateGlobalProperty();
	prop->name        = name;
	prop->nameSpace   = defaultNamespace;
	prop->type        = type;
	prop->accessMask  = defaultAccessMask;

	prop->SetRegisteredAddress(pointer);
	varAddressMap.Insert(prop->GetAddressOfValue(), prop);

	registeredGlobalProps.Put(prop);
	currentGroup->globalProps.PushLast(prop);

	// If from another group add reference
	if( type.GetObjectType() )
	{
		asCConfigGroup *group = FindConfigGroupForObjectType(type.GetObjectType());
		currentGroup->RefConfigGroup(group);
	}

	return asSUCCESS;
}

// internal
asCGlobalProperty *asCScriptEngine::AllocateGlobalProperty()
{
	asCGlobalProperty *prop = asNEW(asCGlobalProperty);
	if( prop == 0 )
	{
		// Out of memory
		return 0;
	}

	// First check the availability of a free slot
	if( freeGlobalPropertyIds.GetLength() )
	{
		prop->id = freeGlobalPropertyIds.PopLast();
		globalProperties[prop->id] = prop;
		return prop;
	}

	prop->id = (asUINT)globalProperties.GetLength();
	globalProperties.PushLast(prop);
	return prop;
}

// internal
void asCScriptEngine::FreeUnusedGlobalProperties()
{
	for( asUINT n = 0; n < globalProperties.GetLength(); n++ )
	{
		if( globalProperties[n] && globalProperties[n]->GetRefCount() == 0 )
		{
			freeGlobalPropertyIds.PushLast(n);

			asSMapNode<void*, asCGlobalProperty*> *node;
			varAddressMap.MoveTo(&node, globalProperties[n]->GetAddressOfValue());
			asASSERT(node);
			if( node )
				varAddressMap.Erase(node);

			asDELETE(globalProperties[n], asCGlobalProperty);
			globalProperties[n] = 0;
		}
	}
}

// interface
asUINT asCScriptEngine::GetGlobalPropertyCount() const
{
	return asUINT(registeredGlobalProps.GetSize());
}

// interface
// TODO: If the typeId ever encodes the const flag, then the isConst parameter should be removed
int asCScriptEngine::GetGlobalPropertyByIndex(asUINT index, const char **name, const char **nameSpace, int *typeId, bool *isConst, const char **configGroup, void **pointer, asDWORD *accessMask) const
{
	const asCGlobalProperty *prop = registeredGlobalProps.Get(index);
	if( !prop )
		return asINVALID_ARG;

	if( name )       *name       = prop->name.AddressOf();
	if( nameSpace )  *nameSpace  = prop->nameSpace->name.AddressOf();
	if( typeId )     *typeId     = GetTypeIdFromDataType(prop->type);
	if( isConst )    *isConst    = prop->type.IsReadOnly();
	if( pointer )    *pointer    = prop->GetRegisteredAddress();
	if( accessMask ) *accessMask = prop->accessMask;

	if( configGroup )
	{
		asCConfigGroup *group = FindConfigGroupForGlobalVar(index);
		if( group )
			*configGroup = group->groupName.AddressOf();
		else
			*configGroup = 0;
	}

	return asSUCCESS;
}

// interface
int asCScriptEngine::GetGlobalPropertyIndexByName(const char *name) const
{
	// Find the global var id
	int id = registeredGlobalProps.GetFirstIndex(defaultNamespace, name);
	if( id == -1 ) return asNO_GLOBAL_VAR;

	return id;
}

// interface
int asCScriptEngine::GetGlobalPropertyIndexByDecl(const char *decl) const
{
	// This const cast is OK. The builder won't modify the engine
	asCBuilder bld(const_cast<asCScriptEngine*>(this), 0);

	asCString name;
	asSNameSpace *ns;
	asCDataType dt;
	bld.ParseVariableDeclaration(decl, defaultNamespace, name, ns, dt);

	// Search for a match
	int id = registeredGlobalProps.GetFirstIndex(ns, name, asCCompGlobPropType(dt));
	if (id < 0)
		return asNO_GLOBAL_VAR;

	return id;
}

// interface
int asCScriptEngine::RegisterObjectMethod(const char *obj, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv)
{
	if( obj == 0 )
		return ConfigError(asINVALID_ARG, "RegisterObjectMethod", obj, declaration);

	// Determine the object type
	asCDataType dt;
	asCBuilder bld(this, 0);
	int r = bld.ParseDataType(obj, &dt, defaultNamespace);
	if( r < 0 )
		return ConfigError(r, "RegisterObjectMethod", obj, declaration);

	if( dt.GetObjectType() == 0 )
		return ConfigError(asINVALID_ARG, "RegisterObjectMethod", obj, declaration);

	return RegisterMethodToObjectType(dt.GetObjectType(), declaration, funcPointer, callConv);
}

// internal
int asCScriptEngine::RegisterMethodToObjectType(asCObjectType *objectType, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv)
{
	asSSystemFunctionInterface internal;
	int r = DetectCallingConvention(true, funcPointer, callConv, 0, &internal);
	if( r < 0 )
		return ConfigError(r, "RegisterObjectMethod", objectType->name.AddressOf(), declaration);

	// We only support these calling conventions for object methods
#ifdef AS_MAX_PORTABILITY
	if( callConv != asCALL_GENERIC )
		return ConfigError(asNOT_SUPPORTED, "RegisterObjectMethod", objectType->name.AddressOf(), declaration);
#else
	if( callConv != asCALL_THISCALL &&
		callConv != asCALL_CDECL_OBJLAST &&
		callConv != asCALL_CDECL_OBJFIRST &&
		callConv != asCALL_GENERIC )
		return ConfigError(asNOT_SUPPORTED, "RegisterObjectMethod", objectType->name.AddressOf(), declaration);
#endif

	isPrepared = false;

	// Put the system function in the list of system functions
	asSSystemFunctionInterface *newInterface = asNEW(asSSystemFunctionInterface)(internal);
	if( newInterface == 0 )
		return ConfigError(asOUT_OF_MEMORY, "RegisterObjectMethod", objectType->name.AddressOf(), declaration);

	asCScriptFunction *func = asNEW(asCScriptFunction)(this, 0, asFUNC_SYSTEM);
	if( func == 0 )
	{
		asDELETE(newInterface, asSSystemFunctionInterface);
		return ConfigError(asOUT_OF_MEMORY, "RegisterObjectMethod", objectType->name.AddressOf(), declaration);
	}

	func->sysFuncIntf = newInterface;
	func->objectType  = objectType;

	asCBuilder bld(this, 0);
	r = bld.ParseFunctionDeclaration(func->objectType, declaration, func, true, &newInterface->paramAutoHandles, &newInterface->returnAutoHandle);
	if( r < 0 )
	{
		// Set as dummy function before deleting
		func->funcType = asFUNC_DUMMY;
		asDELETE(func,asCScriptFunction);
		return ConfigError(asINVALID_DECLARATION, "RegisterObjectMethod", objectType->name.AddressOf(), declaration);
	}

	// Check name conflicts
	r = bld.CheckNameConflictMember(objectType, func->name.AddressOf(), 0, 0, false);
	if( r < 0 )
	{
		func->funcType = asFUNC_DUMMY;
		asDELETE(func,asCScriptFunction);
		return ConfigError(asNAME_TAKEN, "RegisterObjectMethod", objectType->name.AddressOf(), declaration);
	}

	// Check against duplicate methods
	asUINT n;
	for( n = 0; n < func->objectType->methods.GetLength(); n++ )
	{
		asCScriptFunction *f = scriptFunctions[func->objectType->methods[n]];
		if( f->name == func->name &&
			f->IsSignatureExceptNameAndReturnTypeEqual(func) )
		{
			func->funcType = asFUNC_DUMMY;
			asDELETE(func,asCScriptFunction);
			return ConfigError(asALREADY_REGISTERED, "RegisterObjectMethod", objectType->name.AddressOf(), declaration);
		}
	}

	func->id = GetNextScriptFunctionId();
	func->objectType->methods.PushLast(func->id);
	func->accessMask = defaultAccessMask;
	SetScriptFunction(func);

	// TODO: This code is repeated in many places
	// If parameter type from other groups are used, add references
	if( func->returnType.GetObjectType() )
	{
		asCConfigGroup *group = FindConfigGroupForObjectType(func->returnType.GetObjectType());
		currentGroup->RefConfigGroup(group);
	}
	for( n = 0; n < func->parameterTypes.GetLength(); n++ )
	{
		if( func->parameterTypes[n].GetObjectType() )
		{
			asCConfigGroup *group = FindConfigGroupForObjectType(func->parameterTypes[n].GetObjectType());
			currentGroup->RefConfigGroup(group);
		}
	}

	// Check if the method restricts that use of the template to value types or reference types
	if( func->objectType->flags & asOBJ_TEMPLATE )
	{
		if( func->returnType.GetObjectType() == func->objectType->templateSubType.GetObjectType() )
		{
			if( func->returnType.IsObjectHandle() )
				func->objectType->acceptValueSubType = false;
			else if( !func->returnType.IsReference() )
				func->objectType->acceptRefSubType = false;
		}

		for( asUINT n = 0; n < func->parameterTypes.GetLength(); n++ )
		{
			if( func->parameterTypes[n].GetObjectType() == func->objectType->templateSubType.GetObjectType() )
			{
				// TODO: If unsafe references are allowed, then inout references allow value types
				if( func->parameterTypes[n].IsObjectHandle() || (func->parameterTypes[n].IsReference() && func->inOutFlags[n] == asTM_INOUTREF) )
					func->objectType->acceptValueSubType = false;
				else if( !func->parameterTypes[n].IsReference() )
					func->objectType->acceptRefSubType = false;
			}
		}
	}

	// TODO: beh.copy member will be removed, so this is not necessary
	// Is this the default copy behaviour?
	if( func->name == "opAssign" && func->parameterTypes.GetLength() == 1 && func->isReadOnly == false &&
		(objectType->flags & asOBJ_SCRIPT_OBJECT || func->parameterTypes[0].IsEqualExceptRefAndConst(asCDataType::CreateObject(func->objectType, false))) )
	{
		func->objectType->beh.copy = func->id;
		func->AddRef();
	}

	// Return the function id as success
	return func->id;
}

// interface
int asCScriptEngine::RegisterGlobalFunction(const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv, void *objForThiscall)
{
	asSSystemFunctionInterface internal;
	int r = DetectCallingConvention(false, funcPointer, callConv, objForThiscall, &internal);
	if( r < 0 )
		return ConfigError(r, "RegisterGlobalFunction", declaration, 0);

#ifdef AS_MAX_PORTABILITY
	if( callConv != asCALL_GENERIC )
		return ConfigError(asNOT_SUPPORTED, "RegisterGlobalFunction", declaration, 0);
#else
	if( callConv != asCALL_CDECL &&
		callConv != asCALL_STDCALL &&
		callConv != asCALL_THISCALL_ASGLOBAL &&
		callConv != asCALL_GENERIC )
		return ConfigError(asNOT_SUPPORTED, "RegisterGlobalFunction", declaration, 0);
#endif

	isPrepared = false;

	// Put the system function in the list of system functions
	asSSystemFunctionInterface *newInterface = asNEW(asSSystemFunctionInterface)(internal);
	if( newInterface == 0 )
		return ConfigError(asOUT_OF_MEMORY, "RegisterGlobalFunction", declaration, 0);

	asCScriptFunction *func = asNEW(asCScriptFunction)(this, 0, asFUNC_SYSTEM);
	if( func == 0 )
	{
		asDELETE(newInterface, asSSystemFunctionInterface);
		return ConfigError(asOUT_OF_MEMORY, "RegisterGlobalFunction", declaration, 0);
	}

	func->sysFuncIntf = newInterface;

	asCBuilder bld(this, 0);
	r = bld.ParseFunctionDeclaration(0, declaration, func, true, &newInterface->paramAutoHandles, &newInterface->returnAutoHandle, defaultNamespace);
	if( r < 0 )
	{
		// Set as dummy function before deleting
		func->funcType = asFUNC_DUMMY;
		asDELETE(func,asCScriptFunction);
		return ConfigError(asINVALID_DECLARATION, "RegisterGlobalFunction", declaration, 0);
	}

	// TODO: namespace: What if the declaration defined an explicit namespace?
	func->nameSpace = defaultNamespace;

	// Check name conflicts
	r = bld.CheckNameConflict(func->name.AddressOf(), 0, 0, defaultNamespace);
	if( r < 0 )
	{
		// Set as dummy function before deleting
		func->funcType = asFUNC_DUMMY;
		asDELETE(func,asCScriptFunction);
		return ConfigError(asNAME_TAKEN, "RegisterGlobalFunction", declaration, 0);
	}

	// Make sure the function is not identical to a previously registered function
	asUINT n;
	for( n = 0; n < registeredGlobalFuncs.GetLength(); n++ )
	{
		asCScriptFunction *f = registeredGlobalFuncs[n];
		if( f->name == func->name &&
			f->nameSpace == func->nameSpace &&
			f->IsSignatureExceptNameAndReturnTypeEqual(func) )
		{
			func->funcType = asFUNC_DUMMY;
			asDELETE(func,asCScriptFunction);
			return ConfigError(asALREADY_REGISTERED, "RegisterGlobalFunction", declaration, 0);
		}
	}

	func->id = GetNextScriptFunctionId();
	SetScriptFunction(func);

	currentGroup->scriptFunctions.PushLast(func);
	func->accessMask = defaultAccessMask;
	registeredGlobalFuncs.PushLast(func);

	// If parameter type from other groups are used, add references
	if( func->returnType.GetObjectType() )
	{
		asCConfigGroup *group = FindConfigGroupForObjectType(func->returnType.GetObjectType());
		currentGroup->RefConfigGroup(group);
	}
	for( n = 0; n < func->parameterTypes.GetLength(); n++ )
	{
		if( func->parameterTypes[n].GetObjectType() )
		{
			asCConfigGroup *group = FindConfigGroupForObjectType(func->parameterTypes[n].GetObjectType());
			currentGroup->RefConfigGroup(group);
		}
	}

	// Return the function id as success
	return func->id;
}

// interface
asUINT asCScriptEngine::GetGlobalFunctionCount() const
{
	return asUINT(registeredGlobalFuncs.GetLength());
}

#ifdef AS_DEPRECATED
// Deprecated since 2.24.0 - 2012-05-20
// interface
int asCScriptEngine::GetGlobalFunctionIdByIndex(asUINT index) const
{
	if( index >= registeredGlobalFuncs.GetLength() )
		return asINVALID_ARG;

	return registeredGlobalFuncs[index]->id;
}
#endif

// interface
asIScriptFunction *asCScriptEngine::GetGlobalFunctionByIndex(asUINT index) const
{
	if( index >= registeredGlobalFuncs.GetLength() )
		return 0;

	return registeredGlobalFuncs[index];
}

// interface
asIScriptFunction *asCScriptEngine::GetGlobalFunctionByDecl(const char *decl) const
{
	asCBuilder bld(const_cast<asCScriptEngine*>(this), 0);

	asCScriptFunction func(const_cast<asCScriptEngine*>(this), 0, asFUNC_DUMMY);
	int r = bld.ParseFunctionDeclaration(0, decl, &func, false, 0, 0, defaultNamespace);
	if( r < 0 )
		return 0;

	// TODO: optimize: Improve linear search
	// Search registered functions for matching interface
	int id = -1;
	for( size_t n = 0; n < registeredGlobalFuncs.GetLength(); ++n )
	{
		if( registeredGlobalFuncs[n]->objectType == 0 &&
			func.name == registeredGlobalFuncs[n]->name &&
			func.returnType == registeredGlobalFuncs[n]->returnType &&
			func.parameterTypes.GetLength() == registeredGlobalFuncs[n]->parameterTypes.GetLength() )
		{
			bool match = true;
			for( size_t p = 0; p < func.parameterTypes.GetLength(); ++p )
			{
				if( func.parameterTypes[p] != registeredGlobalFuncs[n]->parameterTypes[p] )
				{
					match = false;
					break;
				}
			}

			if( match )
			{
				if( id == -1 )
					id = registeredGlobalFuncs[n]->id;
				else
					return 0; // Multiple matches
			}
		}
	}

	if( id < 0 ) return 0; // No matches

	return scriptFunctions[id];
}


asCObjectType *asCScriptEngine::GetObjectType(const char *type, asSNameSpace *ns)
{
	// TODO: optimize: Improve linear search
	for( asUINT n = 0; n < objectTypes.GetLength(); n++ )
		if( objectTypes[n] &&
			objectTypes[n]->name == type &&
			objectTypes[n]->nameSpace == ns ) // TODO: template: Should we check the subtype in case of template instances?
			return objectTypes[n];

	return 0;
}




void asCScriptEngine::PrepareEngine()
{
	if( isPrepared ) return;
	if( configFailed ) return;

	asUINT n;
	for( n = 0; n < scriptFunctions.GetLength(); n++ )
	{
		// Determine the host application interface
		if( scriptFunctions[n] && scriptFunctions[n]->funcType == asFUNC_SYSTEM )
		{
			if( scriptFunctions[n]->sysFuncIntf->callConv == ICC_GENERIC_FUNC ||
				scriptFunctions[n]->sysFuncIntf->callConv == ICC_GENERIC_METHOD )
				PrepareSystemFunctionGeneric(scriptFunctions[n], scriptFunctions[n]->sysFuncIntf, this);
			else
				PrepareSystemFunction(scriptFunctions[n], scriptFunctions[n]->sysFuncIntf, this);
		}
	}

	// Validate object type registrations
	for( n = 0; n < objectTypes.GetLength(); n++ )
	{
		if( objectTypes[n] && !(objectTypes[n]->flags & asOBJ_SCRIPT_OBJECT) )
		{
			bool missingBehaviour = false;
			const char *infoMsg = 0;

			// Verify that GC types have all behaviours
			if( objectTypes[n]->flags & asOBJ_GC )
			{
				if( objectTypes[n]->beh.addref                 == 0 ||
					objectTypes[n]->beh.release                == 0 ||
					objectTypes[n]->beh.gcGetRefCount          == 0 ||
					objectTypes[n]->beh.gcSetFlag              == 0 ||
					objectTypes[n]->beh.gcGetFlag              == 0 ||
					objectTypes[n]->beh.gcEnumReferences       == 0 ||
					objectTypes[n]->beh.gcReleaseAllReferences == 0 )
				{
					infoMsg = TXT_GC_REQUIRE_ADD_REL_GC_BEHAVIOUR;
					missingBehaviour = true;
				}
			}
			// Verify that scoped ref types have the release behaviour
			else if( objectTypes[n]->flags & asOBJ_SCOPED )
			{
				if( objectTypes[n]->beh.release == 0 )
				{
					infoMsg = TXT_SCOPE_REQUIRE_REL_BEHAVIOUR;
					missingBehaviour = true;
				}
			}
			// Verify that ref types have add ref and release behaviours
			else if( (objectTypes[n]->flags & asOBJ_REF) &&
				     !(objectTypes[n]->flags & asOBJ_NOHANDLE) &&
					 !(objectTypes[n]->flags & asOBJ_NOCOUNT) )
			{
				if( objectTypes[n]->beh.addref  == 0 ||
					objectTypes[n]->beh.release == 0 )
				{
					infoMsg = TXT_REF_REQUIRE_ADD_REL_BEHAVIOUR;
					missingBehaviour = true;
				}
			}
			// Verify that non-pod value types have the constructor and destructor registered
			else if( (objectTypes[n]->flags & asOBJ_VALUE) &&
				     !(objectTypes[n]->flags & asOBJ_POD) )
			{
				if( objectTypes[n]->beh.construct == 0 ||
					objectTypes[n]->beh.destruct  == 0 )
				{
					infoMsg = TXT_NON_POD_REQUIRE_CONSTR_DESTR_BEHAVIOUR;
					missingBehaviour = true;
				}
			}

			if( missingBehaviour )
			{
				asCString str;
				str.Format(TXT_TYPE_s_IS_MISSING_BEHAVIOURS, objectTypes[n]->name.AddressOf());
				WriteMessage("", 0, 0, asMSGTYPE_ERROR, str.AddressOf());
				WriteMessage("", 0, 0, asMSGTYPE_INFORMATION, infoMsg);
				ConfigError(asINVALID_CONFIGURATION, 0, 0, 0);
			}
		}
	}

	isPrepared = true;
}

int asCScriptEngine::ConfigError(int err, const char *funcName, const char *arg1, const char *arg2)
{
	configFailed = true;
	if( funcName )
	{
		asCString str;
		if( arg1 )
		{
			if( arg2 )
				str.Format(TXT_FAILED_IN_FUNC_s_WITH_s_AND_s_d, funcName, arg1, arg2, err);
			else
				str.Format(TXT_FAILED_IN_FUNC_s_WITH_s_d, funcName, arg1, err);
		}
		else
			str.Format(TXT_FAILED_IN_FUNC_s_d, funcName, err);

		WriteMessage("", 0, 0, asMSGTYPE_ERROR, str.AddressOf());
	}
	return err;
}

// interface
int asCScriptEngine::RegisterDefaultArrayType(const char *type)
{
	asCBuilder bld(this, 0);
	asCDataType dt;
	int r = bld.ParseDataType(type, &dt, defaultNamespace);
	if( r < 0 ) return r;

	if( dt.GetObjectType() == 0 ||
		!(dt.GetObjectType()->GetFlags() & asOBJ_TEMPLATE) )
		return asINVALID_TYPE;

	defaultArrayObjectType = dt.GetObjectType();
	defaultArrayObjectType->AddRef();

	return 0;
}

// interface
int asCScriptEngine::GetDefaultArrayTypeId() const
{
	if( defaultArrayObjectType )
		return GetTypeIdFromDataType(asCDataType::CreateObject(defaultArrayObjectType, false));

	return asINVALID_TYPE;
}

// interface
int asCScriptEngine::RegisterStringFactory(const char *datatype, const asSFuncPtr &funcPointer, asDWORD callConv)
{
	asSSystemFunctionInterface internal;
	int r = DetectCallingConvention(false, funcPointer, callConv, 0, &internal);
	if( r < 0 )
		return ConfigError(r, "RegisterStringFactory", datatype, 0);

#ifdef AS_MAX_PORTABILITY
	if( callConv != asCALL_GENERIC )
		return ConfigError(asNOT_SUPPORTED, "RegisterStringFactory", datatype, 0);
#else
	if( callConv != asCALL_CDECL &&
		callConv != asCALL_STDCALL &&
		callConv != asCALL_GENERIC )
		return ConfigError(asNOT_SUPPORTED, "RegisterStringFactory", datatype, 0);
#endif

	// Put the system function in the list of system functions
	asSSystemFunctionInterface *newInterface = asNEW(asSSystemFunctionInterface)(internal);
	if( newInterface == 0 )
		return ConfigError(asOUT_OF_MEMORY, "RegisterStringFactory", datatype, 0);

	asCScriptFunction *func = asNEW(asCScriptFunction)(this, 0, asFUNC_SYSTEM);
	if( func == 0 )
	{
		asDELETE(newInterface, asSSystemFunctionInterface);
		return ConfigError(asOUT_OF_MEMORY, "RegisterStringFactory", datatype, 0);
	}

	func->name        = "_string_factory_";
	func->sysFuncIntf = newInterface;

	asCBuilder bld(this, 0);

	asCDataType dt;
	r = bld.ParseDataType(datatype, &dt, defaultNamespace, true);
	if( r < 0 )
	{
		// Set as dummy before deleting
		func->funcType = asFUNC_DUMMY;
		asDELETE(func,asCScriptFunction);
		return ConfigError(asINVALID_TYPE, "RegisterStringFactory", datatype, 0);
	}

	func->returnType = dt;
	func->parameterTypes.PushLast(asCDataType::CreatePrimitive(ttInt, true));
	asCDataType parm1 = asCDataType::CreatePrimitive(ttUInt8, true);
	parm1.MakeReference(true);
	func->parameterTypes.PushLast(parm1);
	func->id = GetNextScriptFunctionId();
	SetScriptFunction(func);

	stringFactory = func;

	if( func->returnType.GetObjectType() )
	{
		asCConfigGroup *group = FindConfigGroupForObjectType(func->returnType.GetObjectType());
		if( group == 0 ) group = &defaultGroup;
		group->scriptFunctions.PushLast(func);
	}

	// Register function id as success
	return func->id;
}

// interface
int asCScriptEngine::GetStringFactoryReturnTypeId() const
{
	if( stringFactory == 0 )
		return asNO_FUNCTION;

	return GetTypeIdFromDataType(stringFactory->returnType);
}

// interface
asCModule *asCScriptEngine::GetModule(const char *_name, bool create)
{
	// Accept null as well as zero-length string
	const char *name = "";
	if( _name != 0 ) name = _name;

	if( lastModule && lastModule->name == name )
		return lastModule;

	// TODO: optimize: Improve linear search
	for( asUINT n = 0; n < scriptModules.GetLength(); ++n )
		if( scriptModules[n] && scriptModules[n]->name == name )
		{
			lastModule = scriptModules[n];
			return lastModule;
		}

	if( create )
	{
		asCModule *module = asNEW(asCModule)(name, this);
		if( module == 0 )
		{
			// Out of memory
			return 0;
		}

		scriptModules.PushLast(module);

		lastModule = module;

		return lastModule;
	}

	return 0;
}

asCModule *asCScriptEngine::GetModuleFromFuncId(int id)
{
	if( id < 0 ) return 0;
	if( id >= (int)scriptFunctions.GetLength() ) return 0;
	asCScriptFunction *func = scriptFunctions[id];
	if( func == 0 ) return 0;
	return func->module;
}

// internal
int asCScriptEngine::RequestBuild()
{
	ACQUIREEXCLUSIVE(engineRWLock);
	if( isBuilding )
	{
		RELEASEEXCLUSIVE(engineRWLock);
		return asBUILD_IN_PROGRESS;
	}
	isBuilding = true;
	RELEASEEXCLUSIVE(engineRWLock);

	return 0;
}

// internal
void asCScriptEngine::BuildCompleted()
{
	// Always free up pooled memory after a completed build
	memoryMgr.FreeUnusedMemory();

	isBuilding = false;
}

void asCScriptEngine::RemoveTemplateInstanceType(asCObjectType *t)
{
	int n;

	// Destroy the factory stubs
	for( n = 0; n < (int)t->beh.factories.GetLength(); n++ )
	{
		// Make sure the factory stub isn't referencing this object anymore
		scriptFunctions[t->beh.factories[n]]->ReleaseAllHandles(this);
		scriptFunctions[t->beh.factories[n]]->Release();
	}
	t->beh.factories.SetLength(0);

	// Destroy the stub for the list factory too
	if( t->beh.listFactory )
	{
		scriptFunctions[t->beh.listFactory]->ReleaseAllHandles(this);
		scriptFunctions[t->beh.listFactory]->Release();
		t->beh.listFactory = 0;
	}

	// Destroy the specialized functions
	for( n = 1; n < (int)t->beh.operators.GetLength(); n += 2 )
	{
		if( t->beh.operators[n] && scriptFunctions[t->beh.operators[n]]->objectType == t )
		{
			scriptFunctions[t->beh.operators[n]]->Release();
		}
	}
	t->beh.operators.SetLength(0);

	// Start searching from the end of the list, as most of
	// the time it will be the last two types
	for( n = (int)templateTypes.GetLength()-1; n >= 0; n-- )
	{
		if( templateTypes[n] == t )
		{
			if( n == (signed)templateTypes.GetLength()-1 )
				templateTypes.PopLast();
			else
				templateTypes[n] = templateTypes.PopLast();
		}
	}

	for( n = (int)templateInstanceTypes.GetLength()-1; n >= 0; n-- )
	{
		if( templateInstanceTypes[n] == t )
		{
			if( n == (signed)templateInstanceTypes.GetLength()-1 )
				templateInstanceTypes.PopLast();
			else
				templateInstanceTypes[n] = templateInstanceTypes.PopLast();
		}
	}

	asDELETE(t,asCObjectType);
}

// internal
asCObjectType *asCScriptEngine::GetTemplateInstanceType(asCObjectType *templateType, asCDataType &subType)
{
	asUINT n;

	// Is there any template instance type or template specialization already with this subtype?
	for( n = 0; n < templateTypes.GetLength(); n++ )
	{
		if( templateTypes[n] &&
			templateTypes[n]->name == templateType->name &&
			templateTypes[n]->templateSubType == subType )
			return templateTypes[n];
	}

	// No previous template instance exists

	// Make sure this template supports the subtype
	if( !templateType->acceptValueSubType && (subType.IsPrimitive() || (subType.GetObjectType()->flags & asOBJ_VALUE)) )
		return 0;

	if( !templateType->acceptRefSubType && (subType.IsObject() && (subType.GetObjectType()->flags & asOBJ_REF)) )
		return 0;

	// Create a new template instance type based on the templateType
	asCObjectType *ot = asNEW(asCObjectType)(this);
	if( ot == 0 )
	{
		// Out of memory
		return 0;
	}

	ot->templateSubType = subType;
	ot->flags     = templateType->flags;
	ot->size      = templateType->size;
	ot->name      = templateType->name;

	// Before filling in the methods, call the template instance callback behaviour to validate the type
	if( templateType->beh.templateCallback )
	{
		bool dontGarbageCollect = false;

		asCScriptFunction *callback = scriptFunctions[templateType->beh.templateCallback];
		if( !CallGlobalFunctionRetBool(ot, &dontGarbageCollect, callback->sysFuncIntf, callback) )
		{
			// If the validation is deferred then the validation will be done later,
			// so it is necessary to continue the preparation of the template instance type
			if( !deferValidationOfTemplateTypes )
			{
				// The type cannot be instanciated
				ot->templateSubType = asCDataType();
				asDELETE(ot, asCObjectType);
				return 0;
			}
		}

		// If the callback said this template instance won't be garbage collected then remove the flag
		if( dontGarbageCollect )
			ot->flags &= ~asOBJ_GC;

		ot->beh.templateCallback = templateType->beh.templateCallback;
		scriptFunctions[ot->beh.templateCallback]->AddRef();
	}

	ot->methods = templateType->methods;
	for( n = 0; n < ot->methods.GetLength(); n++ )
		scriptFunctions[ot->methods[n]]->AddRef();

	// Store the real factory in the constructor. This is used by the CreateScriptObject function.
	// Otherwise it wouldn't be necessary to store the real factory ids.
	ot->beh.construct = templateType->beh.factory;
	ot->beh.constructors = templateType->beh.factories;
	for( n = 0; n < ot->beh.constructors.GetLength(); n++ )
		scriptFunctions[ot->beh.constructors[n]]->AddRef();

	ot->beh.factory = 0;

	// Generate factory stubs for each of the factories
	for( n = 0; n < templateType->beh.factories.GetLength(); n++ )
	{
		asCScriptFunction *func = GenerateTemplateFactoryStub(templateType, ot, templateType->beh.factories[n]);

		// The function's refCount was already initialized to 1
		ot->beh.factories.PushLast(func->id);

		// Set the default factory as well
		if( templateType->beh.factories[n] == templateType->beh.factory )
			ot->beh.factory = func->id;
	}

	// Generate stub for the list factory as well
	if( templateType->beh.listFactory )
	{
		asCScriptFunction *func = GenerateTemplateFactoryStub(templateType, ot, templateType->beh.listFactory);

		// The function's refCount was already initialized to 1
		ot->beh.listFactory = func->id;
	}

	ot->beh.addref                 = templateType->beh.addref;
	if( scriptFunctions[ot->beh.addref] ) scriptFunctions[ot->beh.addref]->AddRef();
	ot->beh.release                = templateType->beh.release;
	if( scriptFunctions[ot->beh.release] ) scriptFunctions[ot->beh.release]->AddRef();
	ot->beh.copy                   = templateType->beh.copy;
	if( scriptFunctions[ot->beh.copy] ) scriptFunctions[ot->beh.copy]->AddRef();
	ot->beh.operators              = templateType->beh.operators;
	for( n = 1; n < ot->beh.operators.GetLength(); n += 2 )
	{
		scriptFunctions[ot->beh.operators[n]]->AddRef();
	}
	ot->beh.gcGetRefCount          = templateType->beh.gcGetRefCount;
	if( scriptFunctions[ot->beh.gcGetRefCount] ) scriptFunctions[ot->beh.gcGetRefCount]->AddRef();
	ot->beh.gcSetFlag              = templateType->beh.gcSetFlag;
	if( scriptFunctions[ot->beh.gcSetFlag] ) scriptFunctions[ot->beh.gcSetFlag]->AddRef();
	ot->beh.gcGetFlag              = templateType->beh.gcGetFlag;
	if( scriptFunctions[ot->beh.gcGetFlag] ) scriptFunctions[ot->beh.gcGetFlag]->AddRef();
	ot->beh.gcEnumReferences       = templateType->beh.gcEnumReferences;
	if( scriptFunctions[ot->beh.gcEnumReferences] ) scriptFunctions[ot->beh.gcEnumReferences]->AddRef();
	ot->beh.gcReleaseAllReferences = templateType->beh.gcReleaseAllReferences;
	if( scriptFunctions[ot->beh.gcReleaseAllReferences] ) scriptFunctions[ot->beh.gcReleaseAllReferences]->AddRef();

	// As the new template type is instanciated, the engine should
	// generate new functions to substitute the ones with the template subtype.
	for( n = 1; n < ot->beh.operators.GetLength(); n += 2 )
	{
		int funcId = ot->beh.operators[n];
		asCScriptFunction *func = scriptFunctions[funcId];

		if( GenerateNewTemplateFunction(templateType, ot, subType, func, &func) )
		{
			// Release the old function, the new one already has its ref count set to 1
			scriptFunctions[ot->beh.operators[n]]->Release();
			ot->beh.operators[n] = func->id;
		}
	}

	// As the new template type is instanciated, the engine should
	// generate new functions to substitute the ones with the template subtype.
	for( n = 0; n < ot->methods.GetLength(); n++ )
	{
		int funcId = ot->methods[n];
		asCScriptFunction *func = scriptFunctions[funcId];

		if( GenerateNewTemplateFunction(templateType, ot, subType, func, &func) )
		{
			// Release the old function, the new one already has its ref count set to 1
			scriptFunctions[ot->methods[n]]->Release();
			ot->methods[n] = func->id;
		}
	}

	// Increase ref counter for sub type if it is an object type
	if( ot->templateSubType.GetObjectType() ) ot->templateSubType.GetObjectType()->AddRef();

	templateTypes.PushLast(ot);

	// We need to store the object type somewhere for clean-up later
	// TODO: Why do we need both templateTypes and templateInstanceTypes? It is possible to differ between template instance and template specialization by checking for the asOBJ_TEMPLATE flag
	templateInstanceTypes.PushLast(ot);

	return ot;
}

// internal
asCScriptFunction *asCScriptEngine::GenerateTemplateFactoryStub(asCObjectType *templateType, asCObjectType *ot, int factoryId)
{
	asCScriptFunction *factory = scriptFunctions[factoryId];

	// By first instantiating the function as a dummy and then changing it to be a script function
	// I avoid having it added to the garbage collector. As it is known that this object will stay
	// alive until the template instance is no longer used there is no need to have the GC check
	// this function all the time.
	asCScriptFunction *func = asNEW(asCScriptFunction)(this, 0, asFUNC_DUMMY);
	if( func == 0 )
	{
		// Out of memory
		return 0;
	}

	func->funcType         = asFUNC_SCRIPT;
	func->name             = "factstub";
	func->id               = GetNextScriptFunctionId();
	func->returnType       = asCDataType::CreateObjectHandle(ot, false);
	func->isShared         = true;

	// Skip the first parameter as this is the object type pointer that the stub will add
	func->parameterTypes.SetLength(factory->parameterTypes.GetLength()-1);
	func->inOutFlags.SetLength(factory->inOutFlags.GetLength()-1);
	for( asUINT p = 1; p < factory->parameterTypes.GetLength(); p++ )
	{
		if( factory->parameterTypes[p].GetObjectType() == templateType->templateSubType.GetObjectType() )
		{
			func->parameterTypes[p-1] = ot->templateSubType;
			if( factory->parameterTypes[p].IsObjectHandle() )
				func->parameterTypes[p-1].MakeHandle(true);
			func->parameterTypes[p-1].MakeReference(factory->parameterTypes[p].IsReference());
			func->parameterTypes[p-1].MakeReadOnly(factory->parameterTypes[p].IsReference());
		}
		else if( factory->parameterTypes[p].GetObjectType() == templateType )
		{
			if( factory->parameterTypes[p].IsObjectHandle() )
				func->parameterTypes[p-1] = asCDataType::CreateObjectHandle(ot, false);
			else
				func->parameterTypes[p-1] = asCDataType::CreateObject(ot, false);

			func->parameterTypes[p-1].MakeReference(factory->parameterTypes[p].IsReference());
			func->parameterTypes[p-1].MakeReadOnly(factory->parameterTypes[p].IsReadOnly());
		}
		else
		{
			func->parameterTypes[p-1] = factory->parameterTypes[p];
		}
		func->inOutFlags[p-1] = factory->inOutFlags[p];
	}
	func->objVariablesOnHeap = 0;

	SetScriptFunction(func);

	// Generate the bytecode for the factory stub
	asUINT bcLength = asBCTypeSize[asBCInfo[asBC_OBJTYPE].type] +
	                  asBCTypeSize[asBCInfo[asBC_CALLSYS].type] +
	                  asBCTypeSize[asBCInfo[asBC_RET].type];

	if( ep.includeJitInstructions )
		bcLength += asBCTypeSize[asBCInfo[asBC_JitEntry].type];

	func->byteCode.SetLength(bcLength);
	asDWORD *bc = func->byteCode.AddressOf();

	if( ep.includeJitInstructions )
	{
		*(asBYTE*)bc = asBC_JitEntry;
		*(asPWORD*)(bc+1) = 0;
		bc += asBCTypeSize[asBCInfo[asBC_JitEntry].type];
	}

	*(asBYTE*)bc = asBC_OBJTYPE;
	*(asPWORD*)(bc+1) = (asPWORD)ot;
	bc += asBCTypeSize[asBCInfo[asBC_OBJTYPE].type];
	*(asBYTE*)bc = asBC_CALLSYS;
	*(asDWORD*)(bc+1) = factoryId;
	bc += asBCTypeSize[asBCInfo[asBC_CALLSYS].type];
	*(asBYTE*)bc = asBC_RET;
	*(((asWORD*)bc)+1) = (asWORD)func->GetSpaceNeededForArguments();

	func->AddReferences();
	func->stackNeeded = AS_PTR_SIZE;

	// Tell the virtual machine not to clean up the object on exception
	func->dontCleanUpOnException = true;

	func->JITCompile();

	return func;
}

bool asCScriptEngine::GenerateNewTemplateFunction(asCObjectType *templateType, asCObjectType *ot, asCDataType &subType, asCScriptFunction *func, asCScriptFunction **newFunc)
{
	bool needNewFunc = false;
	if( func->returnType.GetObjectType() == templateType->templateSubType.GetObjectType() ||
		func->returnType.GetObjectType() == templateType )
		needNewFunc = true;
	else
	{
		for( asUINT p = 0; p < func->parameterTypes.GetLength(); p++ )
		{
			if( func->parameterTypes[p].GetObjectType() == templateType->templateSubType.GetObjectType() ||
				func->parameterTypes[p].GetObjectType() == templateType )
			{
				needNewFunc = true;
				break;
			}
		}
	}

	if( needNewFunc )
	{
		asCScriptFunction *func2 = asNEW(asCScriptFunction)(this, 0, func->funcType);
		if( func2 == 0 )
		{
			// Out of memory
			return false;
		}

		func2->name     = func->name;
		func2->id       = GetNextScriptFunctionId();

		if( func->returnType.GetObjectType() == templateType->templateSubType.GetObjectType() )
		{
			func2->returnType = subType;
			if( func->returnType.IsObjectHandle() && !subType.IsObjectHandle() )
			{
				// The return type is a handle to the subtype
				func2->returnType.MakeHandle(true, true);
				func2->returnType.MakeReference(func->returnType.IsReference());
				func2->returnType.MakeReadOnly(func->returnType.IsReadOnly());
			}
			else
			{
				// The return type is the subtype directly
				func2->returnType.MakeReference(func->returnType.IsReference());
				func2->returnType.MakeReadOnly(subType.IsReadOnly() || func->returnType.IsReadOnly());
			}
		}
		else if( func->returnType.GetObjectType() == templateType )
		{
			if( func2->returnType.IsObjectHandle() )
				func2->returnType = asCDataType::CreateObjectHandle(ot, false);
			else
				func2->returnType = asCDataType::CreateObject(ot, false);

			func2->returnType.MakeReference(func->returnType.IsReference());
			func2->returnType.MakeReadOnly(func->returnType.IsReadOnly());
		}
		else
			func2->returnType = func->returnType;

		func2->parameterTypes.SetLength(func->parameterTypes.GetLength());
		for( asUINT p = 0; p < func->parameterTypes.GetLength(); p++ )
		{
			if( func->parameterTypes[p].GetObjectType() == templateType->templateSubType.GetObjectType() )
			{
				func2->parameterTypes[p] = subType;
				if( func->parameterTypes[p].IsObjectHandle() )
				{
					// The parameter is a handle to the subtype
					func2->parameterTypes[p].MakeHandle(true);
					func2->parameterTypes[p].MakeReference(func->parameterTypes[p].IsReference());
					func2->parameterTypes[p].MakeReadOnly(func->parameterTypes[p].IsReadOnly());
				}
				else
				{
					// The parameter is the subtype directly
					func2->parameterTypes[p].MakeReference(func->parameterTypes[p].IsReference());
					func2->parameterTypes[p].MakeReadOnly(subType.IsReadOnly() || func->parameterTypes[p].IsReference());
				}
			}
			else if( func->parameterTypes[p].GetObjectType() == templateType )
			{
				if( func->parameterTypes[p].IsObjectHandle() )
					func2->parameterTypes[p] = asCDataType::CreateObjectHandle(ot, false);
				else
					func2->parameterTypes[p] = asCDataType::CreateObject(ot, false);

				func2->parameterTypes[p].MakeReference(func->parameterTypes[p].IsReference());
				func2->parameterTypes[p].MakeReadOnly(func->parameterTypes[p].IsReadOnly());
			}
			else
				func2->parameterTypes[p] = func->parameterTypes[p];
		}

		// TODO: template: Must be careful when instanciating templates for garbage collected types
		//                 If the template hasn't been registered with the behaviours, it shouldn't
		//                 permit instanciation of garbage collected types that in turn may refer to
		//                 this instance.

		func2->inOutFlags = func->inOutFlags;
		func2->isReadOnly = func->isReadOnly;
		func2->objectType = ot;
		func2->stackNeeded = func->stackNeeded;
		func2->sysFuncIntf = asNEW(asSSystemFunctionInterface)(*func->sysFuncIntf);

		SetScriptFunction(func2);

		// Return the new function
		*newFunc = func2;
	}

	return needNewFunc;
}

void asCScriptEngine::CallObjectMethod(void *obj, int func)
{
	asCScriptFunction *s = scriptFunctions[func];
	asASSERT( s != 0 );
	CallObjectMethod(obj, s->sysFuncIntf, s);
}

void asCScriptEngine::CallObjectMethod(void *obj, asSSystemFunctionInterface *i, asCScriptFunction *s)
{
#ifdef __GNUC__
	if( i->callConv == ICC_GENERIC_METHOD )
	{
		asCGeneric gen(this, s, obj, 0);
		void (*f)(asIScriptGeneric *) = (void (*)(asIScriptGeneric *))(i->func);
		f(&gen);
	}
	else if( i->callConv == ICC_THISCALL || i->callConv == ICC_VIRTUAL_THISCALL )
	{
		// For virtual thiscalls we must call the method as a true class method
		// so that the compiler will lookup the function address in the vftable
		union
		{
			asSIMPLEMETHOD_t mthd;
			struct
			{
				asFUNCTION_t func;
				asPWORD baseOffset;  // Same size as the pointer
			} f;
		} p;
		p.f.func = (void (*)())(i->func);
		p.f.baseOffset = asPWORD(i->baseOffset);
		void (asCSimpleDummy::*f)() = p.mthd;
		(((asCSimpleDummy*)obj)->*f)();
	}
	else /*if( i->callConv == ICC_CDECL_OBJLAST || i->callConv == ICC_CDECL_OBJFIRST )*/
	{
		void (*f)(void *) = (void (*)(void *))(i->func);
		f(obj);
	}
#else
#ifndef AS_NO_CLASS_METHODS
	if( i->callConv == ICC_THISCALL )
	{
		union
		{
			asSIMPLEMETHOD_t mthd;
			asFUNCTION_t func;
		} p;
		p.func = (void (*)())(i->func);
		void (asCSimpleDummy::*f)() = p.mthd;
		obj = (void*)(asPWORD(obj) + i->baseOffset);
		(((asCSimpleDummy*)obj)->*f)();
	}
	else
#endif
	if( i->callConv == ICC_GENERIC_METHOD )
	{
		asCGeneric gen(this, s, obj, 0);
		void (*f)(asIScriptGeneric *) = (void (*)(asIScriptGeneric *))(i->func);
		f(&gen);
	}
	else /*if( i->callConv == ICC_CDECL_OBJLAST || i->callConv == ICC_CDECL_OBJFIRST )*/
	{
		void (*f)(void *) = (void (*)(void *))(i->func);
		f(obj);
	}
#endif
}

bool asCScriptEngine::CallObjectMethodRetBool(void *obj, int func)
{
	asCScriptFunction *s = scriptFunctions[func];
	asASSERT( s != 0 );
	asSSystemFunctionInterface *i = s->sysFuncIntf;

#ifdef __GNUC__
	if( i->callConv == ICC_GENERIC_METHOD )
	{
		asCGeneric gen(this, s, obj, 0);
		void (*f)(asIScriptGeneric *) = (void (*)(asIScriptGeneric *))(i->func);
		f(&gen);
		return *(bool*)gen.GetReturnPointer();
	}
	else if( i->callConv == ICC_THISCALL || i->callConv == ICC_VIRTUAL_THISCALL )
	{
		// For virtual thiscalls we must call the method as a true class method so that the compiler will lookup the function address in the vftable
		union
		{
			asSIMPLEMETHOD_t mthd;
			struct
			{
				asFUNCTION_t func;
				asPWORD baseOffset;
			} f;
		} p;
		p.f.func = (void (*)())(i->func);
		p.f.baseOffset = asPWORD(i->baseOffset);
		bool (asCSimpleDummy::*f)() = (bool (asCSimpleDummy::*)())(p.mthd);
		return (((asCSimpleDummy*)obj)->*f)();
	}
	else /*if( i->callConv == ICC_CDECL_OBJLAST || i->callConv == ICC_CDECL_OBJFIRST )*/
	{
		bool (*f)(void *) = (bool (*)(void *))(i->func);
		return f(obj);
	}
#else
#ifndef AS_NO_CLASS_METHODS
	if( i->callConv == ICC_THISCALL )
	{
		union
		{
			asSIMPLEMETHOD_t mthd;
			asFUNCTION_t func;
		} p;
		p.func = (void (*)())(i->func);
		bool (asCSimpleDummy::*f)() = (bool (asCSimpleDummy::*)())p.mthd;
		obj = (void*)(asPWORD(obj) + i->baseOffset);
		return (((asCSimpleDummy*)obj)->*f)();
	}
	else
#endif
	if( i->callConv == ICC_GENERIC_METHOD )
	{
		asCGeneric gen(this, s, obj, 0);
		void (*f)(asIScriptGeneric *) = (void (*)(asIScriptGeneric *))(i->func);
		f(&gen);
		return *(bool*)gen.GetReturnPointer();
	}
	else /*if( i->callConv == ICC_CDECL_OBJLAST || i->callConv == ICC_CDECL_OBJFIRST )*/
	{
		bool (*f)(void *) = (bool (*)(void *))(i->func);
		return f(obj);
	}
#endif
}

int asCScriptEngine::CallObjectMethodRetInt(void *obj, int func)
{
	asCScriptFunction *s = scriptFunctions[func];
	asASSERT( s != 0 );
	asSSystemFunctionInterface *i = s->sysFuncIntf;

#ifdef __GNUC__
	if( i->callConv == ICC_GENERIC_METHOD )
	{
		asCGeneric gen(this, s, obj, 0);
		void (*f)(asIScriptGeneric *) = (void (*)(asIScriptGeneric *))(i->func);
		f(&gen);
		return *(int*)gen.GetReturnPointer();
	}
	else if( i->callConv == ICC_THISCALL || i->callConv == ICC_VIRTUAL_THISCALL )
	{
		// For virtual thiscalls we must call the method as a true class method so that the compiler will lookup the function address in the vftable
		union
		{
			asSIMPLEMETHOD_t mthd;
			struct
			{
				asFUNCTION_t func;
				asPWORD baseOffset;
			} f;
		} p;
		p.f.func = (void (*)())(i->func);
		p.f.baseOffset = asPWORD(i->baseOffset);
		int (asCSimpleDummy::*f)() = (int (asCSimpleDummy::*)())(p.mthd);
		return (((asCSimpleDummy*)obj)->*f)();
	}
	else /*if( i->callConv == ICC_CDECL_OBJLAST || i->callConv == ICC_CDECL_OBJFIRST )*/
	{
		int (*f)(void *) = (int (*)(void *))(i->func);
		return f(obj);
	}
#else
#ifndef AS_NO_CLASS_METHODS
	if( i->callConv == ICC_THISCALL )
	{
		union
		{
			asSIMPLEMETHOD_t mthd;
			asFUNCTION_t func;
		} p;
		p.func = (void (*)())(i->func);
		int (asCSimpleDummy::*f)() = (int (asCSimpleDummy::*)())p.mthd;
		obj = (void*)(asPWORD(obj) + i->baseOffset);
		return (((asCSimpleDummy*)obj)->*f)();
	}
	else
#endif
	if( i->callConv == ICC_GENERIC_METHOD )
	{
		asCGeneric gen(this, s, obj, 0);
		void (*f)(asIScriptGeneric *) = (void (*)(asIScriptGeneric *))(i->func);
		f(&gen);
		return *(int*)gen.GetReturnPointer();
	}
	else /*if( i->callConv == ICC_CDECL_OBJLAST || i->callConv == ICC_CDECL_OBJFIRST )*/
	{
		int (*f)(void *) = (int (*)(void *))(i->func);
		return f(obj);
	}
#endif
}

void *asCScriptEngine::CallGlobalFunctionRetPtr(int func)
{
	asCScriptFunction *s = scriptFunctions[func];
	asASSERT( s != 0 );
	return CallGlobalFunctionRetPtr(s->sysFuncIntf, s);
}

void *asCScriptEngine::CallGlobalFunctionRetPtr(int func, void *param1)
{
	asCScriptFunction *s = scriptFunctions[func];
	asASSERT( s != 0 );
	return CallGlobalFunctionRetPtr(s->sysFuncIntf, s, param1);
}

void *asCScriptEngine::CallGlobalFunctionRetPtr(asSSystemFunctionInterface *i, asCScriptFunction *s)
{
	if( i->callConv == ICC_CDECL )
	{
		void *(*f)() = (void *(*)())(i->func);
		return f();
	}
	else if( i->callConv == ICC_STDCALL )
	{
		void *(STDCALL *f)() = (void *(STDCALL *)())(i->func);
		return f();
	}
	else
	{
		asCGeneric gen(this, s, 0, 0);
		void (*f)(asIScriptGeneric *) = (void (*)(asIScriptGeneric *))(i->func);
		f(&gen);
		return *(void**)gen.GetReturnPointer();
	}
}

void *asCScriptEngine::CallGlobalFunctionRetPtr(asSSystemFunctionInterface *i, asCScriptFunction *s, void *param1)
{
	if( i->callConv == ICC_CDECL )
	{
		void *(*f)(void *) = (void *(*)(void *))(i->func);
		return f(param1);
	}
	else if( i->callConv == ICC_STDCALL )
	{
		void *(STDCALL *f)(void *) = (void *(STDCALL *)(void *))(i->func);
		return f(param1);
	}
	else
	{
		asCGeneric gen(this, s, 0, (asDWORD*)&param1);
		void (*f)(asIScriptGeneric *) = (void (*)(asIScriptGeneric *))(i->func);
		f(&gen);
		return *(void**)gen.GetReturnPointer();
	}
}

void asCScriptEngine::CallObjectMethod(void *obj, void *param, int func)
{
	asCScriptFunction *s = scriptFunctions[func];
	asASSERT( s != 0 );
	CallObjectMethod(obj, param, s->sysFuncIntf, s);
}

void asCScriptEngine::CallObjectMethod(void *obj, void *param, asSSystemFunctionInterface *i, asCScriptFunction *s)
{
#ifdef __GNUC__
	if( i->callConv == ICC_CDECL_OBJLAST )
	{
		void (*f)(void *, void *) = (void (*)(void *, void *))(i->func);
		f(param, obj);
	}
	else if( i->callConv == ICC_GENERIC_METHOD )
	{
		asCGeneric gen(this, s, obj, (asDWORD*)&param);
		void (*f)(asIScriptGeneric *) = (void (*)(asIScriptGeneric *))(i->func);
		f(&gen);
	}
	else if( i->callConv == ICC_VIRTUAL_THISCALL || i->callConv == ICC_THISCALL )
	{
		// For virtual thiscalls we must call the method as a true class method
		// so that the compiler will lookup the function address in the vftable
		union
		{
			asSIMPLEMETHOD_t mthd;
			struct
			{
				asFUNCTION_t func;
				asPWORD baseOffset;  // Same size as the pointer
			} f;
		} p;
		p.f.func = (void (*)())(i->func);
		p.f.baseOffset = asPWORD(i->baseOffset);
		void (asCSimpleDummy::*f)(void*) = (void (asCSimpleDummy::*)(void*))(p.mthd);
		(((asCSimpleDummy*)obj)->*f)(param);
	}
	else /*if( i->callConv == ICC_CDECL_OBJFIRST */
	{
		void (*f)(void *, void *) = (void (*)(void *, void *))(i->func);
		f(obj, param);
	}
#else
#ifndef AS_NO_CLASS_METHODS
	if( i->callConv == ICC_THISCALL )
	{
		union
		{
			asSIMPLEMETHOD_t mthd;
			asFUNCTION_t func;
		} p;
		p.func = (void (*)())(i->func);
		void (asCSimpleDummy::*f)(void *) = (void (asCSimpleDummy::*)(void *))(p.mthd);
		obj = (void*)(asPWORD(obj) + i->baseOffset);
		(((asCSimpleDummy*)obj)->*f)(param);
	}
	else
#endif
	if( i->callConv == ICC_CDECL_OBJLAST )
	{
		void (*f)(void *, void *) = (void (*)(void *, void *))(i->func);
		f(param, obj);
	}
	else if( i->callConv == ICC_GENERIC_METHOD )
	{
		asCGeneric gen(this, s, obj, (asDWORD*)&param);
		void (*f)(asIScriptGeneric *) = (void (*)(asIScriptGeneric *))(i->func);
		f(&gen);
	}
	else /*if( i->callConv == ICC_CDECL_OBJFIRST )*/
	{
		void (*f)(void *, void *) = (void (*)(void *, void *))(i->func);
		f(obj, param);
	}
#endif
}

void asCScriptEngine::CallGlobalFunction(void *param1, void *param2, asSSystemFunctionInterface *i, asCScriptFunction *s)
{
	if( i->callConv == ICC_CDECL )
	{
		void (*f)(void *, void *) = (void (*)(void *, void *))(i->func);
		f(param1, param2);
	}
	else if( i->callConv == ICC_STDCALL )
	{
		void (STDCALL *f)(void *, void *) = (void (STDCALL *)(void *, void *))(i->func);
		f(param1, param2);
	}
	else
	{
		// We must guarantee the order of the arguments which is why we copy them to this
		// array. Otherwise the compiler may put them anywhere it likes, or even keep them 
		// in the registers which causes problem.
		void *params[2] = {param1, param2};

		asCGeneric gen(this, s, 0, (asDWORD*)&params);
		void (*f)(asIScriptGeneric *) = (void (*)(asIScriptGeneric *))(i->func);
		f(&gen);
	}
}

bool asCScriptEngine::CallGlobalFunctionRetBool(void *param1, void *param2, asSSystemFunctionInterface *i, asCScriptFunction *s)
{
	if( i->callConv == ICC_CDECL )
	{
		bool (*f)(void *, void *) = (bool (*)(void *, void *))(i->func);
		return f(param1, param2);
	}
	else if( i->callConv == ICC_STDCALL )
	{
		bool (STDCALL *f)(void *, void *) = (bool (STDCALL *)(void *, void *))(i->func);
		return f(param1, param2);
	}
	else
	{
		// TODO: When simulating a 64bit environment by defining AS_64BIT_PTR on a 32bit platform this code
		//       fails, because the stack given to asCGeneric is not prepared with two 64bit arguments.

		// We must guarantee the order of the arguments which is why we copy them to this
		// array. Otherwise the compiler may put them anywhere it likes, or even keep them 
		// in the registers which causes problem.
		void *params[2] = {param1, param2};
		asCGeneric gen(this, s, 0, (asDWORD*)params);
		void (*f)(asIScriptGeneric *) = (void (*)(asIScriptGeneric *))(i->func);
		f(&gen);
		return *(bool*)gen.GetReturnPointer();
	}
}

void *asCScriptEngine::CallAlloc(asCObjectType *type) const
{
    // Allocate 4 bytes as the smallest size. Otherwise CallSystemFunction may try to
    // copy a DWORD onto a smaller memory block, in case the object type is return in registers.
#if defined(AS_DEBUG)
	return ((asALLOCFUNCDEBUG_t)(userAlloc))(type->size < 4 ? 4 : type->size, __FILE__, __LINE__);
#else
	return userAlloc(type->size < 4 ? 4 : type->size);
#endif
}

void asCScriptEngine::CallFree(void *obj) const
{
	userFree(obj);
}

// interface
void asCScriptEngine::NotifyGarbageCollectorOfNewObject(void *obj, asIObjectType *type)
{
	gc.AddScriptObjectToGC(obj, static_cast<asCObjectType*>(type));
}

// interface
int asCScriptEngine::GarbageCollect(asDWORD flags)
{
	return gc.GarbageCollect(flags);
}

// interface
void asCScriptEngine::GetGCStatistics(asUINT *currentSize, asUINT *totalDestroyed, asUINT *totalDetected, asUINT *newObjects, asUINT *totalNewDestroyed) const
{
	gc.GetStatistics(currentSize, totalDestroyed, totalDetected, newObjects, totalNewDestroyed);
}

// interface
void asCScriptEngine::GCEnumCallback(void *reference)
{
	gc.GCEnumCallback(reference);
}


// TODO: multithread: The mapTypeIdToDataType must be protected with critical sections in all functions that access it
int asCScriptEngine::GetTypeIdFromDataType(const asCDataType &dtIn) const
{
	if( dtIn.IsNullHandle() ) return 0;

	// Register the base form
	asCDataType dt(dtIn);
	if( dt.GetObjectType() )
		dt.MakeHandle(false);

	// Find the existing type id
	asSMapNode<int,asCDataType*> *cursor = 0;
	mapTypeIdToDataType.MoveFirst(&cursor);
	while( cursor )
	{
		if( mapTypeIdToDataType.GetValue(cursor)->IsEqualExceptRefAndConst(dt) )
		{
			int typeId = mapTypeIdToDataType.GetKey(cursor);
			if( dtIn.GetObjectType() && !(dtIn.GetObjectType()->flags & asOBJ_ASHANDLE) )
			{
				// The the ASHANDLE types behave like handles, but are really
				// value types so the typeId is never returned as a handle
				if( dtIn.IsObjectHandle() )
					typeId |= asTYPEID_OBJHANDLE;
				if( dtIn.IsHandleToConst() )
					typeId |= asTYPEID_HANDLETOCONST;
			}

			return typeId;
		}

		mapTypeIdToDataType.MoveNext(&cursor, cursor);
	}

	// The type id doesn't exist, create it

	// Setup the basic type id
	int typeId = typeIdSeqNbr++;
	if( dt.GetObjectType() )
	{
		if( dt.GetObjectType()->flags & asOBJ_SCRIPT_OBJECT ) typeId |= asTYPEID_SCRIPTOBJECT;
		else if( dt.GetObjectType()->flags & asOBJ_TEMPLATE ) typeId |= asTYPEID_TEMPLATE;
		else if( dt.GetObjectType()->flags & asOBJ_ENUM ) {} // TODO: Should we have a specific bit for this?
		else typeId |= asTYPEID_APPOBJECT;
	}

	// Insert the basic object type
	asCDataType *newDt = asNEW(asCDataType)(dt);
	if( newDt == 0 )
	{
		// Out of memory
		return 0;
	}

	newDt->MakeReference(false);
	newDt->MakeReadOnly(false);
	newDt->MakeHandle(false);

	mapTypeIdToDataType.Insert(typeId, newDt);

	// Call recursively to get the correct typeId
	return GetTypeIdFromDataType(dtIn);
}

asCDataType asCScriptEngine::GetDataTypeFromTypeId(int typeId) const
{
	int baseId = typeId & (asTYPEID_MASK_OBJECT | asTYPEID_MASK_SEQNBR);

	asSMapNode<int,asCDataType*> *cursor = 0;
	if( mapTypeIdToDataType.MoveTo(&cursor, baseId) )
	{
		asCDataType dt(*mapTypeIdToDataType.GetValue(cursor));
		if( typeId & asTYPEID_OBJHANDLE )
			dt.MakeHandle(true);
		if( typeId & asTYPEID_HANDLETOCONST )
			dt.MakeHandleToConst(true);
		return dt;
	}

	return asCDataType();
}

asCObjectType *asCScriptEngine::GetObjectTypeFromTypeId(int typeId) const
{
	asCDataType dt = GetDataTypeFromTypeId(typeId);
	return dt.GetObjectType();
}

void asCScriptEngine::RemoveFromTypeIdMap(asCObjectType *type)
{
	asSMapNode<int,asCDataType*> *cursor = 0;
	mapTypeIdToDataType.MoveFirst(&cursor);
	while( cursor )
	{
		asCDataType *dt = mapTypeIdToDataType.GetValue(cursor);
		asSMapNode<int,asCDataType*> *old = cursor;
		mapTypeIdToDataType.MoveNext(&cursor, cursor);
		if( dt->GetObjectType() == type )
		{
			asDELETE(dt,asCDataType);
			mapTypeIdToDataType.Erase(old);
		}
	}
}

// interface
int asCScriptEngine::GetTypeIdByDecl(const char *decl) const
{
	asCDataType dt;
	// This cast is ok, because we are not changing anything in the engine
	asCBuilder bld(const_cast<asCScriptEngine*>(this), 0);
	int r = bld.ParseDataType(decl, &dt, defaultNamespace);
	if( r < 0 )
		return asINVALID_TYPE;

	return GetTypeIdFromDataType(dt);
}

// interface
const char *asCScriptEngine::GetTypeDeclaration(int typeId, bool includeNamespace) const
{
	asCDataType dt = GetDataTypeFromTypeId(typeId);

	asCString *tempString = &asCThreadManager::GetLocalData()->string;
	*tempString = dt.Format(includeNamespace);

	return tempString->AddressOf();
}

// TODO: interface: Deprecate. This function is not necessary now that all primitive types have fixed typeIds
int asCScriptEngine::GetSizeOfPrimitiveType(int typeId) const
{
	asCDataType dt = GetDataTypeFromTypeId(typeId);
	if( !dt.IsPrimitive() ) return 0;

	return dt.GetSizeInMemoryBytes();
}

// TODO: interface: Should deprecate this. The application should be calling the factory directly
void *asCScriptEngine::CreateScriptObject(int typeId)
{
	// Make sure the type id is for an object type, and not a primitive or a handle
	if( (typeId & (asTYPEID_MASK_OBJECT | asTYPEID_MASK_SEQNBR)) != typeId ) return 0;
	if( (typeId & asTYPEID_MASK_OBJECT) == 0 ) return 0;

	asCDataType dt = GetDataTypeFromTypeId(typeId);

	// Is the type id valid?
	if( !dt.IsValid() ) return 0;

	asCObjectType *objType = dt.GetObjectType();
	void *ptr = 0;

	// Construct the object
	if( objType->flags & asOBJ_SCRIPT_OBJECT )
	{
		// Call the script class' default factory with a context
		ptr = ScriptObjectFactory(objType, this);
	}
	else if( objType->flags & asOBJ_TEMPLATE )
	{
		// The registered factory that takes the object type is moved
		// to the construct behaviour when the type is instanciated
		ptr = CallGlobalFunctionRetPtr(objType->beh.construct, objType);
	}
	else if( objType->flags & asOBJ_REF )
	{
		// Call the default factory directly
		ptr = CallGlobalFunctionRetPtr(objType->beh.factory);
	}
	else
	{
		// Manually allocate the memory, then
		// call the default constructor
		ptr = CallAlloc(objType);
		int funcIndex = objType->beh.construct;
		if( funcIndex )
			CallObjectMethod(ptr, funcIndex);
	}

	return ptr;
}

// interface
void *asCScriptEngine::CreateUninitializedScriptObject(int typeId)
{
	// Make sure the type id is for an object type, and not a primitive or a handle.
	// This function only works for script classes. Registered types cannot be created this way.
	if( (typeId & (asTYPEID_MASK_OBJECT | asTYPEID_MASK_SEQNBR)) != typeId ) return 0;
	if( (typeId & asTYPEID_MASK_OBJECT) == 0 ) return 0;
	if( (typeId & asTYPEID_SCRIPTOBJECT) == 0 ) return 0;

	asCDataType dt = GetDataTypeFromTypeId(typeId);

	// Is the type id valid?
	if( !dt.IsValid() ) return 0;

	asCObjectType *objType = dt.GetObjectType();

	asASSERT( objType->flags & asOBJ_SCRIPT_OBJECT );

	// Construct the object, but do not call the actual constructor that initializes the members
	// The initialization will be done by the application afterwards, e.g. through serialization.
	asCScriptObject *obj = reinterpret_cast<asCScriptObject*>(CallAlloc(objType));

	// Pre-initialize the memory so there are no invalid pointers
	ScriptObject_ConstructUnitialized(objType, obj);

	return obj;
}

// TODO: interface: Should deprecate this. The application should be calling the factory directly
void *asCScriptEngine::CreateScriptObjectCopy(void *origObj, int typeId)
{
	void *newObj = CreateScriptObject(typeId);
	if( newObj == 0 ) return 0;

	AssignScriptObject(newObj, origObj, typeId);

	return newObj;
}

// internal
void asCScriptEngine::ConstructScriptObjectCopy(void *mem, void *obj, asCObjectType *type)
{
	// This function is only meant to be used for value types
	asASSERT( type->flags & asOBJ_VALUE );

	// TODO: runtime optimize: Should use the copy constructor when available
	int funcIndex = type->beh.construct;
	if( funcIndex )
		CallObjectMethod(mem, funcIndex);

	AssignScriptObject(mem, obj, type->GetTypeId());
}

#ifdef AS_DEPRECATED
// Deprecated since 2.24.0 - 2012-06-07
void asCScriptEngine::CopyScriptObject(void *dstObj, void *srcObj, int typeId)
{
	AssignScriptObject(dstObj, srcObj, typeId);
}
#endif

// interface
void asCScriptEngine::AssignScriptObject(void *dstObj, void *srcObj, int typeId)
{
	// Make sure the type id is for an object type, and not a primitive or a handle
	if( (typeId & (asTYPEID_MASK_OBJECT | asTYPEID_MASK_SEQNBR)) != typeId ) return;
	if( (typeId & asTYPEID_MASK_OBJECT) == 0 ) return;

	// Copy the contents from the original object, using the assignment operator
	asCDataType dt = GetDataTypeFromTypeId(typeId);

	// Is the type id valid?
	if( !dt.IsValid() ) return;

	asCObjectType *objType = dt.GetObjectType();

	// Must not copy if the opAssign is not available and the object is not a POD object
	if( objType->beh.copy )
	{
		CallObjectMethod(dstObj, srcObj, objType->beh.copy);
	}
	else if( objType->size && (objType->flags & asOBJ_POD) )
	{
		memcpy(dstObj, srcObj, objType->size);
	}
}

// interface
void asCScriptEngine::AddRefScriptObject(void *obj, int typeId)
{
	// Make sure it is not a null pointer
	if( obj == 0 ) return;

	// Make sure the type id is for an object type or a handle
	if( (typeId & asTYPEID_MASK_OBJECT) == 0 ) return;

	asCDataType dt = GetDataTypeFromTypeId(typeId);

	// Is the type id valid?
	if( !dt.IsValid() ) return;

	asCObjectType *objType = dt.GetObjectType();

	if( objType->beh.addref )
	{
		// Call the addref behaviour
		CallObjectMethod(obj, objType->beh.addref);
	}
}

// interface
void asCScriptEngine::AddRefScriptObject(void *obj, const asIObjectType *type)
{
	// Make sure it is not a null pointer
	if( obj == 0 ) return;

	const asCObjectType *objType = static_cast<const asCObjectType *>(type);
	if( objType->beh.addref )
	{
		// Call the addref behaviour
		CallObjectMethod(obj, objType->beh.addref);
	}
}

// interface
void asCScriptEngine::ReleaseScriptObject(void *obj, int typeId)
{
	// Make sure it is not a null pointer
	if( obj == 0 ) return;

	// Make sure the type id is for an object type or a handle
	if( (typeId & asTYPEID_MASK_OBJECT) == 0 ) return;

	asCDataType dt = GetDataTypeFromTypeId(typeId);

	// Is the type id valid?
	if( !dt.IsValid() ) return;

	asCObjectType *objType = dt.GetObjectType();

	ReleaseScriptObject(obj, objType);
}

// interface
void asCScriptEngine::ReleaseScriptObject(void *obj, const asIObjectType *type)
{
	// Make sure it is not a null pointer
	if( obj == 0 ) return;

	const asCObjectType *objType = static_cast<const asCObjectType *>(type);
	if( objType->flags & asOBJ_REF )
	{
		asASSERT( (objType->flags & asOBJ_NOCOUNT) || objType->beh.release );
		if( objType->beh.release )
		{
			// Call the release behaviour
			CallObjectMethod(obj, objType->beh.release);
		}
	}
	else
	{
		// TODO: interface: shouldn't work on non reference types

		// Call the destructor
		if( objType->beh.destruct )
			CallObjectMethod(obj, objType->beh.destruct);

		// Then free the memory
		CallFree(obj);
	}
}

// interface
bool asCScriptEngine::IsHandleCompatibleWithObject(void *obj, int objTypeId, int handleTypeId) const
{
	// if equal, then it is obvious they are compatible
	if( objTypeId == handleTypeId )
		return true;

	// Get the actual data types from the type ids
	asCDataType objDt = GetDataTypeFromTypeId(objTypeId);
	asCDataType hdlDt = GetDataTypeFromTypeId(handleTypeId);

	// A handle to const cannot be passed to a handle that is not referencing a const object
	if( objDt.IsHandleToConst() && !hdlDt.IsHandleToConst() )
		return false;

	if( objDt.GetObjectType() == hdlDt.GetObjectType() )
	{
		// The object type is equal
		return true;
	}
	else if( objDt.IsScriptObject() && obj )
	{
		// Get the true type from the object instance
		asCObjectType *objType = ((asCScriptObject*)obj)->objType;

		// Check if the object implements the interface, or derives from the base class
		// This will also return true, if the requested handle type is an exact match for the object type
		if( objType->Implements(hdlDt.GetObjectType()) ||
		    objType->DerivesFrom(hdlDt.GetObjectType()) )
			return true;
	}

	return false;
}

// interface
int asCScriptEngine::BeginConfigGroup(const char *groupName)
{
	// Make sure the group name doesn't already exist
	for( asUINT n = 0; n < configGroups.GetLength(); n++ )
	{
		if( configGroups[n]->groupName == groupName )
			return asNAME_TAKEN;
	}

	if( currentGroup != &defaultGroup )
		return asNOT_SUPPORTED;

	asCConfigGroup *group = asNEW(asCConfigGroup)();
	if( group == 0 )
		return asOUT_OF_MEMORY;

	group->groupName = groupName;

	configGroups.PushLast(group);
	currentGroup = group;

	return 0;
}

// interface
int asCScriptEngine::EndConfigGroup()
{
	// Raise error if trying to end the default config
	if( currentGroup == &defaultGroup )
		return asERROR;

	currentGroup = &defaultGroup;

	return 0;
}

// interface
int asCScriptEngine::RemoveConfigGroup(const char *groupName)
{
	// It is not allowed to remove a group that is still in use.

	// It would be possible to change the code in such a way that
	// the group could be removed even though it was still in use,
	// but that would cause severe negative impact on runtime
	// performance, since the VM would then have to be able handle
	// situations where the types, functions, and global variables
	// can be removed at any time.

	for( asUINT n = 0; n < configGroups.GetLength(); n++ )
	{
		if( configGroups[n]->groupName == groupName )
		{
			asCConfigGroup *group = configGroups[n];

			// Make sure the group isn't referenced by anyone
			if( group->refCount > 0 )
				return asCONFIG_GROUP_IS_IN_USE;

			// Verify if any objects registered in this group is still alive
			if( group->HasLiveObjects() )
				return asCONFIG_GROUP_IS_IN_USE;

			// Remove the group from the list
			if( n == configGroups.GetLength() - 1 )
				configGroups.PopLast();
			else
				configGroups[n] = configGroups.PopLast();

			// Remove the configurations registered with this group
			group->RemoveConfiguration(this);

			asDELETE(group,asCConfigGroup);
		}
	}

	return 0;
}

asCConfigGroup *asCScriptEngine::FindConfigGroupForFunction(int funcId) const
{
	for( asUINT n = 0; n < configGroups.GetLength(); n++ )
	{
		// Check global functions
		asUINT m;
		for( m = 0; m < configGroups[n]->scriptFunctions.GetLength(); m++ )
		{
			if( configGroups[n]->scriptFunctions[m]->id == funcId )
				return configGroups[n];
		}
	}

	return 0;
}


asCConfigGroup *asCScriptEngine::FindConfigGroupForGlobalVar(int gvarId) const
{
	for( asUINT n = 0; n < configGroups.GetLength(); n++ )
	{
		for( asUINT m = 0; m < configGroups[n]->globalProps.GetLength(); m++ )
		{
			if( int(configGroups[n]->globalProps[m]->id) == gvarId )
				return configGroups[n];
		}
	}

	return 0;
}

asCConfigGroup *asCScriptEngine::FindConfigGroupForObjectType(const asCObjectType *objType) const
{
	for( asUINT n = 0; n < configGroups.GetLength(); n++ )
	{
		for( asUINT m = 0; m < configGroups[n]->objTypes.GetLength(); m++ )
		{
			if( configGroups[n]->objTypes[m] == objType )
				return configGroups[n];
		}
	}

	return 0;
}

asCConfigGroup *asCScriptEngine::FindConfigGroupForFuncDef(const asCScriptFunction *funcDef) const
{
	for( asUINT n = 0; n < configGroups.GetLength(); n++ )
	{
		asCScriptFunction *f = const_cast<asCScriptFunction*>(funcDef);
		if( configGroups[n]->funcDefs.Exists(f) )
			return configGroups[n];
	}

	return 0;
}

// interface
asDWORD asCScriptEngine::SetDefaultAccessMask(asDWORD defaultMask)
{
	asDWORD old = defaultAccessMask;
	defaultAccessMask = defaultMask;
	return old;
}

int asCScriptEngine::GetNextScriptFunctionId()
{
	if( freeScriptFunctionIds.GetLength() )
		return freeScriptFunctionIds.PopLast();

	int id = (int)scriptFunctions.GetLength();
	scriptFunctions.PushLast(0);
	return id;
}

void asCScriptEngine::SetScriptFunction(asCScriptFunction *func)
{
	scriptFunctions[func->id] = func;
}

void asCScriptEngine::FreeScriptFunctionId(int id)
{
	if( id < 0 ) return;
	id &= 0xFFFF;
	if( id >= (int)scriptFunctions.GetLength() ) return;

	if( scriptFunctions[id] )
	{
		asCScriptFunction *func = scriptFunctions[id];

		// Remove the function from the list of script functions
		if( id == (int)scriptFunctions.GetLength() - 1 )
		{
			scriptFunctions.PopLast();
		}
		else
		{
			scriptFunctions[id] = 0;
			freeScriptFunctionIds.PushLast(id);
		}

		// Is the function used as signature id?
		if( func->signatureId == id )
		{
			// Remove the signature id
			signatureIds.RemoveValue(func);

			// Update all functions using the signature id
			int newSigId = 0;
			for( asUINT n = 0; n < scriptFunctions.GetLength(); n++ )
			{
				if( scriptFunctions[n] && scriptFunctions[n]->signatureId == id )
				{
					if( newSigId == 0 )
					{
						newSigId = scriptFunctions[n]->id;
						signatureIds.PushLast(scriptFunctions[n]);
					}

					scriptFunctions[n]->signatureId = newSigId;
				}
			}
		}
	}
}

// interface
int asCScriptEngine::RegisterFuncdef(const char *decl)
{
	if( decl == 0 ) return ConfigError(asINVALID_ARG, "RegisterFuncdef", decl, 0);

	// Parse the function declaration
	asCScriptFunction *func = asNEW(asCScriptFunction)(this, 0, asFUNC_FUNCDEF);
	if( func == 0 )
		return ConfigError(asOUT_OF_MEMORY, "RegisterFuncdef", decl, 0);

	asCBuilder bld(this, 0);
	int r = bld.ParseFunctionDeclaration(0, decl, func, false, 0, 0, defaultNamespace);
	if( r < 0 )
	{
		// Set as dummy function before deleting
		func->funcType = asFUNC_DUMMY;
		asDELETE(func,asCScriptFunction);
		return ConfigError(asINVALID_DECLARATION, "RegisterFuncdef", decl, 0);
	}

	// Check name conflicts
	r = bld.CheckNameConflict(func->name.AddressOf(), 0, 0, defaultNamespace);
	if( r < 0 )
	{
		asDELETE(func,asCScriptFunction);
		return ConfigError(asNAME_TAKEN, "RegisterFuncdef", decl, 0);
	}

	func->id = GetNextScriptFunctionId();
	SetScriptFunction(func);

	funcDefs.PushLast(func);
	registeredFuncDefs.PushLast(func);
	currentGroup->funcDefs.PushLast(func);

	// If parameter type from other groups are used, add references
	if( func->returnType.GetObjectType() )
	{
		asCConfigGroup *group = FindConfigGroupForObjectType(func->returnType.GetObjectType());
		currentGroup->RefConfigGroup(group);
	}
	for( asUINT n = 0; n < func->parameterTypes.GetLength(); n++ )
	{
		if( func->parameterTypes[n].GetObjectType() )
		{
			asCConfigGroup *group = FindConfigGroupForObjectType(func->parameterTypes[n].GetObjectType());
			currentGroup->RefConfigGroup(group);
		}
	}

	// Return the function id as success
	return func->id;
}

// interface
asUINT asCScriptEngine::GetFuncdefCount() const
{
	return asUINT(registeredFuncDefs.GetLength());
}

// interface
asIScriptFunction *asCScriptEngine::GetFuncdefByIndex(asUINT index) const
{
	if( index >= registeredFuncDefs.GetLength() )
		return 0;

	return registeredFuncDefs[index];
}

// interface
// TODO: typedef: Accept complex types for the typedefs
int asCScriptEngine::RegisterTypedef(const char *type, const char *decl)
{
	if( type == 0 ) return ConfigError(asINVALID_NAME, "RegisterTypedef", type, decl);

	// Verify if the name has been registered as a type already
	asUINT n;
	for( n = 0; n < objectTypes.GetLength(); n++ )
	{
		if( objectTypes[n] && objectTypes[n]->name == type && objectTypes[n]->nameSpace == defaultNamespace )
			// Let the application recover from this error, for example if the same typedef is registered twice
			return asALREADY_REGISTERED;
	}

	// Grab the data type
	size_t tokenLen;
	eTokenType token;
	asCDataType dataType;

	//	Create the data type
	token = tok.GetToken(decl, strlen(decl), &tokenLen);
	switch(token)
	{
	case ttBool:
	case ttInt:
	case ttInt8:
	case ttInt16:
	case ttInt64:
	case ttUInt:
	case ttUInt8:
	case ttUInt16:
	case ttUInt64:
	case ttFloat:
	case ttDouble:
		if( strlen(decl) != tokenLen )
		{
			return ConfigError(asINVALID_TYPE, "RegisterTypedef", type, decl);
		}
		break;

	default:
		return ConfigError(asINVALID_TYPE, "RegisterTypedef", type, decl);
	}

	dataType = asCDataType::CreatePrimitive(token, false);

	// Make sure the name is not a reserved keyword
	token = tok.GetToken(type, strlen(type), &tokenLen);
	if( token != ttIdentifier || strlen(type) != tokenLen )
		return ConfigError(asINVALID_NAME, "RegisterTypedef", type, decl);

	asCBuilder bld(this, 0);
	int r = bld.CheckNameConflict(type, 0, 0, defaultNamespace);
	if( r < 0 )
		return ConfigError(asNAME_TAKEN, "RegisterTypedef", type, decl);

	// Don't have to check against members of object
	// types as they are allowed to use the names

	// Put the data type in the list
	asCObjectType *object = asNEW(asCObjectType)(this);
	if( object == 0 )
		return ConfigError(asOUT_OF_MEMORY, "RegisterTypedef", type, decl);

	object->flags           = asOBJ_TYPEDEF;
	object->size            = dataType.GetSizeInMemoryBytes();
	object->name            = type;
	object->nameSpace       = defaultNamespace;
	object->templateSubType = dataType;

	objectTypes.PushLast(object);
	registeredTypeDefs.PushLast(object);

	currentGroup->objTypes.PushLast(object);

	return asSUCCESS;
}

// interface
asUINT asCScriptEngine::GetTypedefCount() const
{
	return asUINT(registeredTypeDefs.GetLength());
}

// interface
const char *asCScriptEngine::GetTypedefByIndex(asUINT index, int *typeId, const char **nameSpace, const char **configGroup, asDWORD *accessMask) const
{
	if( index >= registeredTypeDefs.GetLength() )
		return 0;

	if( typeId )
		*typeId = GetTypeIdFromDataType(registeredTypeDefs[index]->templateSubType);

	if( configGroup )
	{
		asCConfigGroup *group = FindConfigGroupForObjectType(registeredTypeDefs[index]);
		if( group )
			*configGroup = group->groupName.AddressOf();
		else
			*configGroup = 0;
	}

	if( accessMask )
		*accessMask = registeredTypeDefs[index]->accessMask;

	if( nameSpace )
		*nameSpace = registeredTypeDefs[index]->nameSpace->name.AddressOf();

	return registeredTypeDefs[index]->name.AddressOf();
}

// interface
int asCScriptEngine::RegisterEnum(const char *name)
{
	//	Check the name
	if( NULL == name )
		return ConfigError(asINVALID_NAME, "RegisterEnum", name, 0);

	// Verify if the name has been registered as a type already
	asUINT n;
	for( n = 0; n < objectTypes.GetLength(); n++ )
		if( objectTypes[n] && objectTypes[n]->name == name && objectTypes[n]->nameSpace == defaultNamespace )
			return asALREADY_REGISTERED;

	// Use builder to parse the datatype
	asCDataType dt;
	asCBuilder bld(this, 0);
	bool oldMsgCallback = msgCallback; msgCallback = false;
	int r = bld.ParseDataType(name, &dt, defaultNamespace);
	msgCallback = oldMsgCallback;
	if( r >= 0 )
		return ConfigError(asERROR, "RegisterEnum", name, 0);

	// Make sure the name is not a reserved keyword
	size_t tokenLen;
	int token = tok.GetToken(name, strlen(name), &tokenLen);
	if( token != ttIdentifier || strlen(name) != tokenLen )
		return ConfigError(asINVALID_NAME, "RegisterEnum", name, 0);

	r = bld.CheckNameConflict(name, 0, 0, defaultNamespace);
	if( r < 0 )
		return ConfigError(asNAME_TAKEN, "RegisterEnum", name, 0);

	asCObjectType *st = asNEW(asCObjectType)(this);
	if( st == 0 )
		return ConfigError(asOUT_OF_MEMORY, "RegisterEnum", name, 0);

	asCDataType dataType;
	dataType.CreatePrimitive(ttInt, false);

	st->flags = asOBJ_ENUM | asOBJ_SHARED;
	st->size = 4;
	st->name = name;
	st->nameSpace = defaultNamespace;

	objectTypes.PushLast(st);
	registeredEnums.PushLast(st);

	currentGroup->objTypes.PushLast(st);

	return asSUCCESS;
}

// interface
int asCScriptEngine::RegisterEnumValue(const char *typeName, const char *valueName, int value)
{
	// Verify that the correct config group is used
	if( currentGroup->FindType(typeName) == 0 )
		return ConfigError(asWRONG_CONFIG_GROUP, "RegisterEnumValue", typeName, valueName);

	asCDataType dt;
	int r;
	asCBuilder bld(this, 0);
	r = bld.ParseDataType(typeName, &dt, defaultNamespace);
	if( r < 0 )
		return ConfigError(r, "RegisterEnumValue", typeName, valueName);

	// Store the enum value
	asCObjectType *ot = dt.GetObjectType();
	if( ot == 0 || !(ot->flags & asOBJ_ENUM) )
		return ConfigError(asINVALID_TYPE, "RegisterEnumValue", typeName, valueName);

	if( NULL == valueName )
		return ConfigError(asINVALID_NAME, "RegisterEnumValue", typeName, valueName);

	int tokenLen;
	asETokenClass tokenClass = ParseToken(valueName, 0, &tokenLen);
	if( tokenClass != asTC_IDENTIFIER || tokenLen != (int)strlen(valueName) )
		return ConfigError(asINVALID_NAME, "RegisterEnumValue", typeName, valueName);

	for( unsigned int n = 0; n < ot->enumValues.GetLength(); n++ )
	{
		if( ot->enumValues[n]->name == valueName )
			return ConfigError(asALREADY_REGISTERED, "RegisterEnumValue", typeName, valueName);
	}

	asSEnumValue *e = asNEW(asSEnumValue);
	if( e == 0 )
		return ConfigError(asOUT_OF_MEMORY, "RegisterEnumValue", typeName, valueName);

	e->name = valueName;
	e->value = value;

	ot->enumValues.PushLast(e);

	return asSUCCESS;
}

// interface
asUINT asCScriptEngine::GetEnumCount() const
{
	return asUINT(registeredEnums.GetLength());
}

// interface
const char *asCScriptEngine::GetEnumByIndex(asUINT index, int *enumTypeId, const char **nameSpace, const char **configGroup, asDWORD *accessMask) const
{
	if( index >= registeredEnums.GetLength() )
		return 0;

	if( configGroup )
	{
		asCConfigGroup *group = FindConfigGroupForObjectType(registeredEnums[index]);
		if( group )
			*configGroup = group->groupName.AddressOf();
		else
			*configGroup = 0;
	}

	if( accessMask )
		*accessMask = registeredEnums[index]->accessMask;

	if( enumTypeId )
		*enumTypeId = GetTypeIdFromDataType(asCDataType::CreateObject(registeredEnums[index], false));

	if( nameSpace )
		*nameSpace = registeredEnums[index]->nameSpace->name.AddressOf();

	return registeredEnums[index]->name.AddressOf();
}

// interface
int asCScriptEngine::GetEnumValueCount(int enumTypeId) const
{
	asCDataType dt = GetDataTypeFromTypeId(enumTypeId);
	asCObjectType *t = dt.GetObjectType();
	if( t == 0 || !(t->GetFlags() & asOBJ_ENUM) )
		return asINVALID_TYPE;

	return (int)t->enumValues.GetLength();
}

// interface
const char *asCScriptEngine::GetEnumValueByIndex(int enumTypeId, asUINT index, int *outValue) const
{
	// TODO: This same function is implemented in as_module.cpp as well. Perhaps it should be moved to asCObjectType?
	asCDataType dt = GetDataTypeFromTypeId(enumTypeId);
	asCObjectType *t = dt.GetObjectType();
	if( t == 0 || !(t->GetFlags() & asOBJ_ENUM) )
		return 0;

	if( index >= t->enumValues.GetLength() )
		return 0;

	if( outValue )
		*outValue = t->enumValues[index]->value;

	return t->enumValues[index]->name.AddressOf();
}

// interface
asUINT asCScriptEngine::GetObjectTypeCount() const
{
	return asUINT(registeredObjTypes.GetLength());
}

// interface
asIObjectType *asCScriptEngine::GetObjectTypeByIndex(asUINT index) const
{
	if( index >= registeredObjTypes.GetLength() )
		return 0;

	return registeredObjTypes[index];
}

// interface
asIObjectType *asCScriptEngine::GetObjectTypeByName(const char *name) const
{
	for( asUINT n = 0; n < registeredObjTypes.GetLength(); n++ )
	{
		if( registeredObjTypes[n]->name == name &&
			registeredObjTypes[n]->nameSpace == defaultNamespace )
			return registeredObjTypes[n];
	}

	return 0;
}

// interface
asIObjectType *asCScriptEngine::GetObjectTypeById(int typeId) const
{
	asCDataType dt = GetDataTypeFromTypeId(typeId);

	// Is the type id valid?
	if( !dt.IsValid() ) return 0;

	// Enum types are not objects, so we shouldn't return an object type for them
	if( dt.GetObjectType() && dt.GetObjectType()->GetFlags() & asOBJ_ENUM )
		return 0;

	return dt.GetObjectType();
}

// interface
asIScriptFunction *asCScriptEngine::GetFunctionById(int funcId) const
{
	return GetScriptFunction(funcId);
}

// internal
bool asCScriptEngine::IsTemplateType(const char *name) const
{
	// TODO: optimize: Improve linear search
	for( unsigned int n = 0; n < objectTypes.GetLength(); n++ )
	{
		if( objectTypes[n] && objectTypes[n]->name == name )
		{
			return objectTypes[n]->flags & asOBJ_TEMPLATE ? true : false;
		}
	}

	return false;
}

// internal
int asCScriptEngine::AddConstantString(const char *str, size_t len)
{
	// This is only called when build a script module, so it is
	// known that only one thread can enter the function at a time.
	asASSERT( isBuilding );

	// The str may contain null chars, so we cannot use strlen, or strcmp, or strcpy

	// Has the string been registered before?
	asSMapNode<asCStringPointer, int> *cursor = 0;
	if (stringToIdMap.MoveTo(&cursor, asCStringPointer(str, len)))
		return cursor->value;

	// No match was found, add the string
	asCString *cstr = asNEW(asCString)(str, len);
	if( cstr )
	{
		stringConstants.PushLast(cstr);
		int index = (int)stringConstants.GetLength() - 1;
		stringToIdMap.Insert(asCStringPointer(cstr), index);

		// The VM currently doesn't handle string ids larger than 65535
		asASSERT(stringConstants.GetLength() <= 65536);

		return index;
	}

	return 0;
}

// internal
const asCString &asCScriptEngine::GetConstantString(int id)
{
	return *stringConstants[id];
}

// internal
int asCScriptEngine::GetScriptSectionNameIndex(const char *name)
{
	ACQUIREEXCLUSIVE(engineRWLock);

	// TODO: These names are only released when the engine is freed. The assumption is that
	//       the same script section names will be reused instead of there always being new
	//       names. Is this assumption valid? Do we need to add reference counting?

	// Store the script section names for future reference
	for( asUINT n = 0; n < scriptSectionNames.GetLength(); n++ )
	{
		if( scriptSectionNames[n]->Compare(name) == 0 )
		{
			RELEASEEXCLUSIVE(engineRWLock);
			return n;
		}
	}

	asCString *str = asNEW(asCString)(name);
	if( str )
		scriptSectionNames.PushLast(str);
	int r = int(scriptSectionNames.GetLength()-1);

	RELEASEEXCLUSIVE(engineRWLock);

	return r;
}

// interface
void asCScriptEngine::SetEngineUserDataCleanupCallback(asCLEANENGINEFUNC_t callback, asPWORD type)
{
	ACQUIREEXCLUSIVE(engineRWLock);

	for( asUINT n = 0; n < cleanEngineFuncs.GetLength(); n++ )
	{
		if( cleanEngineFuncs[n].type == type )
		{
			cleanEngineFuncs[n].cleanFunc = callback;

			RELEASEEXCLUSIVE(engineRWLock);

			return;
		}
	}
	SEngineClean otc = {type, callback};
	cleanEngineFuncs.PushLast(otc);

	RELEASEEXCLUSIVE(engineRWLock);
}

// interface
void asCScriptEngine::SetModuleUserDataCleanupCallback(asCLEANMODULEFUNC_t callback)
{
	cleanModuleFunc = callback;
}

// interface
void asCScriptEngine::SetContextUserDataCleanupCallback(asCLEANCONTEXTFUNC_t callback)
{
	cleanContextFunc = callback;
}

// interface
void asCScriptEngine::SetFunctionUserDataCleanupCallback(asCLEANFUNCTIONFUNC_t callback)
{
	cleanFunctionFunc = callback;
}

// interface
void asCScriptEngine::SetObjectTypeUserDataCleanupCallback(asCLEANOBJECTTYPEFUNC_t callback, asPWORD type)
{
	ACQUIREEXCLUSIVE(engineRWLock);

	for( asUINT n = 0; n < cleanObjectTypeFuncs.GetLength(); n++ )
	{
		if( cleanObjectTypeFuncs[n].type == type )
		{
			cleanObjectTypeFuncs[n].cleanFunc = callback;

			RELEASEEXCLUSIVE(engineRWLock);

			return;
		}
	}
	SObjTypeClean otc = {type, callback};
	cleanObjectTypeFuncs.PushLast(otc);

	RELEASEEXCLUSIVE(engineRWLock);
}

END_AS_NAMESPACE

