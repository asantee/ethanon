#include "../../source/src/angelscript/include/angelscript.h"
#include <iostream>
#include <assert.h>

// this function must be extern "C" to export its name correctly
extern "C" __declspec(dllexport) bool ETHCall(asIScriptEngine *pASEngine);

/*
	To load custom DLL's to your project, create a text file named
	plugin.list and place it into the project's root. Insert each DLL
	file name line by line in the file, for instance:

	NetworkFunctions.dll
	Pathfinding.dll

	Ethanon Engine will always call the 'int ETHCall(asIScriptEngine*)'
	function in the DLL and execute it before the game script is loaded.
	With the asIScriptEngine object the user may bind its own functions
	and objects to the script, allowing them to be used in the game script.
	This can be used on network operations or whatever the project needs.

	To learn how to use the asIScriptEngine object, check the AngelScript
	manual:
	http://www.angelcode.com/angelscript/sdk/docs/manual/index.html
	Consider browsing the ETHScriptObjRegister.cpp and ETHEngine.cpp
	source files the get some samples of global functions and object
	bindings.
*/
bool ETHCall(asIScriptEngine *pASEngine)
{
	/*
		Your script bindings here...
	*/
	return true;
}
