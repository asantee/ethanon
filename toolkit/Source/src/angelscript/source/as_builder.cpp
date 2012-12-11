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
// as_builder.cpp
//
// This is the class that manages the compilation of the scripts
//


#include "as_config.h"
#include "as_builder.h"
#include "as_parser.h"
#include "as_compiler.h"
#include "as_tokendef.h"
#include "as_string_util.h"
#include "as_outputbuffer.h"
#include "as_texts.h"
#include "as_scriptobject.h"
#include "as_debug.h"

BEGIN_AS_NAMESPACE

#ifndef AS_NO_COMPILER

// asCSymbolTable template specializations for sGlobalVariableDescription entries
template<>
void asCSymbolTable<sGlobalVariableDescription>::GetKey(const sGlobalVariableDescription *entry, asCString &key) const
{
	// TODO: optimize: The key should be a struct, composed of namespace pointer and the name string
	asSNameSpace *ns = entry->property->nameSpace;
	asCString name = entry->property->name;
	key = ns->name + "::" + name;
}

// Comparator for exact variable search
class asCCompGlobVarType : public asIFilter
{
public:
	const asCDataType &m_type;
	asCCompGlobVarType(const asCDataType &type) : m_type(type) {}

	bool operator()(const void *p) const
	{
		const sGlobalVariableDescription* desc = reinterpret_cast<const sGlobalVariableDescription*>(p);
		return desc->datatype == m_type;
	}

private:
	// The assignment operator is required for MSVC9, otherwise it will complain that it is not possible to auto generate the operator
	asCCompGlobVarType &operator=(const asCCompGlobVarType &) {return *this;}
};

#endif

asCBuilder::asCBuilder(asCScriptEngine *engine, asCModule *module)
{
	this->engine = engine;
	this->module = module;
}

asCBuilder::~asCBuilder()
{
#ifndef AS_NO_COMPILER
	asUINT n;

	// Free all functions
	for( n = 0; n < functions.GetLength(); n++ )
	{
		if( functions[n] )
		{
			if( functions[n]->node )
			{
				functions[n]->node->Destroy(engine);
			}

			asDELETE(functions[n],sFunctionDescription);
		}

		functions[n] = 0;
	}

	// Free all global variables
	asCSymbolTable<sGlobalVariableDescription>::iterator it = globVariables.List();
	while( it )
	{
		if( (*it)->nextNode )
			(*it)->nextNode->Destroy(engine);
		asDELETE((*it),sGlobalVariableDescription);
		it++;
	}
	globVariables.Clear();

	// Free all the loaded files
	for( n = 0; n < scripts.GetLength(); n++ )
	{
		if( scripts[n] )
		{
			asDELETE(scripts[n],asCScriptCode);
		}

		scripts[n] = 0;
	}

	// Free all class declarations
	for( n = 0; n < classDeclarations.GetLength(); n++ )
	{
		if( classDeclarations[n] )
		{
			if( classDeclarations[n]->node )
			{
				classDeclarations[n]->node->Destroy(engine);
			}

			asDELETE(classDeclarations[n],sClassDeclaration);
			classDeclarations[n] = 0;
		}
	}

	for( n = 0; n < interfaceDeclarations.GetLength(); n++ )
	{
		if( interfaceDeclarations[n] )
		{
			if( interfaceDeclarations[n]->node )
			{
				interfaceDeclarations[n]->node->Destroy(engine);
			}

			asDELETE(interfaceDeclarations[n],sClassDeclaration);
			interfaceDeclarations[n] = 0;
		}
	}

	for( n = 0; n < namedTypeDeclarations.GetLength(); n++ )
	{
		if( namedTypeDeclarations[n] )
		{
			if( namedTypeDeclarations[n]->node )
			{
				namedTypeDeclarations[n]->node->Destroy(engine);
			}

			asDELETE(namedTypeDeclarations[n],sClassDeclaration);
			namedTypeDeclarations[n] = 0;
		}
	}

	for( n = 0; n < funcDefs.GetLength(); n++ )
	{
		if( funcDefs[n] )
		{
			if( funcDefs[n]->node )
				funcDefs[n]->node->Destroy(engine);

			asDELETE(funcDefs[n],sFuncDef);
			funcDefs[n] = 0;
		}
	}

	for( n = 0; n < mixinClasses.GetLength(); n++ )
	{
		if( mixinClasses[n] )
		{
			if( mixinClasses[n]->node )
				mixinClasses[n]->node->Destroy(engine);

			asDELETE(mixinClasses[n],sMixinClass);
			mixinClasses[n] = 0;
		}
	}

#endif // AS_NO_COMPILER
}

void asCBuilder::Reset()
{
	numErrors = 0;
	numWarnings = 0;
	preMessage.isSet = false;
}

#ifndef AS_NO_COMPILER
int asCBuilder::AddCode(const char *name, const char *code, int codeLength, int lineOffset, int sectionIdx, bool makeCopy)
{
	asCScriptCode *script = asNEW(asCScriptCode);
	if( script == 0 )
		return asOUT_OF_MEMORY;

	int r = script->SetCode(name, code, codeLength, makeCopy);
	script->lineOffset = lineOffset;
	script->idx = sectionIdx;
	scripts.PushLast(script);

	return r;
}

int asCBuilder::Build()
{
	Reset();

	ParseScripts();

	CompileInterfaces();
	CompileClasses();
	CompileGlobalVariables();
	CompileFunctions();

	// TODO: Attempt to reorder the initialization of global variables so that
	//       they do not access other uninitialized global variables out-of-order
	//       The builder needs to check for each of the global variable, what functions
	//       that are accessed, and what global variables are access by these functions.

	if( numErrors > 0 )
		return asERROR;

	return asSUCCESS;
}

int asCBuilder::CompileGlobalVar(const char *sectionName, const char *code, int lineOffset)
{
	Reset();

	// Add the string to the script code
	asCScriptCode *script = asNEW(asCScriptCode);
	if( script == 0 )
		return asOUT_OF_MEMORY;

	script->SetCode(sectionName, code, true);
	script->lineOffset = lineOffset;
	scripts.PushLast(script);

	// Parse the string
	asCParser parser(this);
	if( parser.ParseScript(scripts[0]) < 0 )
		return asERROR;

	asCScriptNode *node = parser.GetScriptNode();

	// Make sure there is nothing else than the global variable in the script code
	if( node == 0 ||
		node->firstChild == 0 ||
		node->firstChild != node->lastChild ||
		node->firstChild->nodeType != snGlobalVar )
	{
		WriteError(TXT_ONLY_ONE_VARIABLE_ALLOWED, script, 0);
		return asERROR;
	}

	node = node->firstChild;
	node->DisconnectParent();
	RegisterGlobalVar(node, script, module->defaultNamespace);

	CompileGlobalVariables();

	if( numErrors > 0 )
	{
		// Remove the variable from the module, if it was registered
		if( globVariables.GetSize() > 0 )
			module->RemoveGlobalVar(module->GetGlobalVarCount()-1);

		return asERROR;
	}

	return 0;
}
#endif

int asCBuilder::ValidateDefaultArgs(asCScriptCode *script, asCScriptNode *node, asCScriptFunction *func)
{
	int firstArgWithDefaultValue = -1;
	for( asUINT n = 0; n < func->defaultArgs.GetLength(); n++ )
	{
		if( func->defaultArgs[n] )
			firstArgWithDefaultValue = n;
		else if( firstArgWithDefaultValue >= 0 )
		{
			asCString str;
			str.Format(TXT_DEF_ARG_MISSING_IN_FUNC_s, func->GetDeclaration());
			WriteError(str, script, node);
			return asINVALID_DECLARATION;
		}
	}

	return 0;
}

#ifndef AS_NO_COMPILER
int asCBuilder::CompileFunction(const char *sectionName, const char *code, int lineOffset, asDWORD compileFlags, asCScriptFunction **outFunc)
{
	asASSERT(outFunc != 0);

	Reset();

	// Add the string to the script code
	asCScriptCode *script = asNEW(asCScriptCode);
	if( script == 0 )
		return asOUT_OF_MEMORY;

	script->SetCode(sectionName, code, true);
	script->lineOffset = lineOffset;
	scripts.PushLast(script);

	// Parse the string
	asCParser parser(this);
	if( parser.ParseScript(scripts[0]) < 0 )
		return asERROR;

	asCScriptNode *node = parser.GetScriptNode();

	// Make sure there is nothing else than the function in the script code
	if( node == 0 ||
		node->firstChild == 0 ||
		node->firstChild != node->lastChild ||
		node->firstChild->nodeType != snFunction )
	{
		WriteError(TXT_ONLY_ONE_FUNCTION_ALLOWED, script, 0);
		return asERROR;
	}

	// Find the function node
	node = node->firstChild;

	// Create the function
	bool isConstructor, isDestructor, isPrivate, isFinal, isOverride, isShared;
	asCScriptFunction *func = asNEW(asCScriptFunction)(engine, compileFlags & asCOMP_ADD_TO_MODULE ? module : 0, asFUNC_SCRIPT);
	if( func == 0 )
		return asOUT_OF_MEMORY;

	asCArray<asCString> parameterNames;
	GetParsedFunctionDetails(node, scripts[0], 0, func->name, func->returnType, parameterNames, func->parameterTypes, func->inOutFlags, func->defaultArgs, func->isReadOnly, isConstructor, isDestructor, isPrivate, isFinal, isOverride, isShared);
	func->id               = engine->GetNextScriptFunctionId();
	func->scriptSectionIdx = engine->GetScriptSectionNameIndex(sectionName ? sectionName : "");
	func->nameSpace        = module->defaultNamespace;

	// Make sure the default args are declared correctly
	int r = ValidateDefaultArgs(script, node, func);
	if( r < 0 )
	{
		func->Release();
		return asERROR;
	}

	// Tell the engine that the function exists already so the compiler can access it
	if( compileFlags & asCOMP_ADD_TO_MODULE )
	{
		int r = CheckNameConflict(func->name.AddressOf(), node, scripts[0], module->defaultNamespace);
		if( r < 0 )
		{
			func->Orphan(module);
			return asERROR;
		}

		module->globalFunctions.Put(func);
		func->AddRef();
		module->AddScriptFunction(func);
	}
	else
		engine->SetScriptFunction(func);

	// Fill in the function info for the builder too
	node->DisconnectParent();
	sFunctionDescription *funcDesc = asNEW(sFunctionDescription);
	if( funcDesc == 0 )
	{
		func->Release();
		return asOUT_OF_MEMORY;
	}

	functions.PushLast(funcDesc);
	funcDesc->script            = scripts[0];
	funcDesc->node              = node;
	funcDesc->name              = func->name;
	funcDesc->funcId            = func->id;
	funcDesc->paramNames        = parameterNames;

	asCCompiler compiler(engine);
	if( compiler.CompileFunction(this, functions[0]->script, parameterNames, functions[0]->node, func, 0) >= 0 )
	{
		// Return the function
		*outFunc = func;
	}
	else
	{
		// If the function was added to the module then remove it again
		if( compileFlags & asCOMP_ADD_TO_MODULE )
		{
			module->globalFunctions.Erase(module->globalFunctions.GetIndex(func));
			module->scriptFunctions.RemoveValue(func);
			func->Release();
			func->Orphan(module);
		}

		func->Release();

		return asERROR;
	}

	return asSUCCESS;
}

void asCBuilder::ParseScripts()
{
	TimeIt("asCBuilder::ParseScripts");

	asCArray<asCParser*> parsers((int)scripts.GetLength());

	// Parse all the files as if they were one
	asUINT n = 0;
	for( n = 0; n < scripts.GetLength(); n++ )
	{
		asCParser *parser = asNEW(asCParser)(this);
		if( parser != 0 )
		{
			parsers.PushLast(parser);

			// Parse the script file
			parser->ParseScript(scripts[n]);
		}
	}

	if( numErrors == 0 )
	{
		// Find all type declarations
		for( n = 0; n < scripts.GetLength(); n++ )
		{
			asCScriptNode *node = parsers[n]->GetScriptNode();
			RegisterTypesFromScript(node, scripts[n], engine->nameSpaces[0]);
		}

		// Register the complete function definitions
		for( n = 0; n < funcDefs.GetLength(); n++ )
		{
			CompleteFuncDef(funcDefs[n]);
		}

		// Register script methods found in the interfaces
		for( n = 0; n < interfaceDeclarations.GetLength(); n++ )
		{
			sClassDeclaration *decl = interfaceDeclarations[n];
			asCScriptNode *node = decl->node->firstChild->next;

			// Skip list of inherited interfaces
			while( node && node->nodeType == snIdentifier )
				node = node->next;

			while( node )
			{
				asCScriptNode *next = node->next;
				if( node->nodeType == snFunction )
				{
					node->DisconnectParent();
					RegisterScriptFunctionFromNode(engine->GetNextScriptFunctionId(), node, decl->script, decl->objType, true, false, 0, decl->isExistingShared);
				}
				else if( node->nodeType == snVirtualProperty )
				{
					node->DisconnectParent();
					RegisterVirtualProperty(node, decl->script, decl->objType, true, false, 0, decl->isExistingShared);
				}

				node = next;
			}
		}

		// Register script methods found in the classes
		for( n = 0; n < classDeclarations.GetLength(); n++ )
		{
			sClassDeclaration *decl = classDeclarations[n];

			asCScriptNode *node = decl->node->firstChild->next;

			// Skip list of classes and interfaces
			while( node && node->nodeType == snIdentifier )
				node = node->next;

			while( node )
			{
				asCScriptNode *next = node->next;
				if( node->nodeType == snFunction )
				{
					node->DisconnectParent();
					RegisterScriptFunctionFromNode(engine->GetNextScriptFunctionId(), node, decl->script, decl->objType, false, false, 0, decl->isExistingShared);
				}
				else if( node->nodeType == snVirtualProperty )
				{
					node->DisconnectParent();
					RegisterVirtualProperty(node, decl->script, decl->objType, false, false, 0, decl->isExistingShared);
				}

				node = next;
			}

			// Make sure the default factory & constructor exists for classes
			if( decl->objType->beh.construct == engine->scriptTypeBehaviours.beh.construct )
			{
				if( decl->objType->beh.constructors.GetLength() == 1 || engine->ep.alwaysImplDefaultConstruct )
				{
					AddDefaultConstructor(decl->objType, decl->script);
				}
				else
				{
					// As the class has another constructor we shouldn't provide the default constructor
					if( decl->objType->beh.construct )
					{
						engine->scriptFunctions[decl->objType->beh.construct]->Release();
						decl->objType->beh.construct = 0;
						decl->objType->beh.constructors.RemoveIndex(0);
					}
					if( decl->objType->beh.factory )
					{
						engine->scriptFunctions[decl->objType->beh.factory]->Release();
						decl->objType->beh.factory = 0;
						decl->objType->beh.factories.RemoveIndex(0);
					}
					if( decl->objType->beh.copy )
					{
						engine->scriptFunctions[decl->objType->beh.copy]->Release();
						decl->objType->beh.copy = 0;
					}
				}
			}
		}

		// Find other global nodes
		for( n = 0; n < scripts.GetLength(); n++ )
		{
			// Find other global nodes
			asCScriptNode *node = parsers[n]->GetScriptNode();
			RegisterNonTypesFromScript(node, scripts[n], engine->nameSpaces[0]);
		}
	}

	for( n = 0; n < parsers.GetLength(); n++ )
	{
		asDELETE(parsers[n],asCParser);
	}
}

void asCBuilder::RegisterTypesFromScript(asCScriptNode *node, asCScriptCode *script, asSNameSpace *ns)
{
	asASSERT(node->nodeType == snScript);

	// Find structure definitions first
	node = node->firstChild;
	while( node )
	{
		asCScriptNode *next = node->next;
		if( node->nodeType == snNamespace )
		{
			// Recursively register the entities defined in the namespace
			asCString nsName;
			nsName.Assign(&script->code[node->firstChild->tokenPos], node->firstChild->tokenLength);
			if( ns->name != "" )
				nsName = ns->name + "::" + nsName;

			asSNameSpace *nsChild = engine->AddNameSpace(nsName.AddressOf());
			RegisterTypesFromScript(node->lastChild, script, nsChild);
		}
		else
		{
			if( node->nodeType == snClass )
			{
				node->DisconnectParent();
				RegisterClass(node, script, ns);
			}
			else if( node->nodeType == snInterface )
			{
				node->DisconnectParent();
				RegisterInterface(node, script, ns);
			}
			else if( node->nodeType == snEnum )
			{
				node->DisconnectParent();
				RegisterEnum(node, script, ns);
			}
			else if( node->nodeType == snTypedef )
			{
				node->DisconnectParent();
				RegisterTypedef(node, script, ns);
			}
			else if( node->nodeType == snFuncDef )
			{
				node->DisconnectParent();
				RegisterFuncDef(node, script, ns);
			}
			else if( node->nodeType == snMixin )
			{
				node->DisconnectParent();
				RegisterMixinClass(node, script, ns);
			}
		}

		node = next;
	}
}

