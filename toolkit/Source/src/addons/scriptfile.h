//
// CScriptFile
//
// This class encapsulates a FILE pointer in a reference counted class for
// use within AngelScript.
//

#ifndef SCRIPTFILE_H
#define SCRIPTFILE_H

//---------------------------
// Compilation settings
//

// Set this flag to turn on/off write support
//  0 = off
//  1 = on

#ifndef AS_WRITE_OPS
#define AS_WRITE_OPS 1
#endif




//---------------------------
// Declaration
//

#include "../angelscript/include/angelscript.h"
#include <string>
#include <stdio.h>

BEGIN_AS_NAMESPACE

struct PODCScriptFile
{
    // Big-endian = most significant byte first
    bool mostSignificantByteFirst;

    mutable int refCount;
    FILE       *file;
};

class CScriptFile : public PODCScriptFile
{
public:
    CScriptFile();

    void AddRef() const;
    void Release() const;

	// TODO: Implement the "r+", "w+" and "a+" modes
	// mode = "r" -> open the file for reading
	//        "w" -> open the file for writing (overwrites existing file)
	//        "a" -> open the file for appending
    int  Open(const std::string &filename, const std::string &mode);
    int  Close();
    int  GetSize() const;
    bool IsEOF() const;

    // Reading
    int      ReadString(unsigned int length, std::string &str);
    int      ReadLine(std::string &str);
    asINT64  ReadInt(asUINT bytes);
    asQWORD  ReadUInt(asUINT bytes);
    float    ReadFloat();
    double   ReadDouble();

    // Writing
    int WriteString(const std::string &str);
    int WriteInt(asINT64 v, asUINT bytes);
    int WriteUInt(asQWORD v, asUINT bytes);
    int WriteFloat(float v);
    int WriteDouble(double v);

    // Cursor
	int GetPos() const;
    int SetPos(int pos);
    int MovePos(int delta);

protected:
    ~CScriptFile();
};

// This function will determine the configuration of the engine
// and use one of the two functions below to register the file type
void RegisterScriptFile(asIScriptEngine *engine);

// Call this function to register the file type
// using native calling conventions
void RegisterScriptFile_Native(asIScriptEngine *engine);

// Use this one instead if native calling conventions
// are not supported on the target platform
void RegisterScriptFile_Generic(asIScriptEngine *engine);

END_AS_NAMESPACE

#endif
