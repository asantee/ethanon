#ifndef SCRIPTDICTIONARY_H
#define SCRIPTDICTIONARY_H

// The dictionary class relies on the script string object, thus the script
// string type must be registered with the engine before registering the
// dictionary type

#include "../angelscript/include/angelscript.h"
#include <Types.h>
#include <string>

#ifdef _MSC_VER
// Turn off annoying warnings about truncated symbol names
#pragma warning (disable:4786)
#endif

#include <map>

BEGIN_AS_NAMESPACE

class CScriptDictionary
{
public:
    // Memory management
    CScriptDictionary(asIScriptEngine *engine);
    void AddRef() const;
    void Release() const;

    CScriptDictionary &operator =(const CScriptDictionary &other);

    // Sets/Gets a variable type value for a key
    void Set(const gs2d::str_type::string &key, void *value, int typeId);
    bool Get(const gs2d::str_type::string &key, void *value, int typeId) const;

    // Sets/Gets an integer number value for a key
    void Set(const gs2d::str_type::string &key, asINT64 &value);
    bool Get(const gs2d::str_type::string &key, asINT64 &value) const;

    // Sets/Gets a real number value for a key
    void Set(const gs2d::str_type::string &key, double &value);
    bool Get(const gs2d::str_type::string &key, double &value) const;

    // Returns true if the key is set
    bool Exists(const gs2d::str_type::string &key) const;

    // Deletes the key
    void Delete(const gs2d::str_type::string &key);

    // Deletes all keys
    void DeleteAll();

	// Garbage collections behaviours
	int GetRefCount();
	void SetGCFlag();
	bool GetGCFlag();
	void EnumReferences(asIScriptEngine *engine);
	void ReleaseAllReferences(asIScriptEngine *engine);

protected:
	// The structure for holding the values
    struct valueStruct
    {
        union
        {
            asINT64 valueInt;
            double  valueFlt;
            void   *valueObj;
        };
        int   typeId;
    };
    
	// We don't want anyone to call the destructor directly, it should be called through the Release method
	virtual ~CScriptDictionary();

	// Helper methods
    void FreeValue(valueStruct &value);
	
	// Our properties
    asIScriptEngine *engine;
    mutable int refCount;
    std::map<gs2d::str_type::string, valueStruct> dict;
};

// This function will determine the configuration of the engine
// and use one of the two functions below to register the dictionary object
void RegisterScriptDictionary(asIScriptEngine *engine);

// Call this function to register the math functions
// using native calling conventions
void RegisterScriptDictionary_Native(asIScriptEngine *engine);

// Use this one instead if native calling conventions
// are not supported on the target platform
void RegisterScriptDictionary_Generic(asIScriptEngine *engine);

END_AS_NAMESPACE

#endif