void asCBuilder::RegisterNonTypesFromScript(asCScriptNode *node, asCScriptCode *script, asSNameSpace *ns)
{
	node = node->firstChild;
	while( node )
	{
		asCScriptNode *next = node->next;
		if( node->nodeType == snNamespace )
		{
			// Determine the name of the namespace
			asCString nsName;
			nsName.Assign(&script->code[node->firstChild->tokenPos], node->firstChild->tokenLength);
			if( ns->name != "" )
				nsName = ns->name + "::" + nsName;

			// Declare the namespace, then add the entities
			asSNameSpace *nsChild = engine->AddNameSpace(nsName.AddressOf());
			RegisterNonTypesFromScript(node->lastChild, script, nsChild);
		}
		else
		{
			node->DisconnectParent();
			if( node->nodeType == snFunction )
				RegisterScriptFunctionFromNode(engine->GetNextScriptFunctionId(), node, script, 0, false, true, ns);
			else if( node->nodeType == snGlobalVar )
				RegisterGlobalVar(node, script, ns);
			else if( node->nodeType == snVirtualProperty )
				RegisterVirtualProperty(node, script, 0, false, true, ns);
			else if( node->nodeType == snImport )
				RegisterImportedFunction(module->GetNextImportedFunctionId(), node, script, ns);
			else
			{
				// Unused script node
				int r, c;
				script->ConvertPosToRowCol(node->tokenPos, &r, &c);

				WriteWarning(script->name, TXT_UNUSED_SCRIPT_NODE, r, c);

				node->Destroy(engine);
			}
		}

		node = next;
	}
}

void asCBuilder::CompileFunctions()
{
	// Compile each function
	for( asUINT n = 0; n < functions.GetLength(); n++ )
	{
		sFunctionDescription *current = functions[n];
		if( current == 0 ) continue;

		asCCompiler compiler(engine);
		asCScriptFunction *func = engine->scriptFunctions[current->funcId];

		// Find the class declaration for constructors
		sClassDeclaration *classDecl = 0;
		if( current->objType && current->name == current->objType->name )
		{
			for( asUINT n = 0; n < classDeclarations.GetLength(); n++ )
			{
				if( classDeclarations[n]->name == current->name )
				{
					classDecl = classDeclarations[n];
					break;
				}
			}
		}

		if( current->node )
		{
			int r, c;
			current->script->ConvertPosToRowCol(current->node->tokenPos, &r, &c);

			asCString str = func->GetDeclarationStr();
			str.Format(TXT_COMPILING_s, str.AddressOf());
			WriteInfo(current->script->name, str, r, c, true);

			// When compiling a constructor need to pass the class declaration for member initializations
			compiler.CompileFunction(this, current->script, current->paramNames, current->node, func, classDecl);

			preMessage.isSet = false;
		}
		else if( current->name == current->objType->name )
		{
			asCScriptNode *node = classDecl->node;
			
			int r = 0, c = 0;
			if( node )
				current->script->ConvertPosToRowCol(node->tokenPos, &r, &c);

			asCString str = func->GetDeclarationStr();
			str.Format(TXT_COMPILING_s, str.AddressOf());
			WriteInfo(current->script->name, str, r, c, true);

			// This is the default constructor that is generated
			// automatically if not implemented by the user.
			compiler.CompileDefaultConstructor(this, current->script, node, func, classDecl);

			preMessage.isSet = false;
		}
	}
}
#endif

// Called from module and engine
int asCBuilder::ParseDataType(const char *datatype, asCDataType *result, asSNameSpace *implicitNamespace, bool isReturnType)
{
	Reset();

	asCScriptCode source;
	source.SetCode("", datatype, true);

	asCParser parser(this);
	int r = parser.ParseDataType(&source, isReturnType);
	if( r < 0 )
		return asINVALID_TYPE;

	// Get data type and property name
	asCScriptNode *dataType = parser.GetScriptNode()->firstChild;

	*result = CreateDataTypeFromNode(dataType, &source, implicitNamespace, true);
	if( isReturnType )
		*result = ModifyDataTypeFromNode(*result, dataType->next, &source, 0, 0);

	if( numErrors > 0 )
		return asINVALID_TYPE;

	return asSUCCESS;
}

int asCBuilder::ParseTemplateDecl(const char *decl, asCString *name, asCString *subtypeName)
{
	Reset();

	asCScriptCode source;
	source.SetCode("", decl, true);

	asCParser parser(this);
	int r = parser.ParseTemplateDecl(&source);
	if( r < 0 )
		return asINVALID_TYPE;

	// Get the template name and subtype name
	asCScriptNode *node = parser.GetScriptNode()->firstChild;

	name->Assign(&decl[node->tokenPos], node->tokenLength);
	node = node->next;
	subtypeName->Assign(&decl[node->tokenPos], node->tokenLength);

	// TODO: template: check for name conflicts

	if( numErrors > 0 )
		return asINVALID_DECLARATION;

	return asSUCCESS;
}

int asCBuilder::VerifyProperty(asCDataType *dt, const char *decl, asCString &name, asCDataType &type, asSNameSpace *ns)
{
	// Either datatype or namespace must be informed
	asASSERT( dt || ns );

	Reset();

	if( dt )
	{
		// Verify that the object type exist
		if( dt->GetObjectType() == 0 )
			return asINVALID_OBJECT;
	}

	// Check property declaration and type
	asCScriptCode source;
	source.SetCode(TXT_PROPERTY, decl, true);

	asCParser parser(this);
	int r = parser.ParsePropertyDeclaration(&source);
	if( r < 0 )
		return asINVALID_DECLARATION;

	// Get data type and property name
	asCScriptNode *dataType = parser.GetScriptNode()->firstChild;

	asCScriptNode *nameNode = dataType->next;

	// If an object property is registered, then use the
	// object's namespace, otherwise use the specified namespace
	type = CreateDataTypeFromNode(dataType, &source, dt ? dt->GetObjectType()->nameSpace : ns);
	name.Assign(&decl[nameNode->tokenPos], nameNode->tokenLength);

	// Validate that the type really can be a registered property
	// We cannot use CanBeInstanciated, as it is allowed to register
	// properties of type that cannot otherwise be instanciated
	if( type.GetFuncDef() && !type.IsObjectHandle() )
	{
		// Function definitions must always be handles
		return asINVALID_DECLARATION;
	}

	// Verify property name
	if( dt )
	{
		if( CheckNameConflictMember(dt->GetObjectType(), name.AddressOf(), nameNode, &source, true) < 0 )
			return asNAME_TAKEN;
	}
	else
	{
		if( CheckNameConflict(name.AddressOf(), nameNode, &source, ns) < 0 )
			return asNAME_TAKEN;
	}

	if( numErrors > 0 )
		return asINVALID_DECLARATION;

	return asSUCCESS;
}

asCObjectProperty *asCBuilder::GetObjectProperty(asCDataType &obj, const char *prop)
{
	asASSERT(obj.GetObjectType() != 0);

	// TODO: optimize: Improve linear search
	asCArray<asCObjectProperty *> &props = obj.GetObjectType()->properties;
	for( asUINT n = 0; n < props.GetLength(); n++ )
	{
		if( props[n]->name == prop )
		{
			if( module->accessMask & props[n]->accessMask )
				return props[n];
			else
				return 0;
		}
	}

	return 0;
}

asCGlobalProperty *asCBuilder::GetGlobalProperty(const char *prop, asSNameSpace *ns, bool *isCompiled, bool *isPureConstant, asQWORD *constantValue, bool *isAppProp)
{
	if( isCompiled )     *isCompiled     = true;
	if( isPureConstant ) *isPureConstant = false;
	if( isAppProp )      *isAppProp      = false;

	// Check application registered properties
	asCString name(prop);
	asCGlobalProperty *globProp = engine->registeredGlobalProps.GetFirst(ns, name);
	if( globProp )
	{
		if( module )
		{
			// Determine if the module has access to the property
			if( module->accessMask & globProp->accessMask )
			{
				if( isAppProp ) *isAppProp = true;
				return globProp;
			}
		}
		else
		{
			// We're not compiling a module right now, so it must be a registered global property
			if( isAppProp ) *isAppProp = true;
			return globProp;
		}
	}

#ifndef AS_NO_COMPILER
	// Check properties being compiled now
	sGlobalVariableDescription* desc = globVariables.GetFirst(ns, prop);
	if( desc )
	{
		if( isCompiled )     *isCompiled     = desc->isCompiled;
		if( isPureConstant ) *isPureConstant = desc->isPureConstant;
		if( constantValue  ) *constantValue  = desc->constantValue;
		return desc->property;
	}
#else
	UNUSED_VAR(constantValue);
#endif

	// Check previously compiled global variables
	if( module )
		return module->scriptGlobals.GetFirst(ns, prop);

	return 0;
}

int asCBuilder::ParseFunctionDeclaration(asCObjectType *objType, const char *decl, asCScriptFunction *func, bool isSystemFunction, asCArray<bool> *paramAutoHandles, bool *returnAutoHandle, asSNameSpace *ns)
{
	asASSERT( objType || ns );

	// TODO: Can't we use GetParsedFunctionDetails to do most of what is done in this function?

	Reset();

	asCScriptCode source;
	source.SetCode(TXT_SYSTEM_FUNCTION, decl, true);

	asCParser parser(this);
	int r = parser.ParseFunctionDefinition(&source);
	if( r < 0 )
		return asINVALID_DECLARATION;

	asCScriptNode *node = parser.GetScriptNode();

	// Determine scope
	asCScriptNode *n = node->firstChild->next->next;
	asCString scope = GetScopeFromNode(n, &source, &n);
	func->nameSpace = engine->FindNameSpace(scope.AddressOf());
	if( func->nameSpace == 0 )
		return asINVALID_DECLARATION;

	// Find name
	func->name.Assign(&source.code[n->tokenPos], n->tokenLength);

	// Initialize a script function object for registration
	bool autoHandle;

	// Scoped reference types are allowed to use handle when returned from application functions
	func->returnType = CreateDataTypeFromNode(node->firstChild, &source, objType ? objType->nameSpace : ns, true, objType);
	func->returnType = ModifyDataTypeFromNode(func->returnType, node->firstChild->next, &source, 0, &autoHandle);
	if( autoHandle && (!func->returnType.IsObjectHandle() || func->returnType.IsReference()) )
			return asINVALID_DECLARATION;
	if( returnAutoHandle ) *returnAutoHandle = autoHandle;

	// Reference types cannot be returned by value from system functions
	if( isSystemFunction &&
		(func->returnType.GetObjectType() &&
		 (func->returnType.GetObjectType()->flags & asOBJ_REF)) &&
        !(func->returnType.IsReference() ||
		  func->returnType.IsObjectHandle()) )
		return asINVALID_DECLARATION;

	// Count number of parameters
	int paramCount = 0;
	asCScriptNode *paramList = n->next;
	n = paramList->firstChild;
	while( n )
	{
		paramCount++;
		n = n->next->next;
		if( n && n->nodeType == snIdentifier )
			n = n->next;

		if( n && n->nodeType == snExpression )
			n = n->next;
	}

	// Preallocate memory
	func->parameterTypes.Allocate(paramCount, false);
	func->inOutFlags.Allocate(paramCount, false);
	func->defaultArgs.Allocate(paramCount, false);
	if( paramAutoHandles ) paramAutoHandles->Allocate(paramCount, false);

	n = paramList->firstChild;
	while( n )
	{
		asETypeModifiers inOutFlags;
		asCDataType type = CreateDataTypeFromNode(n, &source, objType ? objType->nameSpace : ns, false, objType);
		type = ModifyDataTypeFromNode(type, n->next, &source, &inOutFlags, &autoHandle);

		// Reference types cannot be passed by value to system functions
		if( isSystemFunction &&
			(type.GetObjectType() &&
		     (type.GetObjectType()->flags & asOBJ_REF)) &&
			!(type.IsReference() ||
			  type.IsObjectHandle()) )
			return asINVALID_DECLARATION;

		// Store the parameter type
		func->parameterTypes.PushLast(type);
		func->inOutFlags.PushLast(inOutFlags);

		// Don't permit void parameters
		if( type.GetTokenType() == ttVoid )
			return asINVALID_DECLARATION;

		if( autoHandle && (!type.IsObjectHandle() || type.IsReference()) )
			return asINVALID_DECLARATION;

		if( paramAutoHandles ) paramAutoHandles->PushLast(autoHandle);

		// Make sure that var type parameters are references
		if( type.GetTokenType() == ttQuestion &&
			!type.IsReference() )
			return asINVALID_DECLARATION;

		// Move to next parameter
		n = n->next->next;
		if( n && n->nodeType == snIdentifier )
			n = n->next;

		if( n && n->nodeType == snExpression )
		{
			// Strip out white space and comments to better share the string
			asCString *defaultArgStr = asNEW(asCString);
			if( defaultArgStr )
			{
				*defaultArgStr = GetCleanExpressionString(n, &source);
				func->defaultArgs.PushLast(defaultArgStr);
			}

			n = n->next;
		}
		else
			func->defaultArgs.PushLast(0);
	}

	// Set the read-only flag if const is declared after parameter list
	if( node->lastChild->nodeType == snUndefined && node->lastChild->tokenType == ttConst )
	{
		if( objType == 0 )
			return asINVALID_DECLARATION;
		func->isReadOnly = true;
	}
	else
		func->isReadOnly = false;

	// Make sure the default args are declared correctly
	ValidateDefaultArgs(&source, node, func);

	if( numErrors > 0 || numWarnings > 0 )
		return asINVALID_DECLARATION;

	return 0;
}

int asCBuilder::ParseVariableDeclaration(const char *decl, asSNameSpace *implicitNamespace, asCString &outName, asSNameSpace *&outNamespace, asCDataType &outDt)
{
	Reset();

	asCScriptCode source;
	source.SetCode(TXT_VARIABLE_DECL, decl, true);

	asCParser parser(this);

	int r = parser.ParsePropertyDeclaration(&source);
	if( r < 0 )
		return asINVALID_DECLARATION;

	asCScriptNode *node = parser.GetScriptNode();

	// Determine the scope from declaration
	asCScriptNode *n = node->firstChild->next;
	outNamespace = GetNameSpaceFromNode(n, &source, implicitNamespace, &n);
	if( outNamespace == 0 )
		return asINVALID_DECLARATION;

	// Find name
	outName.Assign(&source.code[n->tokenPos], n->tokenLength);

	// Initialize a script variable object for registration
	outDt = CreateDataTypeFromNode(node->firstChild, &source, implicitNamespace);

	if( numErrors > 0 || numWarnings > 0 )
		return asINVALID_DECLARATION;

	return 0;
}

int asCBuilder::CheckNameConflictMember(asCObjectType *t, const char *name, asCScriptNode *node, asCScriptCode *code, bool isProperty)
{
	// It's not necessary to check against object types

	// TODO: optimize: Improve linear search
	asCArray<asCObjectProperty *> &props = t->properties;
	for( asUINT n = 0; n < props.GetLength(); n++ )
	{
		if( props[n]->name == name )
		{
			if( code )
			{
				asCString str;
				str.Format(TXT_NAME_CONFLICT_s_OBJ_PROPERTY, name);
				WriteError(str, code, node);
			}

			return -1;
		}
	}

	// Property names must be checked against method names
	if( isProperty )
	{
		asCArray<int> methods = t->methods;
		for( asUINT n = 0; n < methods.GetLength(); n++ )
		{
			if( engine->scriptFunctions[methods[n]]->name == name )
			{
				if( code )
				{
					asCString str;
					str.Format(TXT_NAME_CONFLICT_s_METHOD, name);
					WriteError(str, code, node);
				}

				return -1;
			}
		}
	}

	return 0;
}

int asCBuilder::CheckNameConflict(const char *name, asCScriptNode *node, asCScriptCode *code, asSNameSpace *ns)
{
	// Check against registered object types
	if( engine->GetObjectType(name, ns) != 0 )
	{
		if( code )
		{
			asCString str;
			str.Format(TXT_NAME_CONFLICT_s_EXTENDED_TYPE, name);
			WriteError(str, code, node);
		}

		return -1;
	}

	// TODO: Must verify global properties in all config groups, whether the module has access or not
	// Check against global properties
	asCGlobalProperty *prop = GetGlobalProperty(name, ns, 0, 0, 0, 0);
	if( prop )
	{
		if( code )
		{
			asCString str;
			str.Format(TXT_NAME_CONFLICT_s_GLOBAL_PROPERTY, name);
			WriteError(str, code, node);
		}

		return -1;
	}

	// TODO: Property names must be checked against function names

#ifndef AS_NO_COMPILER
	// Check against class types
	asUINT n;
	for( n = 0; n < classDeclarations.GetLength(); n++ )
	{
		if( classDeclarations[n]->name == name &&
			classDeclarations[n]->objType->nameSpace == ns )
		{
			if( code )
			{
				asCString str;
				str.Format(TXT_NAME_CONFLICT_s_STRUCT, name);
				WriteError(str, code, node);
			}

			return -1;
		}
	}

	// Check against named types
	for( n = 0; n < namedTypeDeclarations.GetLength(); n++ )
	{
		if( namedTypeDeclarations[n]->name == name &&
			namedTypeDeclarations[n]->objType->nameSpace == ns )
		{
			if( code )
			{
				asCString str;
				str.Format(TXT_NAME_CONFLICT_s_IS_NAMED_TYPE, name);
				WriteError(str, code, node);
			}

			return -1;
		}
	}

	// Must check for name conflicts with funcdefs
	for( n = 0; n < funcDefs.GetLength(); n++ )
	{
		if( funcDefs[n]->name == name &&
			module->funcDefs[funcDefs[n]->idx]->nameSpace == ns )
		{
			if( code )
			{
				asCString str;
				str.Format(TXT_NAME_CONFLICT_s_IS_FUNCDEF, name);
				WriteError(str, code, node);
			}

			return -1;
		}
	}

	// Check against mixin classes
	if( GetMixinClass(name, ns) )
	{
		if( code )
		{
			asCString str;
			str.Format(TXT_NAME_CONFLICT_s_IS_MIXIN, name);
			WriteError(str, code, node);
		}

		return -1;
	}
#endif

	return 0;
}

