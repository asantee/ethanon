#ifndef SCRIPTARRAY_H
#define SCRIPTARRAY_H

#include "../angelscript/include/angelscript.h"

BEGIN_AS_NAMESPACE

struct SArrayBuffer;

class CScriptArray
{
public:
	CScriptArray(asUINT length, asIObjectType *ot);
	CScriptArray(asUINT length, void *defVal, asIObjectType *ot);
	virtual ~CScriptArray();

	void AddRef() const;
	void Release() const;

	// Type information
	asIObjectType *GetArrayObjectType() const;
	int            GetArrayTypeId() const;
	int            GetElementTypeId() const;

	void   Resize(asUINT numElements);
	asUINT GetSize() const;

	// Get a pointer to an element. Returns 0 if out of bounds
	void  *At(asUINT index);

	CScriptArray &operator=(const CScriptArray&);

	// TODO: Add methods Sort, Reverse, Find, etc
	void InsertAt(asUINT index, void *value);
	void RemoveAt(asUINT index);
	void InsertLast(void *value);
	void RemoveLast();

	// GC methods
	int  GetRefCount();
	void SetFlag();
	bool GetFlag();
	void EnumReferences(asIScriptEngine *engine);
	void ReleaseAllHandles(asIScriptEngine *engine);

protected:
	mutable int    refCount;
	mutable bool   gcFlag;
	asIObjectType *objType;
	SArrayBuffer  *buffer;
	bool           isArrayOfHandles;
	int            elementSize;

	bool CheckMaxSize(asUINT numElements);

	void Resize(int delta, asUINT at);
	void SetValue(asUINT index, void *value);

	void CreateBuffer(SArrayBuffer **buf, asUINT numElements);
	void DeleteBuffer(SArrayBuffer *buf);
	void CopyBuffer(SArrayBuffer *dst, SArrayBuffer *src);

	void Construct(SArrayBuffer *buf, asUINT start, asUINT end);
	void Destruct(SArrayBuffer *buf, asUINT start, asUINT end);
};

void RegisterScriptArray(asIScriptEngine *engine, bool defaultArray);

END_AS_NAMESPACE

#endif