#ifndef AS_NO_COMPILER
sMixinClass *asCBuilder::GetMixinClass(const char *name, asSNameSpace *ns)
{
	for( asUINT n = 0; n < mixinClasses.GetLength(); n++ )
		if( mixinClasses[n]->name == name &&
			mixinClasses[n]->ns == ns )
			return mixinClasses[n];

	return 0;
}

int asCBuilder::RegisterFuncDef(asCScriptNode *node, asCScriptCode *file, asSNameSpace *ns)
{
	// Find the name
	asASSERT( node->firstChild->nodeType == snDataType );
	asCScriptNode *n = node->firstChild->next->next;

	asCString name;
	name.Assign(&file->code[n->tokenPos], n->tokenLength);

	// Check for name conflict with other types
	int r = CheckNameConflict(name.AddressOf(), node, file, ns);
	if( asSUCCESS != r )
	{
		node->Destroy(engine);
		return r;
	}

	// The function definition should be stored as a asCScriptFunction so that the application
	// can use the asIScriptFunction interface to enumerate the return type and parameters

	// The return type and parameter types aren't determined in this function. A second pass is
	// necessary after all type declarations have been identified.

	sFuncDef *fd = asNEW(sFuncDef);
	if( fd == 0 )
	{
		node->Destroy(engine);
		return asOUT_OF_MEMORY;
	}

	fd->name   = name;
	fd->node   = node;
	fd->script = file;
	fd->idx    = module->AddFuncDef(name, ns);

	funcDefs.PushLast(fd);

	return 0;
}

void asCBuilder::CompleteFuncDef(sFuncDef *funcDef)
{
	asCDataType                returnType;
	asCArray<asCString>        parameterNames;
	asCArray<asCDataType>      parameterTypes;
	asCArray<asETypeModifiers> inOutFlags;
	asCArray<asCString *>      defaultArgs;
	bool                       isConstMethod;
	bool                       isConstructor;
	bool                       isDestructor;
	bool                       isPrivate;
	bool                       isOverride;
	bool                       isFinal;
	bool                       isShared;

	GetParsedFunctionDetails(funcDef->node, funcDef->script, 0, funcDef->name, returnType, parameterNames, parameterTypes, inOutFlags, defaultArgs, isConstMethod, isConstructor, isDestructor, isPrivate, isOverride, isFinal, isShared);

	asCScriptFunction *func = module->funcDefs[funcDef->idx];
	if( func )
	{
		func->returnType = returnType;
		for( asUINT p = 0; p < parameterTypes.GetLength(); p++ )
		{
			func->parameterTypes.PushLast(parameterTypes[p]);
			func->inOutFlags.PushLast(inOutFlags[p]);

			// Don't copy the default arg expression as it is not allowed for function definitions
		}
	}
}

int asCBuilder::RegisterGlobalVar(asCScriptNode *node, asCScriptCode *file, asSNameSpace *ns)
{
	// Has the application disabled global vars?
	if( engine->ep.disallowGlobalVars )
		WriteError(TXT_GLOBAL_VARS_NOT_ALLOWED, file, node);

	// What data type is it?
	// TODO: namespace: Use correct implicit namespace
	asCDataType type = CreateDataTypeFromNode(node->firstChild, file, engine->nameSpaces[0]);

	if( !type.CanBeInstanciated() )
	{
		asCString str;
		// TODO: Change to "'type' cannot be declared as variable"
		str.Format(TXT_DATA_TYPE_CANT_BE_s, type.Format().AddressOf());

		int r, c;
		file->ConvertPosToRowCol(node->tokenPos, &r, &c);

		WriteError(str, file, node);
	}

	asCScriptNode *n = node->firstChild->next;

	while( n )
	{
		// Verify that the name isn't taken
		asCString name(&file->code[n->tokenPos], n->tokenLength);
		CheckNameConflict(name.AddressOf(), n, file, ns);

		// Register the global variable
		sGlobalVariableDescription *gvar = asNEW(sGlobalVariableDescription);
		if( gvar == 0 )
		{
			node->Destroy(engine);
			return asOUT_OF_MEMORY;
		}

		gvar->script      = file;
		gvar->name        = name;
		gvar->isCompiled  = false;
		gvar->datatype    = type;
		gvar->isEnumValue = false;

		// TODO: Give error message if wrong
		asASSERT(!gvar->datatype.IsReference());

		gvar->idNode = n;
		gvar->nextNode = 0;
		if( n->next &&
			(n->next->nodeType == snAssignment ||
			 n->next->nodeType == snArgList    ||
			 n->next->nodeType == snInitList     ) )
		{
			gvar->nextNode = n->next;
			n->next->DisconnectParent();
		}

		gvar->property = module->AllocateGlobalProperty(name.AddressOf(), gvar->datatype, ns);
		gvar->index    = gvar->property->id;

		globVariables.Put(gvar);

		n = n->next;
	}

	node->Destroy(engine);

	return 0;
}

int asCBuilder::RegisterMixinClass(asCScriptNode *node, asCScriptCode *file, asSNameSpace *ns)
{
	asCScriptNode *cl = node->firstChild;
	asASSERT( cl->nodeType == snClass );

	asCScriptNode *n = cl->firstChild;

	// Skip potential 'final' and 'shared' tokens
	while( n->tokenType == ttIdentifier &&
		   (file->TokenEquals(n->tokenPos, n->tokenLength, FINAL_TOKEN) ||
		    file->TokenEquals(n->tokenPos, n->tokenLength, SHARED_TOKEN)) )
	{
		// Report error, because mixin class cannot be final or shared
		asCString msg;
		msg.Format(TXT_MIXIN_CANNOT_BE_DECLARED_AS_s, asCString(&file->code[n->tokenPos], n->tokenLength).AddressOf());
		WriteError(msg, file, n);

		asCScriptNode *tmp = n;
		n = n->next;

		// Remove the invalid node, so compilation can continue as if it wasn't there
		tmp->DisconnectParent();
		tmp->Destroy(engine);
	}

	asCString name(&file->code[n->tokenPos], n->tokenLength);

	int r, c;
	file->ConvertPosToRowCol(n->tokenPos, &r, &c);

	CheckNameConflict(name.AddressOf(), n, file, ns);

	sMixinClass *decl = asNEW(sMixinClass);
	if( decl == 0 )
	{
		node->Destroy(engine);
		return asOUT_OF_MEMORY;
	}

	mixinClasses.PushLast(decl);
	decl->name   = name;
	decl->ns     = ns;
	decl->node   = cl;
	decl->script = file;

	// Clean up memory
	cl->DisconnectParent();
	node->Destroy(engine);

	return 0;
}

int asCBuilder::RegisterClass(asCScriptNode *node, asCScriptCode *file, asSNameSpace *ns)
{
	asCScriptNode *n = node->firstChild;
	bool isFinal = false;
	bool isShared = false;

	if( n->tokenType == ttIdentifier && file->TokenEquals(n->tokenPos, n->tokenLength, FINAL_TOKEN) )
	{
		isFinal = true;
		n = n->next;
	}

	if( n->tokenType == ttIdentifier && file->TokenEquals(n->tokenPos, n->tokenLength, SHARED_TOKEN) )
	{
		isShared = true;
		n = n->next;

		// Check for final again
		if( n->tokenType == ttIdentifier && file->TokenEquals(n->tokenPos, n->tokenLength, FINAL_TOKEN) )
		{
			isFinal = true;
			n = n->next;
		}
	}

	asCString name(&file->code[n->tokenPos], n->tokenLength);

	int r, c;
	file->ConvertPosToRowCol(n->tokenPos, &r, &c);

	CheckNameConflict(name.AddressOf(), n, file, ns);

	sClassDeclaration *decl = asNEW(sClassDeclaration);
	if( decl == 0 )
	{
		node->Destroy(engine);
		return asOUT_OF_MEMORY;
	}

	classDeclarations.PushLast(decl);
	decl->name             = name;
	decl->script           = file;
	decl->node             = node;

	// If this type is shared and there already exist another shared
	// type of the same name, then that one should be used instead of
	// creating a new one.
	if( isShared )
	{
		for( asUINT n = 0; n < engine->classTypes.GetLength(); n++ )
		{
			asCObjectType *st = engine->classTypes[n];
			if( st &&
				st->IsShared() &&
				st->name == name &&
				st->nameSpace == ns &&
				!st->IsInterface() )
			{
				// We'll use the existing type
				decl->isExistingShared = true;
				decl->objType          = st;
				module->classTypes.PushLast(st);
				st->AddRef();
				return 0;
			}
		}
	}

	// Create a new object type for this class
	asCObjectType *st = asNEW(asCObjectType)(engine);
	if( st == 0 )
		return asOUT_OF_MEMORY;

	st->flags = asOBJ_REF | asOBJ_SCRIPT_OBJECT;

	if( isShared )
		st->flags |= asOBJ_SHARED;

	if( isFinal )
		st->flags |= asOBJ_NOINHERIT;

	if( node->tokenType == ttHandle )
		st->flags |= asOBJ_IMPLICIT_HANDLE;

	st->size      = sizeof(asCScriptObject);
	st->name      = name;
	st->nameSpace = ns;
	st->module    = module;
	module->classTypes.PushLast(st);
	engine->classTypes.PushLast(st);
	st->AddRef();
	decl->objType = st;

	// Use the default script class behaviours
	st->beh = engine->scriptTypeBehaviours.beh;

	// TODO: Move this to asCObjectType so that the asCRestore can reuse it
	engine->scriptFunctions[st->beh.addref]->AddRef();
	engine->scriptFunctions[st->beh.release]->AddRef();
	engine->scriptFunctions[st->beh.gcEnumReferences]->AddRef();
	engine->scriptFunctions[st->beh.gcGetFlag]->AddRef();
	engine->scriptFunctions[st->beh.gcGetRefCount]->AddRef();
	engine->scriptFunctions[st->beh.gcReleaseAllReferences]->AddRef();
	engine->scriptFunctions[st->beh.gcSetFlag]->AddRef();
	engine->scriptFunctions[st->beh.copy]->AddRef();
	engine->scriptFunctions[st->beh.factory]->AddRef();
	engine->scriptFunctions[st->beh.construct]->AddRef();
	for( asUINT i = 1; i < st->beh.operators.GetLength(); i += 2 )
		engine->scriptFunctions[st->beh.operators[i]]->AddRef();

	return 0;
}

int asCBuilder::RegisterInterface(asCScriptNode *node, asCScriptCode *file, asSNameSpace *ns)
{
	asCScriptNode *n = node->firstChild;
	asCString name(&file->code[n->tokenPos], n->tokenLength);

	bool isShared = false;
	if( name == SHARED_TOKEN )
	{
		isShared = true;

		n = n->next;
		name.Assign(&file->code[n->tokenPos], n->tokenLength);
	}

	int r, c;
	file->ConvertPosToRowCol(n->tokenPos, &r, &c);

	CheckNameConflict(name.AddressOf(), n, file, ns);

	sClassDeclaration *decl = asNEW(sClassDeclaration);
	if( decl == 0 )
	{
		node->Destroy(engine);
		return asOUT_OF_MEMORY;
	}

	interfaceDeclarations.PushLast(decl);
	decl->name             = name;
	decl->script           = file;
	decl->node             = node;

	// If this type is shared and there already exist another shared
	// type of the same name, then that one should be used instead of
	// creating a new one.
	if( isShared )
	{
		for( asUINT n = 0; n < engine->classTypes.GetLength(); n++ )
		{
			asCObjectType *st = engine->classTypes[n];
			if( st &&
				st->IsShared() &&
				st->name == name &&
				st->nameSpace == ns &&
				st->IsInterface() )
			{
				// We'll use the existing type
				decl->isExistingShared = true;
				decl->objType          = st;
				module->classTypes.PushLast(st);
				st->AddRef();
				return 0;
			}
		}
	}

	// Register the object type for the interface
	asCObjectType *st = asNEW(asCObjectType)(engine);
	if( st == 0 )
		return asOUT_OF_MEMORY;

	st->flags = asOBJ_REF | asOBJ_SCRIPT_OBJECT;

	if( isShared )
		st->flags |= asOBJ_SHARED;

	st->size = 0; // Cannot be instanciated
	st->name = name;
	st->nameSpace = ns;
	module->classTypes.PushLast(st);
	engine->classTypes.PushLast(st);
	st->AddRef();
	decl->objType = st;

	// Use the default script class behaviours
	st->beh.construct = 0;
	st->beh.addref = engine->scriptTypeBehaviours.beh.addref;
	engine->scriptFunctions[st->beh.addref]->AddRef();
	st->beh.release = engine->scriptTypeBehaviours.beh.release;
	engine->scriptFunctions[st->beh.release]->AddRef();
	st->beh.copy = 0;

	return 0;
}

void asCBuilder::CompileGlobalVariables()
{
	bool compileSucceeded = true;

	// Store state of compilation (errors, warning, output)
	int currNumErrors   = numErrors;
	int currNumWarnings = numWarnings;

	// Backup the original message stream
	bool                       msgCallback     = engine->msgCallback;
	asSSystemFunctionInterface msgCallbackFunc = engine->msgCallbackFunc;
	void                      *msgCallbackObj  = engine->msgCallbackObj;

	// Set the new temporary message stream
	asCOutputBuffer outBuffer;
	engine->SetMessageCallback(asMETHOD(asCOutputBuffer, Callback), &outBuffer, asCALL_THISCALL);

	asCOutputBuffer finalOutput;
	asCScriptFunction *initFunc = 0;

	asCSymbolTable<asCGlobalProperty> initOrder;

	// We first try to compile all the primitive global variables, and only after that
	// compile the non-primitive global variables. This permits the constructors
	// for the complex types to use the already initialized variables of primitive
	// type. Note, we currently don't know which global variables are used in the
	// constructors, so we cannot guarantee that variables of complex types are
	// initialized in the correct order, so we won't reorder those.
	bool compilingPrimitives = true;

	// Compile each global variable
	while( compileSucceeded )
	{
		compileSucceeded = false;

		int accumErrors = 0;
		int accumWarnings = 0;

		// Restore state of compilation
		finalOutput.Clear();
		asCSymbolTable<sGlobalVariableDescription>::iterator it = globVariables.List();
		for( ; it; it++ )
		{
			sGlobalVariableDescription *gvar = *it;
			if( gvar->isCompiled )
				continue;

			asCByteCode init(engine);
			numWarnings = 0;
			numErrors = 0;
			outBuffer.Clear();

			// Skip this for now if we're not compiling complex types yet
			if( compilingPrimitives && !gvar->datatype.IsPrimitive() )
				continue;

			if( gvar->nextNode )
			{
				int r, c;
				gvar->script->ConvertPosToRowCol(gvar->nextNode->tokenPos, &r, &c);
				asCString str = gvar->datatype.Format();
				str += " " + gvar->name;
				str.Format(TXT_COMPILING_s, str.AddressOf());
				WriteInfo(gvar->script->name, str, r, c, true);
			}

			if( gvar->isEnumValue )
			{
				int r;
				if( gvar->nextNode )
				{
					asCCompiler comp(engine);
					asCScriptFunction func(engine, module, asFUNC_DUMMY);

					// Set the namespace that should be used during the compilation
					func.nameSpace = gvar->datatype.GetObjectType()->nameSpace;

					// Temporarily switch the type of the variable to int so it can be compiled properly
					asCDataType saveType;
					saveType = gvar->datatype;
					gvar->datatype = asCDataType::CreatePrimitive(ttInt, true);
					r = comp.CompileGlobalVariable(this, gvar->script, gvar->nextNode, gvar, &func);
					gvar->datatype = saveType;
				}
				else
				{
					r = 0;

					// When there is no assignment the value is the last + 1
					int enumVal = 0;
					asCSymbolTable<sGlobalVariableDescription>::iterator prev_it = it;
					prev_it--;
					if( prev_it )
					{
						sGlobalVariableDescription *gvar2 = *prev_it;
						if(gvar2->datatype == gvar->datatype )
						{
							// The integer value is stored in the lower bytes
							enumVal = (*(int*)&gvar2->constantValue) + 1;

							if( !gvar2->isCompiled )
							{
								// TODO: Need to get the correct script position
								int row, col;
								gvar->script->ConvertPosToRowCol(0, &row, &col);

								asCString str = gvar->datatype.Format();
								str += " " + gvar->name;
								str.Format(TXT_COMPILING_s, str.AddressOf());
								WriteInfo(gvar->script->name, str, row, col, true);

								str.Format(TXT_UNINITIALIZED_GLOBAL_VAR_s, gvar2->name.AddressOf());
								WriteError(gvar->script->name, str, row, col);
								r = -1;
							}
						}
					}

					// The integer value is stored in the lower bytes
					*(int*)&gvar->constantValue = enumVal;
				}

				if( r >= 0 )
				{
					// Set the value as compiled
					gvar->isCompiled = true;
					compileSucceeded = true;
				}
			}
			else
			{
				// Compile the global variable
				initFunc = asNEW(asCScriptFunction)(engine, module, asFUNC_DUMMY);
				if( initFunc == 0 )
				{
					// Out of memory
					return;
				}

				// Set the namespace that should be used for this function
				initFunc->nameSpace = gvar->property->nameSpace;

				asCCompiler comp(engine);
				int r = comp.CompileGlobalVariable(this, gvar->script, gvar->nextNode, gvar, initFunc);
				if( r >= 0 )
				{
					// Compilation succeeded
					gvar->isCompiled = true;
					compileSucceeded = true;
				}
				else
				{
					// Compilation failed
					asDELETE(initFunc, asCScriptFunction);
					initFunc = 0;
				}
			}

			if( gvar->isCompiled )
			{
				// Add warnings for this constant to the total build
				if( numWarnings )
				{
					currNumWarnings += numWarnings;
					if( msgCallback )
						outBuffer.SendToCallback(engine, &msgCallbackFunc, msgCallbackObj);
				}

				// Determine order of variable initializations
				if( gvar->property && !gvar->isEnumValue )
					initOrder.Put(gvar->property);

				// Does the function contain more than just a SUSPEND followed by a RET instruction?
				if( initFunc && initFunc->byteCode.GetLength() > 2 )
				{
					// Create the init function for this variable
					initFunc->id = engine->GetNextScriptFunctionId();
					engine->SetScriptFunction(initFunc);

					// Finalize the init function for this variable
					initFunc->funcType = asFUNC_SCRIPT;
					initFunc->returnType = asCDataType::CreatePrimitive(ttVoid, false);
					initFunc->scriptSectionIdx = engine->GetScriptSectionNameIndex(gvar->script->name.AddressOf());

					gvar->property->SetInitFunc(initFunc);

					initFunc->Release();
					initFunc = 0;
				}
				else if( initFunc )
				{
					// Destroy the function as it won't be used
					asDELETE(initFunc, asCScriptFunction);
					initFunc = 0;
				}

				// Convert enums to true enum values, so subsequent compilations can access it as an enum
				if( gvar->isEnumValue )
				{
					asCObjectType *objectType = gvar->datatype.GetObjectType();
					asASSERT(NULL != objectType);

					asSEnumValue *e = asNEW(asSEnumValue);
					if( e == 0 )
					{
						// Out of memory
						numErrors++;
						return;
					}

					e->name = gvar->name;
					e->value = *(int*)&gvar->constantValue;

					objectType->enumValues.PushLast(e);
				}
			}
			else
			{
				// Add output to final output
				finalOutput.Append(outBuffer);
				accumErrors += numErrors;
				accumWarnings += numWarnings;
			}

			preMessage.isSet = false;
		}

		if( !compileSucceeded )
		{
			if( compilingPrimitives )
			{
				// No more primitives could be compiled, so
				// switch to compiling the complex variables
				compilingPrimitives = false;
				compileSucceeded    = true;
			}
			else
			{
				// No more variables can be compiled
				// Add errors and warnings to total build
				currNumWarnings += accumWarnings;
				currNumErrors   += accumErrors;
				if( msgCallback )
					finalOutput.SendToCallback(engine, &msgCallbackFunc, msgCallbackObj);
			}
		}
	}

	// Restore states
	engine->msgCallback     = msgCallback;
	engine->msgCallbackFunc = msgCallbackFunc;
	engine->msgCallbackObj  = msgCallbackObj;

	numWarnings = currNumWarnings;
	numErrors   = currNumErrors;

	// Set the correct order of initialization
	if( numErrors == 0 )
	{
		// If the length of the arrays are not the same, then this is the compilation
		// of a single variable, in which case the initialization order of the previous
		// variables must be preserved.
		if( module->scriptGlobals.GetSize() == initOrder.GetSize() )
			module->scriptGlobals.SwapWith(initOrder);
	}

	// Delete the enum expressions
	asCSymbolTableIterator<sGlobalVariableDescription> it = globVariables.List();
	while( it )
	{
		sGlobalVariableDescription *gvar = *it;
		if( gvar->isEnumValue )
		{
			// Remove from symboltable. This has to be done prior to freeing the memeory
			globVariables.Erase(it.GetIndex());

			// Destroy the gvar property
			if( gvar->nextNode )
			{
				gvar->nextNode->Destroy(engine);
				gvar->nextNode = 0;
			}
			if( gvar->property )
			{
				asDELETE(gvar->property, asCGlobalProperty);
				gvar->property = 0;
			}

			asDELETE(gvar, sGlobalVariableDescription);
		}
		it++;
	}
}

int asCBuilder::GetNamespaceAndNameFromNode(asCScriptNode *n, asCScriptCode *script, asSNameSpace *implicitNs, asSNameSpace *&outNs, asCString &outName)
{
	asASSERT( n->nodeType == snIdentifier );

	// Get the optional scope from the node
	asSNameSpace *ns = GetNameSpaceFromNode(n->firstChild, script, implicitNs, 0);
	if( ns == 0 )
		return -1;

	// Get the name
	asCString name(&script->code[n->lastChild->tokenPos], n->lastChild->tokenLength);

	outNs = ns;
	outName = name;

	return 0;
}

void asCBuilder::AddInterfaceFromMixinToClass(sClassDeclaration *decl, asCScriptNode *errNode, sMixinClass *mixin)
{
	// Determine what interfaces that the mixin implements
	asCScriptNode *node = mixin->node;
	asASSERT(node->nodeType == snClass);

	// Skip the name of the mixin
	node = node->firstChild->next;


	while( node && node->nodeType == snIdentifier )
	{
		bool ok = true;
		asSNameSpace *ns;
		asCString name;
		if( GetNamespaceAndNameFromNode(node, mixin->script, mixin->ns, ns, name) < 0 )
			ok = false;
		else
		{
			// Find the object type for the interface
			asCObjectType *objType = GetObjectType(name.AddressOf(), ns);

			// Check that the object type is an interface
			if( objType && objType->size == 0 && (objType->flags & asOBJ_SCRIPT_OBJECT) )
			{
				// Only add the interface if the class doesn't already implement it
				if( !decl->objType->Implements(objType) )
					AddInterfaceToClass(decl, errNode, objType);
			}
			else
			{
				WriteError(TXT_MIXIN_CLASS_CANNOT_INHERIT, mixin->script, node);
				ok = false;
			}
		}

		if( !ok )
		{
			// Remove this node so the error isn't reported again
			asCScriptNode *delNode = node;
			node = node->prev;
			delNode->DisconnectParent();
			delNode->Destroy(engine);
		}

		node = node->next;
	}
}

void asCBuilder::AddInterfaceToClass(sClassDeclaration *decl, asCScriptNode *errNode, asCObjectType *intfType)
{
	// If the interface is already in the class, then don't add it again
	if( !decl->isExistingShared && decl->objType->Implements(intfType) )
	{
		int r, c;
		decl->script->ConvertPosToRowCol(errNode->tokenPos, &r, &c);
		asCString msg;
		msg.Format(TXT_INTERFACE_s_ALREADY_IMPLEMENTED, intfType->GetName());
		WriteWarning(decl->script->name, msg, r, c);
		return;
	}

	// A shared type may only implement from shared interfaces
	if( decl->objType->IsShared() && !intfType->IsShared() )
	{
		asCString msg;
		msg.Format(TXT_SHARED_CANNOT_IMPLEMENT_NON_SHARED_s, intfType->name.AddressOf());
		WriteError(msg, decl->script, errNode);
		return;
	}

	// If the class is an existing shared class, then just check if the 
	// interface exists in the original declaration too
	if( decl->isExistingShared && !decl->objType->Implements(intfType) )
	{
		asCString str;
		str.Format(TXT_SHARED_s_DOESNT_MATCH_ORIGINAL, decl->objType->GetName());
		WriteError(str, decl->script, errNode);
		return;
	}
	
	// Add the interface to the class	
	decl->objType->interfaces.PushLast(intfType);

	// Add the inherited interfaces too
	// For interfaces this will be done outside to handle out-of-order declarations
	if( !decl->objType->IsInterface() )
	{
		for( asUINT n = 0; n < intfType->interfaces.GetLength(); n++ )
			AddInterfaceToClass(decl, errNode, intfType->interfaces[n]);
	}
}

void asCBuilder::CompileInterfaces()
{
	asUINT n;
	for( n = 0; n < interfaceDeclarations.GetLength(); n++ )
	{
		sClassDeclaration *intfDecl = interfaceDeclarations[n];
		asCObjectType *intfType = intfDecl->objType;

		asCScriptNode *node = intfDecl->node;
		asASSERT(node && node->nodeType == snInterface);
		node = node->firstChild;

		// Skip the 'shared' keyword
		if( intfType->IsShared() )
			node = node->next;

		// Skip the name
		node = node->next;

		// Verify the inherited interfaces
		while( node && node->nodeType == snIdentifier )
		{
			asSNameSpace *ns;
			asCString name;
			if( GetNamespaceAndNameFromNode(node, intfDecl->script, intfType->nameSpace, ns, name) < 0 )
			{
				node = node->next;
				continue;
			}

			// Find the object type for the interface
			asCObjectType *objType = GetObjectType(name.AddressOf(), ns);

			// Check that the object type is an interface
			bool ok = true;
			if( objType && objType->size == 0 && (objType->flags & asOBJ_SCRIPT_OBJECT) )
			{
				// Check that the implemented interface is shared if the base interface is shared
				if( intfType->IsShared() && !objType->IsShared() )
				{
					asCString str;
					str.Format(TXT_SHARED_CANNOT_IMPLEMENT_NON_SHARED_s, objType->GetName());
					WriteError(str, intfDecl->script, node);
					ok = false;
				}
			}
			else
			{
				WriteError(TXT_INTERFACE_CAN_ONLY_IMPLEMENT_INTERFACE, intfDecl->script, node);
				ok = false;
			}
	
			if( ok )
			{
				// Make sure none of the implemented interfaces implement from this one
				asCObjectType *base = objType;
				while( base != 0 )
				{
					if( base == intfType )
					{
						WriteError(TXT_CANNOT_IMPLEMENT_SELF, intfDecl->script, node);
						ok = false;
						break;
					}

					// At this point there is at most one implemented interface
					if( base->interfaces.GetLength() )
						base = base->interfaces[0];
					else
						break;
				}
			}

			if( ok )
				AddInterfaceToClass(intfDecl, node, objType);

			// Remove the nodes so they aren't parsed again
			asCScriptNode *delNode = node;
			node = node->next;
			delNode->DisconnectParent();
			delNode->Destroy(engine);
		}
	}

	// Order the interfaces with inheritances so that the inherited 
	// of inherited interfaces can be added properly
	for( n = 0; n < interfaceDeclarations.GetLength(); n++ )
	{
		sClassDeclaration *intfDecl = interfaceDeclarations[n];
		asCObjectType *intfType = intfDecl->objType;

		if( intfType->interfaces.GetLength() == 0 ) continue;

		// If any of the derived interfaces are found after this interface, then move this to the end of the list
		for( asUINT m = n+1; m < interfaceDeclarations.GetLength(); m++ )
		{
			if( intfType->Implements(interfaceDeclarations[m]->objType) )
			{
				interfaceDeclarations.RemoveIndex(n);
				interfaceDeclarations.PushLast(intfDecl);

				// Decrease index so that we don't skip an entry
				n--;
				break;
			}
		}
	}

	// Now recursively add the additional inherited interfaces
	for( n = 0; n < interfaceDeclarations.GetLength(); n++ )
	{
		sClassDeclaration *intfDecl = interfaceDeclarations[n];
		asCObjectType *intfType = intfDecl->objType;

		// As new interfaces will be added to the end of the list, all 
		// interfaces will be traversed the same as recursively
		for( asUINT m = 0; m < intfType->interfaces.GetLength(); m++ )
		{
			asCObjectType *base = intfType->interfaces[m];
			for( asUINT l = 0; l < base->interfaces.GetLength(); l++ )
				AddInterfaceToClass(intfDecl, intfDecl->node, base->interfaces[l]);
		}
	}
}

void asCBuilder::CompileClasses()
{
	asUINT n;
	asCArray<sClassDeclaration*> toValidate((int)classDeclarations.GetLength());

	// Determine class inheritances and interfaces
	for( n = 0; n < classDeclarations.GetLength(); n++ )
	{
		sClassDeclaration *decl = classDeclarations[n];
		asCScriptCode *file = decl->script;

		// Find the base class that this class inherits from
		bool multipleInheritance = false;
		asCScriptNode *node = decl->node->firstChild;

		if( decl->objType->IsShared() )
		{
			// Skip the keyword 'shared'
			asASSERT(node->tokenType == ttIdentifier);
			node = node->next;
		}
		if( decl->objType->flags & asOBJ_NOINHERIT )
		{
			// skip the keyword 'final'
			asASSERT(node->tokenType == ttIdentifier);
			node = node->next;
		}

		// Skip the name of the class
		asASSERT(node->tokenType == ttIdentifier);
		node = node->next;

		while( node && node->nodeType == snIdentifier )
		{
			asSNameSpace *ns;
			asCString name;
			if( GetNamespaceAndNameFromNode(node, file, decl->objType->nameSpace, ns, name) < 0 )
			{
				node = node->next;
				continue;
			}

			// Find the object type for the interface
			asCObjectType *objType = GetObjectType(name.AddressOf(), ns);

			if( objType == 0 )
			{
				// Check if the name is a mixin class
				sMixinClass *mixin = GetMixinClass(name.AddressOf(), ns);
				if( !mixin )
				{
					asCString str;
					str.Format(TXT_IDENTIFIER_s_NOT_DATA_TYPE, name.AddressOf());
					WriteError(str, file, node);
				}
				else
					AddInterfaceFromMixinToClass(decl, node, mixin);
			}
			else if( !(objType->flags & asOBJ_SCRIPT_OBJECT) ||
					 objType->flags & asOBJ_NOINHERIT )
			{
				// Either the class is not a script class or interface
				// or the class has been declared as 'final'
				asCString str;
				str.Format(TXT_CANNOT_INHERIT_FROM_s_FINAL, objType->name.AddressOf());
				WriteError(str, file, node);
			}
			else if( objType->size != 0 )
			{
				// The class inherits from another script class
				if( !decl->isExistingShared && decl->objType->derivedFrom != 0 )
				{
					if( !multipleInheritance )
					{
						WriteError(TXT_CANNOT_INHERIT_FROM_MULTIPLE_CLASSES, file, node);
						multipleInheritance = true;
					}
				}
				else
				{
					// Make sure none of the base classes inherit from this one
					asCObjectType *base = objType;
					bool error = false;
					while( base != 0 )
					{
						if( base == decl->objType )
						{
							WriteError(TXT_CANNOT_INHERIT_FROM_SELF, file, node);
							error = true;
							break;
						}

						base = base->derivedFrom;
					}

					if( !error )
					{
						// A shared type may only inherit from other shared types
						if( (decl->objType->IsShared()) && !(objType->IsShared()) )
						{
							asCString msg;
							msg.Format(TXT_SHARED_CANNOT_INHERIT_FROM_NON_SHARED_s, objType->name.AddressOf());
							WriteError(msg, file, node);
							error = true;
						}
					}

					if( !error )
					{
						if( decl->isExistingShared )
						{
							// Verify that the base class is the same as the original shared type
							if( decl->objType->derivedFrom != objType )
							{
								asCString str;
								str.Format(TXT_SHARED_s_DOESNT_MATCH_ORIGINAL, decl->objType->GetName());
								WriteError(str, file, node);
							}
						}
						else
						{
							// Set the base class
							decl->objType->derivedFrom = objType;
							objType->AddRef();
						}
					}
				}
			}
			else
			{
				// The class implements an interface
				AddInterfaceToClass(decl, node, objType);
			}

			node = node->next;
		}
	}

	// Order class declarations so that base classes are compiled before derived classes.
	// This will allow the derived classes to copy properties and methods in the next step.
	for( n = 0; n < classDeclarations.GetLength(); n++ )
	{
		sClassDeclaration *decl = classDeclarations[n];
		asCObjectType *derived = decl->objType;
		asCObjectType *base = derived->derivedFrom;

		if( base == 0 ) continue;

		// If the base class is found after the derived class, then move the derived class to the end of the list
		for( asUINT m = n+1; m < classDeclarations.GetLength(); m++ )
		{
			sClassDeclaration *declBase = classDeclarations[m];
			if( base == declBase->objType )
			{
				classDeclarations.RemoveIndex(n);
				classDeclarations.PushLast(decl);

				// Decrease index so that we don't skip an entry
				n--;
				break;
			}
		}
	}

	// Go through each of the classes and register the object type descriptions
	for( n = 0; n < classDeclarations.GetLength(); n++ )
	{
		sClassDeclaration *decl = classDeclarations[n];
		if( decl->isExistingShared )
		{
			// Set the declaration as validated already, so that other
			// types that contain this will accept this type
			decl->validState = 1;

			// We'll still validate the declaration to make sure nothing new is 
			// added to the shared class that wasn't there in the previous 
			// compilation. We do not care if something that is there in the previous
			// declaration is not included in the new declaration though.
		}

		// Methods included from mixin classes should take precedence over inherited methods
		IncludeMethodsFromMixins(decl);

		// Add all properties and methods from the base class
		if( !decl->isExistingShared && decl->objType->derivedFrom )
		{
			asCObjectType *baseType = decl->objType->derivedFrom;

			// The derived class inherits all interfaces from the base class
			for( unsigned int n = 0; n < baseType->interfaces.GetLength(); n++ )
			{
				if( !decl->objType->Implements(baseType->interfaces[n]) )
				{
					decl->objType->interfaces.PushLast(baseType->interfaces[n]);
				}
				else
				{
					// Warn if derived class already implements the interface
					int r, c;
					decl->script->ConvertPosToRowCol(decl->node->tokenPos, &r, &c);
					asCString msg;
					msg.Format(TXT_INTERFACE_s_ALREADY_IMPLEMENTED, baseType->interfaces[n]->GetName());
					WriteWarning(decl->script->name, msg, r, c);
				}
			}

			// TODO: Need to check for name conflict with new class methods

			// Copy properties from base class to derived class
			for( asUINT p = 0; p < baseType->properties.GetLength(); p++ )
			{
				asCObjectProperty *prop = AddPropertyToClass(decl, baseType->properties[p]->name, baseType->properties[p]->type, baseType->properties[p]->isPrivate);

				// The properties must maintain the same offset
				asASSERT(prop && prop->byteOffset == baseType->properties[p]->byteOffset); UNUSED_VAR(prop);
			}

			// Copy methods from base class to derived class
			for( asUINT m = 0; m < baseType->methods.GetLength(); m++ )
			{
				// If the derived class implements the same method, then don't add the base class' method
				asCScriptFunction *baseFunc = GetFunctionDescription(baseType->methods[m]);
				asCScriptFunction *derivedFunc = 0;
				bool found = false;
				for( asUINT d = 0; d < decl->objType->methods.GetLength(); d++ )
				{
					derivedFunc = GetFunctionDescription(decl->objType->methods[d]);
					if( derivedFunc->IsSignatureEqual(baseFunc) )
					{
						if( baseFunc->IsFinal() )
						{
							asCString msg;
							msg.Format(TXT_METHOD_CANNOT_OVERRIDE_s, baseFunc->GetDeclaration());
							WriteError(msg, decl->script, decl->node);
						}

						// Move the function from the methods array to the virtualFunctionTable
						decl->objType->methods.RemoveIndex(d);
						decl->objType->virtualFunctionTable.PushLast(derivedFunc);
						found = true;
						break;
					}
				}

				if( !found )
				{
					// Push the base class function on the virtual function table
					decl->objType->virtualFunctionTable.PushLast(baseType->virtualFunctionTable[m]);
					baseType->virtualFunctionTable[m]->AddRef();
				}

				decl->objType->methods.PushLast(baseType->methods[m]);
				engine->scriptFunctions[baseType->methods[m]]->AddRef();
			}
		}

		// Move this class' methods into the virtual function table
		if( !decl->isExistingShared )
		{
			for( asUINT m = 0; m < decl->objType->methods.GetLength(); m++ )
			{
				asCScriptFunction *func = GetFunctionDescription(decl->objType->methods[m]);
				if( func->funcType != asFUNC_VIRTUAL )
				{
					// Move the reference from the method list to the virtual function list
					decl->objType->methods.RemoveIndex(m);
					decl->objType->virtualFunctionTable.PushLast(func);

					// Substitute the function description in the method list for a virtual method
					// Make sure the methods are in the same order as the virtual function table
					decl->objType->methods.PushLast(CreateVirtualFunction(func, (int)decl->objType->virtualFunctionTable.GetLength() - 1));
					m--;
				}
			}
		}

		// Enumerate each of the declared properties
		asCScriptNode *node = decl->node->firstChild->next;
		if( decl->objType->IsShared() )
			node = node->next;
		if( decl->objType->flags & asOBJ_NOINHERIT )
			node = node->next;

		// Skip list of classes and interfaces
		while( node && node->nodeType == snIdentifier )
			node = node->next;

		while( node )
		{
			if( node->nodeType == snDeclaration )
			{
				asCScriptNode *n = node->firstChild;

				// Is the property declared as private?
				bool isPrivate = false;
				if( n && n->tokenType == ttPrivate )
				{
					isPrivate = true;
					n = n->next;
				}

				// Determine the type of the property
				asCScriptCode *file = decl->script;
				// TODO: namespace: Use correct implicit namespace
				asCDataType dt = CreateDataTypeFromNode(n, file, engine->nameSpaces[0]);
				if( decl->objType->IsShared() && dt.GetObjectType() && !dt.GetObjectType()->IsShared() )
				{
					asCString msg;
					msg.Format(TXT_SHARED_CANNOT_USE_NON_SHARED_TYPE_s, dt.GetObjectType()->name.AddressOf());
					WriteError(msg, file, node);
				}

				if( dt.IsReadOnly() )
					WriteError(TXT_PROPERTY_CANT_BE_CONST, file, node);

				// Multiple properties can be declared separated by ,
				n = n->next;
				while( n )
				{
					asCString name(&file->code[n->tokenPos], n->tokenLength);

					if( !decl->isExistingShared )
					{
						CheckNameConflictMember(decl->objType, name.AddressOf(), n, file, true);
						AddPropertyToClass(decl, name, dt, isPrivate, file, node);

						// Register the initialization expression (if any) to be compiled later
						if( n->next && n->next->nodeType != snIdentifier )
						{
							n = n->next;
							asASSERT( n->nodeType == snAssignment );

							// Store the initialization node
							sPropertyInitializer p(name, n, file);
							decl->propInits.PushLast(p);
						}
					}
					else
					{
						// Verify that the property exists in the original declaration
						bool found = false;
						for( asUINT p = 0; p < decl->objType->properties.GetLength(); p++ )
						{
							asCObjectProperty *prop = decl->objType->properties[p];
							if( prop->isPrivate == isPrivate &&
								prop->name == name &&
								prop->type.IsEqualExceptRef(dt) )
							{
								found = true;
								break;
							}
						}
						if( !found )
						{
							asCString str;
							str.Format(TXT_SHARED_s_DOESNT_MATCH_ORIGINAL, decl->objType->GetName());
							WriteError(str, file, n);
						}

						// Skip the initialization expression
						if( n->next && n->next->nodeType != snIdentifier )
							n = n->next;
					}
					n = n->next;
				}
			}
			else
				asASSERT(false);

			node = node->next;
		}

		// Add properties from included mixin classes that don't conflict with existing properties
		IncludePropertiesFromMixins(decl);

		if( !decl->isExistingShared )
			toValidate.PushLast(decl);
	}

	// TODO: Warn if a method overrides a base method without marking it as 'override'.
	//       It must be possible to turn off this warning through engine property.

	// TODO: A base class should be able to mark a method as 'abstract'. This will
	//       allow a base class to provide a partial implementation, but still force
	//       derived classes to implement specific methods.

	// Verify that all interface methods are implemented in the classes
	// We do this here so the base class' methods have already been inherited
	for( n = 0; n < classDeclarations.GetLength(); n++ )
	{
		sClassDeclaration *decl = classDeclarations[n];
		if( decl->isExistingShared ) continue;

		asCArray<bool> overrideValidations(decl->objType->GetMethodCount());
		for( asUINT k = 0; k < decl->objType->methods.GetLength(); k++ )
		{
			overrideValidations.PushLast( !static_cast<asCScriptFunction*>(decl->objType->GetMethodByIndex(k, false))->IsOverride() );
		}

		for( asUINT m = 0; m < decl->objType->interfaces.GetLength(); m++ )
		{
			asCObjectType *objType = decl->objType->interfaces[m];
			for( asUINT i = 0; i < objType->methods.GetLength(); i++ )
			{
				asUINT overrideIndex;
				if( !DoesMethodExist(decl->objType, objType->methods[i], &overrideIndex) )
				{
					asCString str;
					str.Format(TXT_MISSING_IMPLEMENTATION_OF_s,
						engine->GetFunctionDeclaration(objType->methods[i]).AddressOf());
					WriteError(str, decl->script, decl->node);
				}
				else
					overrideValidations[overrideIndex] = true;
			}
		}

		bool hasBaseClass = decl->objType->derivedFrom != 0;

		for( asUINT j = 0; j < overrideValidations.GetLength(); j++ )
		{
			if( !overrideValidations[j] && (!hasBaseClass || !DoesMethodExist(decl->objType->derivedFrom, decl->objType->methods[j])) )
			{
				asCString msg;
				msg.Format(TXT_METHOD_s_DOES_NOT_OVERRIDE, decl->objType->GetMethodByIndex(j, false)->GetDeclaration());
				WriteError(msg, decl->script, decl->node);
			}
		}
	}

	// Verify that the declared structures are valid, e.g. that the structure
	// doesn't contain a member of its own type directly or indirectly
	while( toValidate.GetLength() > 0 )
	{
		asUINT numClasses = (asUINT)toValidate.GetLength();

		asCArray<sClassDeclaration*> toValidateNext((int)toValidate.GetLength());
		while( toValidate.GetLength() > 0 )
		{
			sClassDeclaration *decl = toValidate[toValidate.GetLength()-1];
			int validState = 1;
			for( asUINT n = 0; n < decl->objType->properties.GetLength(); n++ )
			{
				// A valid structure is one that uses only primitives or other valid objects
				asCObjectProperty *prop = decl->objType->properties[n];
				asCDataType dt = prop->type;

				if( dt.IsTemplate() )
				{
					asCDataType sub = dt;
					while( sub.IsTemplate() && !sub.IsObjectHandle() )
						sub = sub.GetSubType();

					dt = sub;
				}

				if( dt.IsObject() && !dt.IsObjectHandle() )
				{
					// Find the class declaration
					sClassDeclaration *pdecl = 0;
					for( asUINT p = 0; p < classDeclarations.GetLength(); p++ )
					{
						if( classDeclarations[p]->objType == dt.GetObjectType() )
						{
							pdecl = classDeclarations[p];
							break;
						}
					}

					if( pdecl )
					{
						if( pdecl->objType == decl->objType )
						{
							WriteError(TXT_ILLEGAL_MEMBER_TYPE, decl->script, decl->node);
							validState = 2;
							break;
						}
						else if( pdecl->validState != 1 )
						{
							validState = pdecl->validState;
							break;
						}
					}
				}
			}

			if( validState == 1 )
			{
				decl->validState = 1;
				toValidate.PopLast();
			}
			else if( validState == 2 )
			{
				decl->validState = 2;
				toValidate.PopLast();
			}
			else
			{
				toValidateNext.PushLast(toValidate.PopLast());
			}
		}

		toValidate = toValidateNext;
		toValidateNext.SetLength(0);

		if( numClasses == toValidate.GetLength() )
		{
			WriteError(TXT_ILLEGAL_MEMBER_TYPE, toValidate[0]->script, toValidate[0]->node);
			break;
		}
	}

	if( numErrors > 0 ) return;

	// Verify potential circular references
	for( n = 0; n < classDeclarations.GetLength(); n++ )
	{
		sClassDeclaration *decl = classDeclarations[n];
		if( decl->isExistingShared ) continue;
		asCObjectType *ot = decl->objType;

		// Is there some path in which this structure is involved in circular references?
		for( asUINT p = 0; p < ot->properties.GetLength(); p++ )
		{
			asCDataType dt = ot->properties[p]->type;
			if( dt.IsObject() )
			{
				if( dt.IsObjectHandle() )
				{
					// TODO: runtime optimize: If it is known that the handle can't be involved in a circular reference
					//                         then this object doesn't need to be marked as garbage collected.
					//                         - The application could set a flag when registering the object.
					//                         - The script classes can be marked as final, then the compiler will
					//                           be able to determine whether the class is garbage collected or not.

					ot->flags |= asOBJ_GC;
					break;
				}
				else if( dt.GetObjectType()->flags & asOBJ_GC )
				{
					// TODO: runtime optimize: Just because the member type is a potential circle doesn't mean that this one is
					//                         Only if the object is of a type that can reference this type, either directly or indirectly

					ot->flags |= asOBJ_GC;
					break;
				}
			}
		}
	}
}

void asCBuilder::IncludeMethodsFromMixins(sClassDeclaration *decl)
{
	asCScriptNode *node = decl->node->firstChild;

	// Skip the 'final' and 'shared' keywords
	if( decl->objType->IsShared() )
		node = node->next;
	if( decl->objType->flags & asOBJ_NOINHERIT )
		node = node->next;

	// Skip the name of the class
	node = node->next;

	// Find the included mixin classes
	while( node && node->nodeType == snIdentifier )
	{
		asSNameSpace *ns;
		asCString name;
		if( GetNamespaceAndNameFromNode(node, decl->script, decl->objType->nameSpace, ns, name) < 0 )
		{
			node = node->next;
			continue;
		}

		sMixinClass *mixin = GetMixinClass(name.AddressOf(), ns);
		if( mixin )
		{
			// Find methods from mixin declaration
			asCScriptNode *n = mixin->node->firstChild;

			// Skip to the member declarations
			// Possible keywords 'final' and 'shared' are removed in RegisterMixinClass so we don't need to worry about those here
			while( n && n->nodeType == snIdentifier )
				n = n->next;

			// Add methods from the mixin that are not already existing in the class
			while( n )
			{
				if( n->nodeType == snFunction )
				{
					// Instead of disconnecting the node, we need to clone it, otherwise other
					// classes that include the same mixin will not see the methods
					asCScriptNode *copy = n->CreateCopy(engine);

					// Register the method, but only if it doesn't already exist in the class
					RegisterScriptFunctionFromNode(engine->GetNextScriptFunctionId(), copy, mixin->script, decl->objType, false, false, 0, false, true);
				}
				else if( n->nodeType == snVirtualProperty )
				{
					// TODO: mixin: Support virtual properties too
					WriteError("The virtual property syntax is currently not supported for mixin classes", mixin->script, n);
					//RegisterVirtualProperty(node, decl->script, decl->objType, false, false);
				}

				n = n->next;
			}
		}

		node = node->next;
	}
}

void asCBuilder::IncludePropertiesFromMixins(sClassDeclaration *decl)
{
	asCScriptNode *node = decl->node->firstChild;

	// Skip the 'final' and 'shared' keywords
	if( decl->objType->IsShared() )
		node = node->next;
	if( decl->objType->flags & asOBJ_NOINHERIT )
		node = node->next;

	// Skip the name of the class
	node = node->next;

	// Find the included mixin classes
	while( node && node->nodeType == snIdentifier )
	{
		asSNameSpace *ns;
		asCString name;
		if( GetNamespaceAndNameFromNode(node, decl->script, decl->objType->nameSpace, ns, name) < 0 )
		{
			node = node->next;
			continue;
		}

		sMixinClass *mixin = GetMixinClass(name.AddressOf(), ns);
		if( mixin )
		{
			// Find properties from mixin declaration
			asCScriptNode *n = mixin->node->firstChild;

			// Skip to the member declarations
			// Possible keywords 'final' and 'shared' are removed in RegisterMixinClass so we don't need to worry about those here
			while( n && n->nodeType == snIdentifier )
				n = n->next;

			// Add properties from the mixin that are not already existing in the class
			while( n )
			{
				if( n->nodeType == snDeclaration )
				{
					asCScriptNode *n2 = n->firstChild;
					bool isPrivate = false;
					if( n2 && n2->tokenType == ttPrivate )
					{
						isPrivate = true;
						n2 = n2->next;
					}

					asCScriptCode *file = mixin->script;
					// TODO: namespace: Use correct implicit namespace from mixin
					asCDataType dt = CreateDataTypeFromNode(n2, file, engine->nameSpaces[0]);

					if( decl->objType->IsShared() && dt.GetObjectType() && !dt.GetObjectType()->IsShared() )
					{
						asCString msg;
						msg.Format(TXT_SHARED_CANNOT_USE_NON_SHARED_TYPE_s, dt.GetObjectType()->name.AddressOf());
						WriteError(msg, file, n);
						WriteInfo(TXT_WHILE_INCLUDING_MIXIN, decl->script, node);
					}

					if( dt.IsReadOnly() )
						WriteError(TXT_PROPERTY_CANT_BE_CONST, file, n);

					n2 = n2->next;
					while( n2 )
					{
						asCString name(&file->code[n2->tokenPos], n2->tokenLength);

						// Add the property only if it doesn't already exist in the class
						bool exists = false;
						for( asUINT p = 0; p < decl->objType->properties.GetLength(); p++ )
							if( decl->objType->properties[p]->name == name )
							{
								exists = true;
								break;
							}

						if( !exists )
						{
							if( !decl->isExistingShared )
							{
								// It must not conflict with the name of methods
								int r = CheckNameConflictMember(decl->objType, name.AddressOf(), n2, file, true);
								if( r < 0 )
									WriteInfo(TXT_WHILE_INCLUDING_MIXIN, decl->script, node);

								AddPropertyToClass(decl, name, dt, isPrivate, file, n);

								// Register the initialization expression
								if( n2->next && n2->next->nodeType != snIdentifier )
								{
									// TODO: decl: Store the initialization node for each property in the builder's  
									//             classDeclaration so these can be compiled in the constructors. The
									//             mixin class' members are added to the end of the class so the 
									//             initialization of these will be done last.
									WriteError("Initialization of class members in declaration is not yet supported for mixin classes", decl->script, n2);
									WriteInfo(TXT_WHILE_INCLUDING_MIXIN, decl->script, node);
								}
							}
							else
							{
								// Verify that the property exists in the original declaration
								bool found = false;
								for( asUINT p = 0; p < decl->objType->properties.GetLength(); p++ )
								{
									asCObjectProperty *prop = decl->objType->properties[p];
									if( prop->isPrivate == isPrivate &&
										prop->name == name &&
										prop->type == dt )
									{
										found = true;
										break;
									}
								}
								if( !found )
								{
									asCString str;
									str.Format(TXT_SHARED_s_DOESNT_MATCH_ORIGINAL, decl->objType->GetName());
									WriteError(str, decl->script, decl->node);
									WriteInfo(TXT_WHILE_INCLUDING_MIXIN, decl->script, node);
								}

								// Skip the initialization expression
								if( n2->next && n2->next->nodeType != snIdentifier )
									n2 = n2->next;
							}
						}

						n2 = n2->next;
					}
				}

				n = n->next;
			}
		}

		node = node->next;
	}
}

int asCBuilder::CreateVirtualFunction(asCScriptFunction *func, int idx)
{
	asCScriptFunction *vf = asNEW(asCScriptFunction)(engine, module, asFUNC_VIRTUAL);
	if( vf == 0 )
		return asOUT_OF_MEMORY;

	vf->name             = func->name;
	vf->returnType       = func->returnType;
	vf->parameterTypes   = func->parameterTypes;
	vf->inOutFlags       = func->inOutFlags;
	vf->id               = engine->GetNextScriptFunctionId();
	vf->scriptSectionIdx = func->scriptSectionIdx;
	vf->isReadOnly       = func->isReadOnly;
	vf->objectType       = func->objectType;
	vf->signatureId      = func->signatureId;
	vf->isPrivate        = func->isPrivate;
	vf->isFinal          = func->isFinal;
	vf->isOverride       = func->isOverride;
	vf->vfTableIdx       = idx;
	vf->defaultArgs      = func->defaultArgs;

	// Copy the default arg strings to avoid multiple deletes on the same object
	for( asUINT n = 0; n < vf->defaultArgs.GetLength(); n++ )
		if( vf->defaultArgs[n] )
			vf->defaultArgs[n] = asNEW(asCString)(*vf->defaultArgs[n]);

	module->AddScriptFunction(vf);

	// Add a dummy to the builder so that it doesn't mix up function ids
	functions.PushLast(0);

	return vf->id;
}

asCObjectProperty *asCBuilder::AddPropertyToClass(sClassDeclaration *decl, const asCString &name, const asCDataType &dt, bool isPrivate, asCScriptCode *file, asCScriptNode *node)
{
	if( !dt.CanBeInstanciated() )
	{
		if( file && node )
		{
			asCString str;
			str.Format(TXT_DATA_TYPE_CANT_BE_s, dt.Format().AddressOf());
			WriteError(str, file, node);
		}
		return 0;
	}

	return decl->objType->AddPropertyToClass(name, dt, isPrivate);
}

bool asCBuilder::DoesMethodExist(asCObjectType *objType, int methodId, asUINT *methodIndex)
{
	asCScriptFunction *method = GetFunctionDescription(methodId);

	for( asUINT n = 0; n < objType->methods.GetLength(); n++ )
	{
		asCScriptFunction *m = GetFunctionDescription(objType->methods[n]);

		if( m->name           != method->name           ) continue;
		if( m->returnType     != method->returnType     ) continue;
		if( m->isReadOnly     != method->isReadOnly     ) continue;
		if( m->parameterTypes != method->parameterTypes ) continue;
		if( m->inOutFlags     != method->inOutFlags     ) continue;

		if( methodIndex )
			*methodIndex = n;

		return true;
	}

	return false;
}

void asCBuilder::AddDefaultConstructor(asCObjectType *objType, asCScriptCode *file)
{
	int funcId = engine->GetNextScriptFunctionId();

	asCDataType returnType = asCDataType::CreatePrimitive(ttVoid, false);
	asCArray<asCDataType> parameterTypes;
	asCArray<asETypeModifiers> inOutFlags;
	asCArray<asCString *> defaultArgs;

	// Add the script function
	module->AddScriptFunction(file->idx, funcId, objType->name, returnType, parameterTypes, inOutFlags, defaultArgs, false, objType);

	// Set it as default constructor
	if( objType->beh.construct )
		engine->scriptFunctions[objType->beh.construct]->Release();
	objType->beh.construct = funcId;
	objType->beh.constructors[0] = funcId;
	engine->scriptFunctions[funcId]->AddRef();

	// The bytecode for the default constructor will be generated
	// only after the potential inheritance has been established
	sFunctionDescription *func = asNEW(sFunctionDescription);
	if( func == 0 )
	{
		// Out of memory
		return;
	}

	functions.PushLast(func);

	func->script            = file;
	func->node              = 0;
	func->name              = objType->name;
	func->objType           = objType;
	func->funcId            = funcId;

	// Add a default factory as well
	funcId = engine->GetNextScriptFunctionId();
	if( objType->beh.factory )
		engine->scriptFunctions[objType->beh.factory]->Release();
	objType->beh.factory = funcId;
	objType->beh.factories[0] = funcId;
	returnType = asCDataType::CreateObjectHandle(objType, false);
	module->AddScriptFunction(file->idx, funcId, objType->name, returnType, parameterTypes, inOutFlags, defaultArgs, false);
	functions.PushLast(0);
	asCCompiler compiler(engine);
	compiler.CompileFactory(this, file, engine->scriptFunctions[funcId]);
	engine->scriptFunctions[funcId]->AddRef();

	// If the object is shared, then the factory must also be marked as shared
	if( objType->flags & asOBJ_SHARED )
		engine->scriptFunctions[funcId]->isShared = true;
}

int asCBuilder::RegisterEnum(asCScriptNode *node, asCScriptCode *file, asSNameSpace *ns)
{
	// Is it a shared enum?
	bool isShared = false;
	asCObjectType *existingSharedType = 0;
	asCScriptNode *tmp = node->firstChild;
	if( tmp->nodeType == snIdentifier && file->TokenEquals(tmp->tokenPos, tmp->tokenLength, SHARED_TOKEN) )
	{
		isShared = true;
		tmp = tmp->next;
	}

	// Grab the name of the enumeration
	asCString name;
	asASSERT(snDataType == tmp->nodeType);
	asASSERT(snIdentifier == tmp->firstChild->nodeType);
	name.Assign(&file->code[tmp->firstChild->tokenPos], tmp->firstChild->tokenLength);

	if( isShared )
	{
		// Look for a pre-existing shared enum with the same signature
		for( asUINT n = 0; n < engine->classTypes.GetLength(); n++ )
		{
			asCObjectType *o = engine->classTypes[n];
			if( o &&
				o->IsShared() &&
				(o->flags & asOBJ_ENUM) &&
				o->name == name &&
				o->nameSpace == ns )
			{
				existingSharedType = o;
				break;
			}
		}
	}

	// Check the name and add the enum
	int r = CheckNameConflict(name.AddressOf(), tmp->firstChild, file, ns);
	if( asSUCCESS == r )
	{
		asCObjectType *st;

		if( existingSharedType )
			st = existingSharedType;
		else
		{
			st = asNEW(asCObjectType)(engine);
			if( st == 0 )
				return asOUT_OF_MEMORY;

			st->flags     = asOBJ_ENUM;
			if( isShared )
				st->flags |= asOBJ_SHARED;
			st->size      = 4;
			st->name      = name;
			st->nameSpace = ns;
		}
		module->enumTypes.PushLast(st);
		st->AddRef();

		// TODO: cleanup: Should the enum type really be stored in the engine->classTypes?
		//                http://www.gamedev.net/topic/616912-c-header-file-shared-with-scripts/page__gopid__4895940
		if( !existingSharedType )
			engine->classTypes.PushLast(st);

		// Store the location of this declaration for reference in name collisions
		sClassDeclaration *decl = asNEW(sClassDeclaration);
		if( decl == 0 )
			return asOUT_OF_MEMORY;

		decl->name             = name;
		decl->script           = file;
		decl->objType          = st;
		namedTypeDeclarations.PushLast(decl);

		asCDataType type = CreateDataTypeFromNode(tmp, file, ns);
		asASSERT(!type.IsReference());

		// Register the enum values
		tmp = tmp->next;
		while( tmp )
		{
			asASSERT(snIdentifier == tmp->nodeType);

			asCString name(&file->code[tmp->tokenPos], tmp->tokenLength);

			if( existingSharedType )
			{
				// If this is a pre-existent shared enum, then just double check
				// that the value is already defined in the original declaration
				bool found = false;
				for( size_t n = 0; n < st->enumValues.GetLength(); n++ )
					if( st->enumValues[n]->name == name )
					{
						found = true;
						break;
					}

				if( !found )
				{
					asCString str;
					str.Format(TXT_SHARED_s_DOESNT_MATCH_ORIGINAL, st->GetName());
					WriteError(str, file, tmp);
					break;
				}

				tmp = tmp->next;
				if( tmp && tmp->nodeType == snAssignment )
					tmp = tmp->next;
				continue;
			}
			else
			{
				// Check for name conflict errors with other values in the enum
				if( globVariables.GetFirst(ns, name, asCCompGlobVarType(type)) )
				{
					asCString str;
					str.Format(TXT_NAME_CONFLICT_s_ALREADY_USED, name.AddressOf());
					WriteError(str, file, tmp);

					tmp = tmp->next;
					if( tmp && tmp->nodeType == snAssignment )
						tmp = tmp->next;
					continue;
				}

				// Check for assignment
				asCScriptNode *asnNode = tmp->next;
				if( asnNode && snAssignment == asnNode->nodeType )
					asnNode->DisconnectParent();
				else
					asnNode = 0;

				// Create the global variable description so the enum value can be evaluated
				sGlobalVariableDescription *gvar = asNEW(sGlobalVariableDescription);
				if( gvar == 0 )
					return asOUT_OF_MEMORY;

				gvar->script		  = file;
				gvar->idNode          = 0;
				gvar->nextNode		  = asnNode;
				gvar->name			  = name;
				gvar->datatype		  = type;
				// No need to allocate space on the global memory stack since the values are stored in the asCObjectType
				gvar->index			  = 0;
				gvar->isCompiled	  = false;
				gvar->isPureConstant  = true;
				gvar->isEnumValue     = true;
				gvar->constantValue   = 0xdeadbeef;

				// Allocate dummy property so we can compile the value.
				// This will be removed later on so we don't add it to the engine.
				gvar->property            = asNEW(asCGlobalProperty);
				if( gvar->property == 0 )
					return asOUT_OF_MEMORY;

				gvar->property->name      = name;
				gvar->property->nameSpace = ns;
				gvar->property->type      = gvar->datatype;
				gvar->property->id        = 0;

				globVariables.Put(gvar);
				tmp = tmp->next;
			}
		}
	}

	node->Destroy(engine);

	return r;
}

int asCBuilder::RegisterTypedef(asCScriptNode *node, asCScriptCode *file, asSNameSpace *ns)
{
	// Get the native data type
	asCScriptNode *tmp = node->firstChild;
	asASSERT(NULL != tmp && snDataType == tmp->nodeType);
	asCDataType dataType;
	dataType.CreatePrimitive(tmp->tokenType, false);
	dataType.SetTokenType(tmp->tokenType);
	tmp = tmp->next;

	// Grab the name of the typedef
	asASSERT(NULL != tmp && NULL == tmp->next);
	asCString name;
	name.Assign(&file->code[tmp->tokenPos], tmp->tokenLength);

	// If the name is not already in use add it
 	int r = CheckNameConflict(name.AddressOf(), tmp, file, ns);

	asCObjectType *st = 0;
	if( asSUCCESS == r )
	{
		// Create the new type
		st = asNEW(asCObjectType)(engine);
		if( st == 0 )
			r = asOUT_OF_MEMORY;
	}

	if( asSUCCESS == r )
	{
		st->flags           = asOBJ_TYPEDEF;
		st->size            = dataType.GetSizeInMemoryBytes();
		st->name            = name;
		st->nameSpace       = ns;
		st->templateSubType = dataType;

		st->AddRef();

		module->typeDefs.PushLast(st);
		engine->classTypes.PushLast(st);

		// Store the location of this declaration for reference in name collisions
		sClassDeclaration *decl = asNEW(sClassDeclaration);
		if( decl == 0 )
			r = asOUT_OF_MEMORY;
		else
		{
			decl->name             = name;
			decl->script           = file;
			decl->objType          = st;
			namedTypeDeclarations.PushLast(decl);
		}
	}

	node->Destroy(engine);

	return r;
}

void asCBuilder::GetParsedFunctionDetails(asCScriptNode *node, asCScriptCode *file, asCObjectType *objType, asCString &name, asCDataType &returnType, asCArray<asCString> &parameterNames, asCArray<asCDataType> &parameterTypes, asCArray<asETypeModifiers> &inOutFlags, asCArray<asCString *> &defaultArgs, bool &isConstMethod, bool &isConstructor, bool &isDestructor, bool &isPrivate, bool &isOverride, bool &isFinal, bool &isShared)
{
	node = node->firstChild;

	// Is the function a private class method?
	isPrivate = false;
	if( node->tokenType == ttPrivate )
	{
		isPrivate = true;
		node = node->next;
	}

	// Is the function shared?
	isShared = false;
	if( node->tokenType == ttIdentifier && file->TokenEquals(node->tokenPos, node->tokenLength, SHARED_TOKEN) )
	{
		isShared = true;
		node = node->next;
	}

	// Find the name
	isConstructor = false;
	isDestructor = false;
	asCScriptNode *n = 0;
	if( node->nodeType == snDataType )
		n = node->next->next;
	else
	{
		// If the first node is a ~ token, then we know it is a destructor
		if( node->tokenType == ttBitNot )
		{
			n = node->next;
			isDestructor = true;
		}
		else
		{
			n = node;
			isConstructor = true;
		}
	}
	name.Assign(&file->code[n->tokenPos], n->tokenLength);

	// Initialize a script function object for registration
	if( !isConstructor && !isDestructor )
	{
		// TODO: namespace: Use correct implicit namespace
		returnType = CreateDataTypeFromNode(node, file, engine->nameSpaces[0]);
		returnType = ModifyDataTypeFromNode(returnType, node->next, file, 0, 0);
	}
	else
		returnType = asCDataType::CreatePrimitive(ttVoid, false);

	isConstMethod = false;
	isFinal = false;
	isOverride = false;

	if( objType && n->next->next )
	{
		asCScriptNode *decorator = n->next->next;

		// Is this a const method?
		if( decorator->tokenType == ttConst )
		{
			isConstMethod = true;
			decorator = decorator->next;
		}

		while( decorator )
		{
			if( decorator->tokenType == ttIdentifier && file->TokenEquals(decorator->tokenPos, decorator->tokenLength, FINAL_TOKEN) )
				isFinal = true;
			else if( decorator->tokenType == ttIdentifier && file->TokenEquals(decorator->tokenPos, decorator->tokenLength, OVERRIDE_TOKEN) )
				isOverride = true;

			decorator = decorator->next;
		}
	}

	// Count the number of parameters
	int count = 0;
	asCScriptNode *c = n->next->firstChild;
	while( c )
	{
		count++;
		c = c->next->next;
		if( c && c->nodeType == snIdentifier )
			c = c->next;
		if( c && c->nodeType == snExpression )
			c = c->next;
	}

	// Get the parameter types
	parameterNames.Allocate(count, false);
	parameterTypes.Allocate(count, false);
	inOutFlags.Allocate(count, false);
	defaultArgs.Allocate(count, false);
	n = n->next->firstChild;
	while( n )
	{
		asETypeModifiers inOutFlag;
		// TODO: namespace: Use correct implicit namespace
		asCDataType type = CreateDataTypeFromNode(n, file, engine->nameSpaces[0]);
		type = ModifyDataTypeFromNode(type, n->next, file, &inOutFlag, 0);

		// Store the parameter type
		parameterTypes.PushLast(type);
		inOutFlags.PushLast(inOutFlag);

		// Move to next parameter
		n = n->next->next;
		if( n && n->nodeType == snIdentifier )
		{
			asCString name;
			name.Assign(&file->code[n->tokenPos], n->tokenLength);
			parameterNames.PushLast(name);
			n = n->next;
		}
		else
		{
			// No name was given for the parameter
			parameterNames.PushLast(asCString());
		}

		if( n && n->nodeType == snExpression )
		{
			// Strip out white space and comments to better share the string
			asCString *defaultArgStr = asNEW(asCString);
			if( defaultArgStr )
				*defaultArgStr = GetCleanExpressionString(n, file);
			defaultArgs.PushLast(defaultArgStr);

			n = n->next;
		}
		else
			defaultArgs.PushLast(0);
	}
}
#endif

asCString asCBuilder::GetCleanExpressionString(asCScriptNode *node, asCScriptCode *file)
{
	asASSERT(node && node->nodeType == snExpression);

	asCString str;
	str.Assign(file->code + node->tokenPos, node->tokenLength);

	asCString cleanStr;
	for( asUINT n = 0; n < str.GetLength(); )
	{
		int len;
		asETokenClass tok = engine->ParseToken(str.AddressOf() + n, str.GetLength() - n, &len);
		if( tok != asTC_COMMENT && tok != asTC_WHITESPACE )
		{
			if( cleanStr.GetLength() ) cleanStr += " ";
			cleanStr.Concatenate(str.AddressOf() + n, len);
		}
		n += len;
	}

	return cleanStr;
}

#ifndef AS_NO_COMPILER
int asCBuilder::RegisterScriptFunctionFromNode(int funcId, asCScriptNode *node, asCScriptCode *file, asCObjectType *objType, bool isInterface, bool isGlobalFunction, asSNameSpace *ns, bool isExistingShared, bool isMixin)
{
	asCString                  name;
	asCDataType                returnType;
	asCArray<asCString>        parameterNames;
	asCArray<asCDataType>      parameterTypes;
	asCArray<asETypeModifiers> inOutFlags;
	asCArray<asCString *>      defaultArgs;
	bool                       isConstMethod;
	bool                       isOverride;
	bool                       isFinal;
	bool                       isConstructor;
	bool                       isDestructor;
	bool                       isPrivate;
	bool                       isShared;

	GetParsedFunctionDetails(node, file, objType, name, returnType, parameterNames, parameterTypes, inOutFlags, defaultArgs, isConstMethod, isConstructor, isDestructor, isPrivate, isOverride, isFinal, isShared);

	return RegisterScriptFunction(funcId, node, file, objType, isInterface, isGlobalFunction, ns, isExistingShared, isMixin, name, returnType, parameterNames, parameterTypes, inOutFlags, defaultArgs, isConstMethod, isConstructor, isDestructor, isPrivate, isOverride, isFinal, isShared);
}

int asCBuilder::RegisterScriptFunction(int funcId, asCScriptNode *node, asCScriptCode *file, asCObjectType *objType, bool isInterface, bool isGlobalFunction, asSNameSpace *ns, bool isExistingShared, bool isMixin, asCString &name, asCDataType &returnType, asCArray<asCString> &parameterNames, asCArray<asCDataType> &parameterTypes, asCArray<asETypeModifiers> &inOutFlags, asCArray<asCString *> &defaultArgs, bool isConstMethod, bool isConstructor, bool isDestructor, bool isPrivate, bool isOverride, bool isFinal, bool isShared)
{
	if( ns == 0 )
		ns = engine->nameSpaces[0];

	if( isExistingShared )
	{
		asASSERT( objType );

		// Should validate that the function really exists in the class/interface
		bool found = false;
		if( isConstructor || isDestructor )
		{
			// TODO: shared: Should check the existance of these too
			found = true;
		}
		else
		{
			for( asUINT n = 0; n < objType->methods.GetLength(); n++ )
			{
				asCScriptFunction *func = engine->scriptFunctions[objType->methods[n]];
				if( func->name == name &&
					func->IsSignatureExceptNameEqual(returnType, parameterTypes, inOutFlags, objType, isConstMethod) )
				{
					found = true;
					break;
				}
			}
		}

		if( !found )
		{
			asCString str;
			str.Format(TXT_SHARED_s_DOESNT_MATCH_ORIGINAL, objType->GetName());
			WriteError(str, file, node);
		}

		node->Destroy(engine);
		return 0;
	}

	// Check for name conflicts
	if( !isConstructor && !isDestructor )
	{
		if( objType )
		{
			CheckNameConflictMember(objType, name.AddressOf(), node, file, false);

			if( name == objType->name )
				WriteError(TXT_METHOD_CANT_HAVE_NAME_OF_CLASS, file, node);
		}
		else
			CheckNameConflict(name.AddressOf(), node, file, ns);
	}
	else
	{
		if( isMixin )
		{
			// Mixins cannot implement constructors/destructors
			WriteError(TXT_MIXIN_CANNOT_HAVE_CONSTRUCTOR, file, node);
			node->Destroy(engine);
			return 0;
		}

		// Verify that the name of the constructor/destructor is the same as the class
		if( name != objType->name )
			WriteError(TXT_CONSTRUCTOR_NAME_ERROR, file, node);

		if( isDestructor )
			name = "~" + name;
	}

	isExistingShared = false;
	if( !isInterface )
	{
		sFunctionDescription *func = asNEW(sFunctionDescription);
		if( func == 0 )
			return asOUT_OF_MEMORY;

		functions.PushLast(func);

		func->script            = file;
		func->node              = node;
		func->name              = name;
		func->objType           = objType;
		func->funcId            = funcId;
		func->isExistingShared  = false;
		func->paramNames        = parameterNames;

		if( isShared )
		{
			// Look for a pre-existing shared function with the same signature
			for( asUINT n = 0; n < engine->scriptFunctions.GetLength(); n++ )
			{
				asCScriptFunction *f = engine->scriptFunctions[n];
				if( f &&
					f->isShared &&
					f->name == name &&
					f->nameSpace == ns &&
					f->IsSignatureExceptNameEqual(returnType, parameterTypes, inOutFlags, 0, false) )
				{
					funcId           = func->funcId           = f->id;
					isExistingShared = func->isExistingShared = true;
					break;
				}
			}
		}
	}

	// Destructors may not have any parameters
	if( isDestructor && parameterTypes.GetLength() > 0 )
		WriteError(TXT_DESTRUCTOR_MAY_NOT_HAVE_PARM, file, node);

	// If a function, class, or interface is shared then only shared types may be used in the signature
	if( (objType && objType->IsShared()) || isShared )
	{
		asCObjectType *ot = returnType.GetObjectType();
		if( ot && !ot->IsShared() )
		{
			asCString msg;
			msg.Format(TXT_SHARED_CANNOT_USE_NON_SHARED_TYPE_s, ot->name.AddressOf());
			WriteError(msg, file, node);
		}

		for( asUINT p = 0; p < parameterTypes.GetLength(); ++p )
		{
			asCObjectType *ot = parameterTypes[p].GetObjectType();
			if( ot && !ot->IsShared() )
			{
				asCString msg;
				msg.Format(TXT_SHARED_CANNOT_USE_NON_SHARED_TYPE_s, ot->name.AddressOf());
				WriteError(msg, file, node);
			}
		}
	}

	// Check that the same function hasn't been registered already in the namespace
	asCArray<int> funcs;
	if( objType )
		GetObjectMethodDescriptions(name.AddressOf(), objType, funcs, false);
	else
		GetFunctionDescriptions(name.AddressOf(), funcs, ns);
	for( asUINT n = 0; n < funcs.GetLength(); ++n )
	{
		asCScriptFunction *func = GetFunctionDescription(funcs[n]);
		if( func->IsSignatureExceptNameAndReturnTypeEqual(parameterTypes, inOutFlags, objType, isConstMethod) )
		{
			if( isMixin )
			{
				// Clean up the memory, as the function will not be registered
				if( node )
					node->Destroy(engine);
				sFunctionDescription *func = functions.PopLast();
				asDELETE(func, sFunctionDescription);
				return 0;
			}

			WriteError(TXT_FUNCTION_ALREADY_EXIST, file, node);
			break;
		}
	}

	// Register the function
	if( isExistingShared )
	{
		asCScriptFunction *f = engine->scriptFunctions[funcId];
		module->AddScriptFunction(f);
		module->globalFunctions.Put(f);
		f->AddRef();
	}
	else
		module->AddScriptFunction(file->idx, funcId, name, returnType, parameterTypes, inOutFlags, defaultArgs, isInterface, objType, isConstMethod, isGlobalFunction, isPrivate, isFinal, isOverride, isShared, ns);

	// Make sure the default args are declared correctly
	ValidateDefaultArgs(file, node, engine->scriptFunctions[funcId]);

	if( objType )
	{
		engine->scriptFunctions[funcId]->AddRef();
		if( isConstructor )
		{
			int factoryId = engine->GetNextScriptFunctionId();
			if( parameterTypes.GetLength() == 0 )
			{
				// Overload the default constructor
				engine->scriptFunctions[objType->beh.construct]->Release();
				objType->beh.construct = funcId;
				objType->beh.constructors[0] = funcId;

				// Register the default factory as well
				engine->scriptFunctions[objType->beh.factory]->Release();
				objType->beh.factory = factoryId;
				objType->beh.factories[0] = factoryId;
			}
			else
			{
				objType->beh.constructors.PushLast(funcId);

				// Register the factory as well
				objType->beh.factories.PushLast(factoryId);
			}

			// We must copy the default arg strings to avoid deleting the same object multiple times
			for( asUINT n = 0; n < defaultArgs.GetLength(); n++ )
				if( defaultArgs[n] )
					defaultArgs[n] = asNEW(asCString)(*defaultArgs[n]);

			asCDataType dt = asCDataType::CreateObjectHandle(objType, false);
			module->AddScriptFunction(file->idx, factoryId, name, dt, parameterTypes, inOutFlags, defaultArgs, false);

			// If the object is shared, then the factory must also be marked as shared
			if( objType->flags & asOBJ_SHARED )
				engine->scriptFunctions[factoryId]->isShared = true;

			// Add a dummy function to the builder so that it doesn't mix up the fund Ids
			functions.PushLast(0);

			// Compile the factory immediately
			asCCompiler compiler(engine);
			compiler.CompileFactory(this, file, engine->scriptFunctions[factoryId]);
			engine->scriptFunctions[factoryId]->AddRef();
		}
		else if( isDestructor )
			objType->beh.destruct = funcId;
		else
			objType->methods.PushLast(funcId);
	}

	// We need to delete the node already if this is an interface method
	if( isInterface && node )
		node->Destroy(engine);

	return 0;
}

int asCBuilder::RegisterVirtualProperty(asCScriptNode *node, asCScriptCode *file, asCObjectType *objType, bool isInterface, bool isGlobalFunction, asSNameSpace *ns, bool isExistingShared)
{
	if( isExistingShared )
	{
		// TODO: shared: Should validate that the function really exists in the class/interface
		node->Destroy(engine);
		return 0;
	}

	if( engine->ep.propertyAccessorMode != 2 )
	{
		WriteError(TXT_PROPERTY_ACCESSOR_DISABLED, file, node);
		node->Destroy(engine);
		return 0;
	}

	if( ns == 0 )
		ns = engine->nameSpaces[0];

	bool isPrivate = false;
	asCString emulatedName;
	asCDataType emulatedType;

	asCScriptNode *mainNode = node;
	node = node->firstChild;

	if( !isGlobalFunction && node->tokenType == ttPrivate )
	{
		isPrivate = true;
		node = node->next;
	}

	// TODO: namespace: Use correct implicit namespace
	emulatedType = CreateDataTypeFromNode(node, file, engine->nameSpaces[0]);
	emulatedType = ModifyDataTypeFromNode(emulatedType, node->next, file, 0, 0);
	node = node->next->next;
	emulatedName.Assign(&file->code[node->tokenPos], node->tokenLength);

	if( node->next == 0 )
		WriteError(TXT_PROPERTY_WITHOUT_ACCESSOR, file, node);

	node = node->next;
	while( node )
	{
		asCScriptNode             *next        = node->next;
		asCScriptNode             *funcNode    = 0;
		bool                       success     = false;
		bool                       isConst     = false;
		bool                       isFinal     = false;
		bool                       isOverride  = false;
		asCDataType                returnType;
		asCArray<asCString>        paramNames;
		asCArray<asCDataType>      paramTypes;
		asCArray<asETypeModifiers> paramModifiers;
		asCArray<asCString*>       defaultArgs;
		asCString                  name;

		// TODO: getset: Allow private for individual property accessors
		// TODO: getset: If the accessor uses its own name, then the property should be automatically declared

		if( node->firstChild->nodeType == snIdentifier && file->TokenEquals(node->firstChild->tokenPos, node->firstChild->tokenLength, GET_TOKEN) )
		{
			funcNode  = node->firstChild->next;

			if( funcNode && funcNode->tokenType == ttConst )
			{
				isConst = true;
				funcNode = funcNode->next;
			}

			while( funcNode && funcNode->nodeType != snStatementBlock )
			{
				if( funcNode->tokenType == ttIdentifier && file->TokenEquals(funcNode->tokenPos, funcNode->tokenLength, FINAL_TOKEN) )
					isFinal = true;
				else if( funcNode->tokenType == ttIdentifier && file->TokenEquals(funcNode->tokenPos, funcNode->tokenLength, OVERRIDE_TOKEN) )
					isOverride = true;

				funcNode = funcNode->next;
			}

			if( funcNode )
				funcNode->DisconnectParent();

			if( funcNode == 0 && (objType == 0 || !objType->IsInterface()) )
			{
				// TODO: getset: If no implementation is supplied the builder should provide an automatically generated implementation
				//               The compiler needs to be able to handle the different types, primitive, value type, and handle
				//               The code is also different for global property accessors
				WriteError(TXT_PROPERTY_ACCESSOR_MUST_BE_IMPLEMENTED, file, node);
			}

			// Setup the signature for the get accessor method
			returnType = emulatedType;
			name       = "get_" + emulatedName;
			success    = true;
		}
		else if( node->firstChild->nodeType == snIdentifier && file->TokenEquals(node->firstChild->tokenPos, node->firstChild->tokenLength, SET_TOKEN) )
		{
			funcNode  = node->firstChild->next;

			if( funcNode && funcNode->tokenType == ttConst )
			{
				isConst = true;
				funcNode = funcNode->next;
			}

			while( funcNode && funcNode->nodeType != snStatementBlock )
			{
				if( funcNode->tokenType == ttIdentifier && file->TokenEquals(funcNode->tokenPos, funcNode->tokenLength, FINAL_TOKEN) )
					isFinal = true;
				else if( funcNode->tokenType == ttIdentifier && file->TokenEquals(funcNode->tokenPos, funcNode->tokenLength, OVERRIDE_TOKEN) )
					isOverride = true;

				funcNode = funcNode->next;
			}

			if( funcNode )
				funcNode->DisconnectParent();

			if( funcNode == 0 && (objType == 0 || !objType->IsInterface()) )
				WriteError(TXT_PROPERTY_ACCESSOR_MUST_BE_IMPLEMENTED, file, node);

			// Setup the signature for the set accessor method
			returnType = asCDataType::CreatePrimitive(ttVoid, false);
			paramModifiers.PushLast(asTM_NONE);
			paramNames.PushLast("value");
			paramTypes.PushLast(emulatedType);
			defaultArgs.PushLast(0);
			name = "set_" + emulatedName;
			success = true;
		}
		else
			WriteError(TXT_UNRECOGNIZED_VIRTUAL_PROPERTY_NODE, file, node);

		if( success )
			RegisterScriptFunction(engine->GetNextScriptFunctionId(), funcNode, file, objType, isInterface, isGlobalFunction, ns, false, false, name, returnType, paramNames, paramTypes, paramModifiers, defaultArgs, isConst, false, false, isPrivate, isOverride, isFinal, false);

		node = next;
	};

	mainNode->Destroy(engine);

	return 0;
}

int asCBuilder::RegisterImportedFunction(int importID, asCScriptNode *node, asCScriptCode *file, asSNameSpace *ns)
{
	asCString                  name;
	asCDataType                returnType;
	asCArray<asCString>        parameterNames;
	asCArray<asCDataType>      parameterTypes;
	asCArray<asETypeModifiers> inOutFlags;
	asCArray<asCString *>      defaultArgs;
	bool isConstMethod, isOverride, isFinal, isConstructor, isDestructor, isPrivate, isShared;

	if( ns == 0 )
		ns = engine->nameSpaces[0];

	GetParsedFunctionDetails(node->firstChild, file, 0, name, returnType, parameterNames, parameterTypes, inOutFlags, defaultArgs, isConstMethod, isConstructor, isDestructor, isPrivate, isOverride, isFinal, isShared);
	CheckNameConflict(name.AddressOf(), node, file, ns);
	
	// Check that the same function hasn't been registered already in the namespace
	asCArray<int> funcs;
	GetFunctionDescriptions(name.AddressOf(), funcs, ns);
	for( asUINT n = 0; n < funcs.GetLength(); ++n )
	{
		asCScriptFunction *func = GetFunctionDescription(funcs[n]);
		if( func->IsSignatureExceptNameAndReturnTypeEqual(parameterTypes, inOutFlags, 0, false) )
		{
			WriteError(TXT_FUNCTION_ALREADY_EXIST, file, node);
			break;
		}
	}

	// Read the module name as well
	asCScriptNode *nd = node->lastChild;
	asASSERT( nd->nodeType == snConstant && nd->tokenType == ttStringConstant );
	asCString moduleName;
	moduleName.Assign(&file->code[nd->tokenPos+1], nd->tokenLength-2);

	node->Destroy(engine);

	// Register the function
	module->AddImportedFunction(importID, name, returnType, parameterTypes, inOutFlags, defaultArgs, ns, moduleName);

	return 0;
}
#endif

asCScriptFunction *asCBuilder::GetFunctionDescription(int id)
{
	// TODO: import: This should be improved when the imported functions are removed
	// Get the description from the engine
	if( (id & 0xFFFF0000) == 0 )
		return engine->scriptFunctions[id];
	else
		return engine->importedFunctions[id & 0xFFFF]->importedFunctionSignature;
}

void asCBuilder::GetFunctionDescriptions(const char *name, asCArray<int> &funcs, asSNameSpace *ns)
{
	asUINT n;
	const asCArray<unsigned int> &idxs = module->globalFunctions.GetIndexes(ns, name);
	for( n = 0; n < idxs.GetLength(); n++ )
	{
		const asCScriptFunction *f = module->globalFunctions.Get(idxs[n]);
		asASSERT( f->objectType == 0 );
		funcs.PushLast(f->id);
	}

	// TODO: optimize: Linear search: This is probably not that critial. Also bindInformation will probably be removed in near future
	for( n = 0; n < module->bindInformations.GetLength(); n++ )
	{
		if( module->bindInformations[n]->importedFunctionSignature->name == name )
			funcs.PushLast(module->bindInformations[n]->importedFunctionSignature->id);
	}

	// TODO: optimize: Linear search. The registered global functions should be stored in a symbol table too
	for( n = 0; n < engine->registeredGlobalFuncs.GetLength(); n++ )
	{
		asCScriptFunction *f = engine->registeredGlobalFuncs[n];
		if( f &&
			f->funcType == asFUNC_SYSTEM &&
			f->objectType == 0 &&
			f->nameSpace == ns &&
			f->name == name )
		{
			// Verify if the module has access to the function
			if( module->accessMask & f->accessMask )
			{
				funcs.PushLast(f->id);
			}
		}
	}
}

void asCBuilder::GetObjectMethodDescriptions(const char *name, asCObjectType *objectType, asCArray<int> &methods, bool objIsConst, const asCString &scope)
{
	if( scope != "" )
	{
		// Find the base class with the specified scope
		while( objectType && objectType->name != scope )
			objectType = objectType->derivedFrom;

		// If the scope is not any of the base classes, then return no methods
		if( objectType == 0 )
			return;
	}

	// TODO: optimize: Improve linear search
	if( objIsConst )
	{
		// Only add const methods to the list
		for( asUINT n = 0; n < objectType->methods.GetLength(); n++ )
		{
			if( engine->scriptFunctions[objectType->methods[n]]->name == name &&
				engine->scriptFunctions[objectType->methods[n]]->isReadOnly )
			{
				// When the scope is defined the returned methods should be the true methods, not the virtual method stubs
				if( scope == "" )
					methods.PushLast(engine->scriptFunctions[objectType->methods[n]]->id);
				else
				{
					asCScriptFunction *virtFunc = engine->scriptFunctions[objectType->methods[n]];
					asCScriptFunction *realFunc = objectType->virtualFunctionTable[virtFunc->vfTableIdx];
					methods.PushLast(realFunc->id);
				}
			}
		}
	}
	else
	{
		// TODO: Prefer non-const over const
		for( asUINT n = 0; n < objectType->methods.GetLength(); n++ )
		{
			if( engine->scriptFunctions[objectType->methods[n]]->name == name )
			{
				// When the scope is defined the returned methods should be the true methods, not the virtual method stubs
				if( scope == "" )
					methods.PushLast(engine->scriptFunctions[objectType->methods[n]]->id);
				else
				{
					asCScriptFunction *virtFunc = engine->scriptFunctions[objectType->methods[n]];
					asCScriptFunction *realFunc = objectType->virtualFunctionTable[virtFunc->vfTableIdx];
					methods.PushLast(realFunc->id);
				}
			}
		}
	}
}

void asCBuilder::WriteInfo(const asCString &scriptname, const asCString &message, int r, int c, bool pre)
{
	// Need to store the pre message in a structure
	if( pre )
	{
		preMessage.isSet = true;
		preMessage.c = c;
		preMessage.r = r;
		preMessage.message = message;
		preMessage.scriptname = scriptname;
	}
	else
	{
		preMessage.isSet = false;
		engine->WriteMessage(scriptname.AddressOf(), r, c, asMSGTYPE_INFORMATION, message.AddressOf());
	}
}

void asCBuilder::WriteInfo(const asCString &message, asCScriptCode *file, asCScriptNode *node)
{
	int r = 0, c = 0;
	if( node )
		file->ConvertPosToRowCol(node->tokenPos, &r, &c);

	WriteInfo(file->name, message, r, c, false);
}

void asCBuilder::WriteError(const asCString &message, asCScriptCode *file, asCScriptNode *node)
{
	int r = 0, c = 0;
	if( node )
		file->ConvertPosToRowCol(node->tokenPos, &r, &c);

	WriteError(file->name, message, r, c);
}

void asCBuilder::WriteError(const asCString &scriptname, const asCString &message, int r, int c)
{
	numErrors++;

	// Need to pass the preMessage first
	if( preMessage.isSet )
		WriteInfo(preMessage.scriptname, preMessage.message, preMessage.r, preMessage.c, false);

	engine->WriteMessage(scriptname.AddressOf(), r, c, asMSGTYPE_ERROR, message.AddressOf());
}

void asCBuilder::WriteWarning(const asCString &scriptname, const asCString &message, int r, int c)
{
	numWarnings++;

	// Need to pass the preMessage first
	if( preMessage.isSet )
		WriteInfo(preMessage.scriptname, preMessage.message, preMessage.r, preMessage.c, false);

	engine->WriteMessage(scriptname.AddressOf(), r, c, asMSGTYPE_WARNING, message.AddressOf());
}

asCString asCBuilder::GetScopeFromNode(asCScriptNode *node, asCScriptCode *script, asCScriptNode **next)
{
	asCString scope;
	asCScriptNode *sn = node;
	if( sn->tokenType == ttScope )
	{
		scope = "::";
		sn = sn->next;
	}

	while( sn && sn->next && sn->next->tokenType == ttScope )
	{
		asCString tmp;
		tmp.Assign(&script->code[sn->tokenPos], sn->tokenLength);
		if( scope != "" && scope != "::" )
			scope += "::";
		scope += tmp;
		sn = sn->next->next;
	}

	if( next )
		*next = sn;

	return scope;
}

asSNameSpace *asCBuilder::GetNameSpaceFromNode(asCScriptNode *node, asCScriptCode *script, asSNameSpace *implicitNs, asCScriptNode **next)
{
	asCString scope = GetScopeFromNode(node, script, next);
	asSNameSpace *ns = implicitNs;
	if( scope == "::" )
		ns = engine->nameSpaces[0];
	else if( scope != "" )
	{
		ns = engine->FindNameSpace(scope.AddressOf());
		if( ns == 0 )
		{
			asCString msg;
			msg.Format(TXT_NAMESPACE_s_DOESNT_EXIST, scope.AddressOf());
			WriteError(msg, script, node);
		}
	}

	return ns;
}

asCDataType asCBuilder::CreateDataTypeFromNode(asCScriptNode *node, asCScriptCode *file, asSNameSpace *implicitNamespace, bool acceptHandleForScope, asCObjectType *currentType)
{
	asASSERT(node->nodeType == snDataType);

	asCDataType dt;

	asCScriptNode *n = node->firstChild;

	bool isConst = false;
	bool isImplicitHandle = false;
	if( n->tokenType == ttConst )
	{
		isConst = true;
		n = n->next;
	}

	// Determine namespace
	asSNameSpace *ns = GetNameSpaceFromNode(n, file, implicitNamespace, &n);
	if( ns == 0 )
	{
		// The namespace doesn't exist. Return a dummy type instead.
		dt = asCDataType::CreatePrimitive(ttInt, false);
		return dt;
	}

	if( n->tokenType == ttIdentifier )
	{
		asCString str;
		str.Assign(&file->code[n->tokenPos], n->tokenLength);

		asCObjectType *ot = 0;

		// If this is for a template type, then we must first determine if the
		// identifier matches any of the template subtypes
		// TODO: template: it should be possible to have more than one subtypes
		if( currentType && (currentType->flags & asOBJ_TEMPLATE) && str == currentType->templateSubType.GetObjectType()->name )
			ot = currentType->templateSubType.GetObjectType();

		if( ot == 0 )
			ot = GetObjectType(str.AddressOf(), ns);
		if( ot == 0 && !module && currentType )
			ot = GetObjectTypeFromTypesKnownByObject(str.AddressOf(), currentType);

		if( ot )
		{
			if( ot->flags & asOBJ_IMPLICIT_HANDLE )
				isImplicitHandle = true;

			// Make sure the module has access to the object type
			if( !module || (module->accessMask & ot->accessMask) )
			{
				if(asOBJ_TYPEDEF == (ot->flags & asOBJ_TYPEDEF))
				{
					// TODO: typedef: A typedef should be considered different from the original type (though with implicit conversions between the two)
					// Create primitive data type based on object flags
					dt = ot->templateSubType;
					dt.MakeReadOnly(isConst);
				}
				else
				{
					if( ot->flags & asOBJ_TEMPLATE )
					{
						n = n->next;

						// Check if the subtype is a type or the template's subtype
						// if it is the template's subtype then this is the actual template type,
						// orderwise it is a template instance.
						// Only do this for application registered interface, as the
						// scripts cannot implement templates.
						// TODO: namespace: Use correct implicit namespace
						asCDataType subType = CreateDataTypeFromNode(n, file, engine->nameSpaces[0], false, module ? 0 : ot);
						if( subType.GetObjectType() != ot->templateSubType.GetObjectType() )
						{
							// This is a template instance
							// Need to find the correct object type
							asCObjectType *otInstance = engine->GetTemplateInstanceType(ot, subType);

							if( !otInstance )
							{
								asCString msg;
								msg.Format(TXT_CANNOT_INSTANCIATE_TEMPLATE_s_WITH_s, ot->name.AddressOf(), subType.Format().AddressOf());
								WriteError(msg, file, n);
							}

							ot = otInstance;
						}
					}

					// Create object data type
					if( ot )
						dt = asCDataType::CreateObject(ot, isConst);
					else
						dt = asCDataType::CreatePrimitive(ttInt, isConst);
				}
			}
			else
			{
				asCString msg;
				msg.Format(TXT_TYPE_s_NOT_AVAILABLE_FOR_MODULE, (const char *)str.AddressOf());
				WriteError(msg, file, n);

				dt.SetTokenType(ttInt);
			}
		}
		else if( ot == 0 )
		{
			// It can still be a function definition
			asCScriptFunction *funcdef = GetFuncDef(str.AddressOf());

			if( funcdef )
			{
				dt = asCDataType::CreateFuncDef(funcdef);
			}
			else if( funcdef == 0 )
			{
				asCString msg;
				msg.Format(TXT_IDENTIFIER_s_NOT_DATA_TYPE, (const char *)str.AddressOf());
				WriteError(msg, file, n);

				dt = asCDataType::CreatePrimitive(ttInt, isConst);
				return dt;
			}
		}
	}
	else
	{
		// Create primitive data type
		dt = asCDataType::CreatePrimitive(n->tokenType, isConst);
	}

	// Determine array dimensions and object handles
	n = n->next;
	while( n && (n->tokenType == ttOpenBracket || n->tokenType == ttHandle) )
	{
		if( n->tokenType == ttOpenBracket )
		{
			// Make sure the sub type can be instanciated
			if( !dt.CanBeInstanciated() )
			{
				asCString str;
				// TODO: Change to "Array sub type cannot be 'type'"
				str.Format(TXT_DATA_TYPE_CANT_BE_s, dt.Format().AddressOf());
				WriteError(str, file, n);
			}

			// Make the type an array (or multidimensional array)
			if( dt.MakeArray(engine) < 0 )
			{
				WriteError(TXT_NO_DEFAULT_ARRAY_TYPE, file, n);
				break;
			}
		}
		else
		{
			// Make the type a handle
			if( dt.MakeHandle(true, acceptHandleForScope) < 0 )
			{
				WriteError(TXT_OBJECT_HANDLE_NOT_SUPPORTED, file, n);
				break;
			}
		}
		n = n->next;
	}

	if( isImplicitHandle )
	{
		// Make the type a handle
		if( dt.MakeHandle(true, acceptHandleForScope) < 0 )
			WriteError(TXT_OBJECT_HANDLE_NOT_SUPPORTED, file, n);
	}

	return dt;
}

asCDataType asCBuilder::ModifyDataTypeFromNode(const asCDataType &type, asCScriptNode *node, asCScriptCode *file, asETypeModifiers *inOutFlags, bool *autoHandle)
{
	asCDataType dt = type;

	if( inOutFlags ) *inOutFlags = asTM_NONE;

	// Is the argument sent by reference?
	asCScriptNode *n = node->firstChild;
	if( n && n->tokenType == ttAmp )
	{
		dt.MakeReference(true);
		n = n->next;

		if( n )
		{
			if( inOutFlags )
			{
				if( n->tokenType == ttIn )
					*inOutFlags = asTM_INREF;
				else if( n->tokenType == ttOut )
					*inOutFlags = asTM_OUTREF;
				else if( n->tokenType == ttInOut )
					*inOutFlags = asTM_INOUTREF;
				else
					asASSERT(false);
			}

			n = n->next;
		}
		else
		{
			if( inOutFlags )
				*inOutFlags = asTM_INOUTREF; // ttInOut
		}

		if( !engine->ep.allowUnsafeReferences &&
			inOutFlags && *inOutFlags == asTM_INOUTREF )
		{
			// Verify that the base type support &inout parameter types
			if( !dt.IsObject() || dt.IsObjectHandle() || !((dt.GetObjectType()->flags & asOBJ_NOCOUNT) || (dt.GetObjectType()->beh.addref && dt.GetObjectType()->beh.release)) )
				WriteError(TXT_ONLY_OBJECTS_MAY_USE_REF_INOUT, file, node->firstChild);
		}
	}

	if( autoHandle ) *autoHandle = false;

	if( n && n->tokenType == ttPlus )
	{
		// Autohandles are not supported for types with NOCOUNT
		if( dt.GetObjectType()->flags & asOBJ_NOCOUNT )
			WriteError(TXT_AUTOHANDLE_CANNOT_BE_USED_FOR_NOCOUNT, file, node->firstChild);

		if( autoHandle ) *autoHandle = true;
	}

	return dt;
}

asCObjectType *asCBuilder::GetObjectType(const char *type, asSNameSpace *ns)
{
	asCObjectType *ot = engine->GetObjectType(type, ns);
	if( !ot && module )
		ot = module->GetObjectType(type, ns);

	return ot;
}

// This function will return true if there are any types in the engine or module
// with the given name. The namespace is ignored in this verification.
bool asCBuilder::DoesTypeExist(const char *type)
{
	asUINT n;

	// TODO: optimize: Improve linear searches

	// Check if it is a registered type
	for( n = 0; n < engine->objectTypes.GetLength(); n++ )
		if( engine->objectTypes[n] &&
			engine->objectTypes[n]->name == type ) // TODO: template: Should we check the subtype in case of template instances?
			return true;

	for( n = 0; n < engine->registeredFuncDefs.GetLength(); n++ )
		if( engine->registeredFuncDefs[n]->name == type )
			return true;

	// Check if it is a script type
	if( module )
	{
		for( n = 0; n < module->classTypes.GetLength(); n++ )
			if( module->classTypes[n]->name == type )
				return true;

		for( n = 0; n < module->enumTypes.GetLength(); n++ )
			if( module->enumTypes[n]->name == type )
				return true;

		for( n = 0; n < module->typeDefs.GetLength(); n++ )
			if( module->typeDefs[n]->name == type )
				return true;

		for( asUINT n = 0; n < module->funcDefs.GetLength(); n++ )
			if( module->funcDefs[n]->name == type )
				return true;
	}

	return false;
}

asCObjectType *asCBuilder::GetObjectTypeFromTypesKnownByObject(const char *type, asCObjectType *currentType)
{
	if( currentType->name == type )
		return currentType;

	asUINT n;

	for( n = 0; n < currentType->properties.GetLength(); n++ )
		if( currentType->properties[n]->type.GetObjectType() &&
			currentType->properties[n]->type.GetObjectType()->name == type )
			return currentType->properties[n]->type.GetObjectType();

	for( n = 0; n < currentType->methods.GetLength(); n++ )
	{
		asCScriptFunction *func = engine->scriptFunctions[currentType->methods[n]];
		if( func->returnType.GetObjectType() &&
			func->returnType.GetObjectType()->name == type )
			return func->returnType.GetObjectType();

		for( asUINT f = 0; f < func->parameterTypes.GetLength(); f++ )
			if( func->parameterTypes[f].GetObjectType() &&
				func->parameterTypes[f].GetObjectType()->name == type )
				return func->parameterTypes[f].GetObjectType();
	}

	return 0;
}

asCScriptFunction *asCBuilder::GetFuncDef(const char *type)
{
	for( asUINT n = 0; n < engine->registeredFuncDefs.GetLength(); n++ )
		// TODO: access: Only return the definitions that the module has access to
		if( engine->registeredFuncDefs[n]->name == type )
			return engine->registeredFuncDefs[n];

	if( module )
	{
		for( asUINT n = 0; n < module->funcDefs.GetLength(); n++ )
			if( module->funcDefs[n]->name == type )
				return module->funcDefs[n];
	}

	return 0;
}

#ifndef AS_NO_COMPILER

int asCBuilder::GetEnumValueFromObjectType(asCObjectType *objType, const char *name, asCDataType &outDt, asDWORD &outValue)
{
	if( !objType || !(objType->flags & asOBJ_ENUM) )
		return 0;

	for( asUINT n = 0; n < objType->enumValues.GetLength(); ++n )
	{
		if( objType->enumValues[n]->name == name )
		{
			outDt = asCDataType::CreateObject(objType, true);
			outValue = objType->enumValues[n]->value;
			return 1;
		}
	}

	return 0;
}

int asCBuilder::GetEnumValue(const char *name, asCDataType &outDt, asDWORD &outValue, asSNameSpace *ns)
{
	bool found = false;

	// Search all available enum types
	asUINT t;
	for( t = 0; t < engine->objectTypes.GetLength(); t++ )
	{
		asCObjectType *ot = engine->objectTypes[t];
		if( ns != ot->nameSpace ) continue;

		if( GetEnumValueFromObjectType(ot, name, outDt, outValue) )
		{
			if( !found )
				found = true;
			else
			{
				// Found more than one value in different enum types
				return 2;
			}
		}
	}

	for( t = 0; t < module->enumTypes.GetLength(); t++ )
	{
		asCObjectType *ot = module->enumTypes[t];
		if( ns != ot->nameSpace ) continue;

		if( GetEnumValueFromObjectType(ot, name, outDt, outValue) )
		{
			if( !found )
				found = true;
			else
			{
				// Found more than one value in different enum types
				return 2;
			}
		}
	}

	if( found )
		return 1;

	// Didn't find any value
	return 0;
}

#endif // AS_NO_COMPILER

END_AS_NAMESPACE
