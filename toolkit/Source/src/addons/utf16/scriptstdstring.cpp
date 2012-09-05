#include <assert.h>
#include <sstream>
#include "scriptstdstring.h"
#include <string.h> // strstr

//using namespace std;
// TODO: add std:: and make std::string -> std::wstring

BEGIN_AS_NAMESPACE

static void StringFactoryGeneric(asIScriptGeneric *gen) {
  asUINT length = gen->GetArgDWord(0)/sizeof(wchar_t);
  const wchar_t *s = (const wchar_t*)gen->GetArgAddress(1);
  std::wstring str(s, length);
  gen->SetReturnObject(&str);
}

static void ConstructStringGeneric(asIScriptGeneric * gen) {
  new (gen->GetObject()) std::wstring();
}

static void DestructStringGeneric(asIScriptGeneric * gen) {
  std::wstring * ptr = static_cast<std::wstring *>(gen->GetObject());
  using namespace std;
  ptr->~wstring();
}

static void AssignStringGeneric(asIScriptGeneric *gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetArgObject(0));
  std::wstring * self = static_cast<std::wstring *>(gen->GetObject());
  *self = *a;
  gen->SetReturnAddress(self);
}

static void AddAssignStringGeneric(asIScriptGeneric *gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetArgObject(0));
  std::wstring * self = static_cast<std::wstring *>(gen->GetObject());
  *self += *a;
  gen->SetReturnAddress(self);
}
/*
static void StringEqualGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetArgAddress(0));
  std::wstring * b = static_cast<std::wstring *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a == *b);
}
*/
static void StringEqualsGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetObject());
  std::wstring * b = static_cast<std::wstring *>(gen->GetArgAddress(0));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a == *b);
}

static void StringCmpGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetObject());
  std::wstring * b = static_cast<std::wstring *>(gen->GetArgAddress(0));

  int cmp = 0;
  if( *a < *b ) cmp = -1;
  else if( *a > *b ) cmp = 1;

  *(int*)gen->GetAddressOfReturnLocation() = cmp;
}
/*
static void StringNotEqualGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetArgAddress(0));
  std::wstring * b = static_cast<std::wstring *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a != *b);
}

static void StringLEqualGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetArgAddress(0));
  std::wstring * b = static_cast<std::wstring *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a <= *b);
}

static void StringGEqualGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetArgAddress(0));
  std::wstring * b = static_cast<std::wstring *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a >= *b);
}

static void StringLessThanGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetArgAddress(0));
  std::wstring * b = static_cast<std::wstring *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a < *b);
}

static void StringGreaterThanGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetArgAddress(0));
  std::wstring * b = static_cast<std::wstring *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a > *b);
}
*/
static void StringAddGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetObject());
  std::wstring * b = static_cast<std::wstring *>(gen->GetArgAddress(0));
  std::wstring ret_val = *a + *b;
  gen->SetReturnObject(&ret_val);
}

static void StringLengthGeneric(asIScriptGeneric * gen) {
  std::wstring * self = static_cast<std::wstring *>(gen->GetObject());
  *static_cast<size_t *>(gen->GetAddressOfReturnLocation()) = self->length();
}

static void StringResizeGeneric(asIScriptGeneric * gen) {
  std::wstring * self = static_cast<std::wstring *>(gen->GetObject());
  self->resize(*static_cast<size_t *>(gen->GetAddressOfArg(0)));
}

static void StringCharAtGeneric(asIScriptGeneric * gen) {
  unsigned int index = gen->GetArgDWord(0);
  std::wstring * self = static_cast<std::wstring *>(gen->GetObject());

  if (index >= self->size()) {
    // Set a script exception
    asIScriptContext *ctx = asGetActiveContext();
    ctx->SetException("Out of range");

    gen->SetReturnAddress(0);
  } else {
    gen->SetReturnAddress(&(self->operator [](index)));
  }
}

void AssignInt2StringGeneric(asIScriptGeneric *gen) 
{
	int *a = static_cast<int*>(gen->GetAddressOfArg(0));
	std::wstring *self = static_cast<std::wstring*>(gen->GetObject());
	std::wstringstream sstr;
	sstr << *a;
	*self = sstr.str();
	gen->SetReturnAddress(self);
}

void AssignUInt2StringGeneric(asIScriptGeneric *gen) 
{
	unsigned int *a = static_cast<unsigned int*>(gen->GetAddressOfArg(0));
	std::wstring *self = static_cast<std::wstring*>(gen->GetObject());
	std::wstringstream sstr;
	sstr << *a;
	*self = sstr.str();
	gen->SetReturnAddress(self);
}

void AssignDouble2StringGeneric(asIScriptGeneric *gen) 
{
	double *a = static_cast<double*>(gen->GetAddressOfArg(0));
	std::wstring *self = static_cast<std::wstring*>(gen->GetObject());
	std::wstringstream sstr;
	sstr << *a;
	*self = sstr.str();
	gen->SetReturnAddress(self);
}

void AssignBool2StringGeneric(asIScriptGeneric *gen) 
{
	bool *a = static_cast<bool*>(gen->GetAddressOfArg(0));
	std::wstring *self = static_cast<std::wstring*>(gen->GetObject());
	std::wstringstream sstr;
	sstr << (*a ? L"true" : L"false");
	*self = sstr.str();
	gen->SetReturnAddress(self);
}

void AddAssignDouble2StringGeneric(asIScriptGeneric * gen) {
  double * a = static_cast<double *>(gen->GetAddressOfArg(0));
  std::wstring * self = static_cast<std::wstring *>(gen->GetObject());
  std::wstringstream sstr;
  sstr << *a;
  *self += sstr.str();
  gen->SetReturnAddress(self);
}

void AddAssignInt2StringGeneric(asIScriptGeneric * gen) {
  int * a = static_cast<int *>(gen->GetAddressOfArg(0));
  std::wstring * self = static_cast<std::wstring *>(gen->GetObject());
  std::wstringstream sstr;
  sstr << *a;
  *self += sstr.str();
  gen->SetReturnAddress(self);
}

void AddAssignUInt2StringGeneric(asIScriptGeneric * gen) {
  unsigned int * a = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
  std::wstring * self = static_cast<std::wstring *>(gen->GetObject());
  std::wstringstream sstr;
  sstr << *a;
  *self += sstr.str();
  gen->SetReturnAddress(self);
}

void AddAssignBool2StringGeneric(asIScriptGeneric * gen) {
  bool * a = static_cast<bool *>(gen->GetAddressOfArg(0));
  std::wstring * self = static_cast<std::wstring *>(gen->GetObject());
  std::wstringstream sstr;
  sstr << (*a ? L"true" : L"false");
  *self += sstr.str();
  gen->SetReturnAddress(self);
}

void AddString2DoubleGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetObject());
  double * b = static_cast<double *>(gen->GetAddressOfArg(0));
  std::wstringstream sstr;
  sstr << *a << *b;
  std::wstring ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddString2IntGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetObject());
  int * b = static_cast<int *>(gen->GetAddressOfArg(0));
  std::wstringstream sstr;
  sstr << *a << *b;
  std::wstring ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddString2UIntGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetObject());
  unsigned int * b = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
  std::wstringstream sstr;
  sstr << *a << *b;
  std::wstring ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddString2BoolGeneric(asIScriptGeneric * gen) {
  std::wstring * a = static_cast<std::wstring *>(gen->GetObject());
  bool * b = static_cast<bool *>(gen->GetAddressOfArg(0));
  std::wstringstream sstr;
  sstr << *a << *b ? L"true" : L"false";
  std::wstring ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddDouble2StringGeneric(asIScriptGeneric * gen) {
  double* a = static_cast<double *>(gen->GetAddressOfArg(0));
  std::wstring * b = static_cast<std::wstring *>(gen->GetObject());
  std::wstringstream sstr;
  sstr << *a << *b;
  std::wstring ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddInt2StringGeneric(asIScriptGeneric * gen) {
  int* a = static_cast<int *>(gen->GetAddressOfArg(0));
  std::wstring * b = static_cast<std::wstring *>(gen->GetObject());
  std::wstringstream sstr;
  sstr << *a << *b;
  std::wstring ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddUInt2StringGeneric(asIScriptGeneric * gen) {
  unsigned int* a = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
  std::wstring * b = static_cast<std::wstring *>(gen->GetObject());
  std::wstringstream sstr;
  sstr << *a << *b;
  std::wstring ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddBool2StringGeneric(asIScriptGeneric * gen) {
  bool* a = static_cast<bool *>(gen->GetAddressOfArg(0));
  std::wstring * b = static_cast<std::wstring *>(gen->GetObject());
  std::wstringstream sstr;
  sstr << (*a ? L"true" : L"false") << *b;
  std::wstring ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

static void StringFindGeneric(asIScriptGeneric * gen) {
	std::wstring * a = static_cast<std::wstring *>(gen->GetObject());
	std::wstring * b = static_cast<std::wstring *>(gen->GetArgAddress(0));
	*static_cast<size_t *>(gen->GetAddressOfReturnLocation()) = a->find(*b);
}

static void StringSubstrGeneric(asIScriptGeneric * gen) {
	std::wstring * a = static_cast<std::wstring *>(gen->GetObject());
	std::size_t pos  = *static_cast<std::size_t*>(gen->GetAddressOfArg(0));
	std::size_t size = *static_cast<std::size_t*>(gen->GetAddressOfArg(1));
	std::wstring ret_val = a->substr(pos, size);
	gen->SetReturnObject(&ret_val);
}

void RegisterStdString_Generic(asIScriptEngine *engine) {
  int r;

  // Register the std::wstring type
  r = engine->RegisterObjectType("string", sizeof(std::wstring), asOBJ_VALUE | asOBJ_APP_CLASS_CDA); assert( r >= 0 );

  // Register the std::wstring factory
  r = engine->RegisterStringFactory("string", asFUNCTION(StringFactoryGeneric), asCALL_GENERIC); assert( r >= 0 );

  // Register the object operator overloads
  r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f()",                    asFUNCTION(ConstructStringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectBehaviour("string", asBEHAVE_DESTRUCT,   "void f()",                    asFUNCTION(DestructStringGeneric),  asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAssign(const string &in)", asFUNCTION(AssignStringGeneric),    asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAddAssign(const string &in)", asFUNCTION(AddAssignStringGeneric), asCALL_GENERIC); assert( r >= 0 );

  r = engine->RegisterObjectMethod("string", "bool opEquals(const string &in) const", asFUNCTION(StringEqualsGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "int opCmp(const string &in) const", asFUNCTION(StringCmpGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd(const string &in) const", asFUNCTION(StringAddGeneric), asCALL_GENERIC); assert( r >= 0 );

  // Register the object methods
#pragma warning( push )
#pragma warning( disable : 4127 )

  if (sizeof(size_t) == 4) {
    r = engine->RegisterObjectMethod("string", "uint length() const", asFUNCTION(StringLengthGeneric), asCALL_GENERIC); assert( r >= 0 );
    r = engine->RegisterObjectMethod("string", "void resize(uint)",   asFUNCTION(StringResizeGeneric), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "uint find(const string &in) const", asFUNCTION(StringFindGeneric), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string substr(const uint, const uint) const", asFUNCTION(StringSubstrGeneric), asCALL_GENERIC); assert( r >= 0 );
  } else {
    r = engine->RegisterObjectMethod("string", "uint64 length() const", asFUNCTION(StringLengthGeneric), asCALL_GENERIC); assert( r >= 0 );
    r = engine->RegisterObjectMethod("string", "void resize(uint64)",   asFUNCTION(StringResizeGeneric), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "uint64 find(const string &in) const", asFUNCTION(StringFindGeneric), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string substr(const uint64, const uint64) const", asFUNCTION(StringSubstrGeneric), asCALL_GENERIC); assert( r >= 0 );
  }

#pragma warning( pop )

  // Register the index operator, both as a mutator and as an inspector
  r = engine->RegisterObjectMethod("string", "uint8 &opIndex(uint)", asFUNCTION(StringCharAtGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "const uint8 &opIndex(uint) const", asFUNCTION(StringCharAtGeneric), asCALL_GENERIC); assert( r >= 0 );

  // Automatic conversion from values
  r = engine->RegisterObjectMethod("string", "string &opAssign(double)", asFUNCTION(AssignDouble2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAddAssign(double)", asFUNCTION(AddAssignDouble2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd(double) const", asFUNCTION(AddString2DoubleGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd_r(double) const", asFUNCTION(AddDouble2StringGeneric), asCALL_GENERIC); assert( r >= 0 );

  r = engine->RegisterObjectMethod("string", "string &opAssign(int)", asFUNCTION(AssignInt2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAddAssign(int)", asFUNCTION(AddAssignInt2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd(int) const", asFUNCTION(AddString2IntGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd_r(int) const", asFUNCTION(AddInt2StringGeneric), asCALL_GENERIC); assert( r >= 0 );

  r = engine->RegisterObjectMethod("string", "string &opAssign(uint)", asFUNCTION(AssignUInt2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAddAssign(uint)", asFUNCTION(AddAssignUInt2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd(uint) const", asFUNCTION(AddString2UIntGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd_r(uint) const", asFUNCTION(AddUInt2StringGeneric), asCALL_GENERIC); assert( r >= 0 );

  r = engine->RegisterObjectMethod("string", "string &opAssign(bool)", asFUNCTION(AssignBool2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAddAssign(bool)", asFUNCTION(AddAssignBool2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd(bool) const", asFUNCTION(AddString2BoolGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd_r(bool) const", asFUNCTION(AddBool2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
}

static std::wstring StringFactory(asUINT length, const wchar_t *s)
{
	return std::wstring(s, length/sizeof(wchar_t));
}

static void ConstructString(std::wstring *thisPointer)
{
	new(thisPointer) std::wstring();
}

static void DestructString(std::wstring *thisPointer)
{
	using namespace std;
	thisPointer->~wstring();
}

static std::wstring &AssignUIntToString(unsigned int i, std::wstring &dest)
{
	std::wostringstream stream;
	stream << i;
	dest = stream.str();
	return dest;
}

static std::wstring &AddAssignUIntToString(unsigned int i, std::wstring &dest)
{
	std::wostringstream stream;
	stream << i;
	dest += stream.str();
	return dest;
}

static std::wstring AddStringUInt(std::wstring &str, unsigned int i)
{
	std::wostringstream stream;
	stream << i;
	str += stream.str();
	return str;
}

static std::wstring AddIntString(int i, std::wstring &str)
{
	std::wostringstream stream;
	stream << i;
	return stream.str() + str;
}

static std::wstring &AssignIntToString(int i, std::wstring &dest)
{
	std::wostringstream stream;
	stream << i;
	dest = stream.str();
	return dest;
}

static std::wstring &AddAssignIntToString(int i, std::wstring &dest)
{
	std::wostringstream stream;
	stream << i;
	dest += stream.str();
	return dest;
}

static std::wstring AddStringInt(std::wstring &str, int i)
{
	std::wostringstream stream;
	stream << i;
	str += stream.str();
	return str;
}

static std::wstring AddUIntString(unsigned int i, std::wstring &str)
{
	std::wostringstream stream;
	stream << i;
	return stream.str() + str;
}

static std::wstring &AssignDoubleToString(double f, std::wstring &dest)
{
	std::wostringstream stream;
	stream << f;
	dest = stream.str();
	return dest;
}

static std::wstring &AddAssignDoubleToString(double f, std::wstring &dest)
{
	std::wostringstream stream;
	stream << f;
	dest += stream.str();
	return dest;
}

static std::wstring &AssignBoolToString(bool b, std::wstring &dest)
{
	std::wostringstream stream;
	stream << (b ? L"true" : L"false");
	dest = stream.str();
	return dest;
}

static std::wstring &AddAssignBoolToString(bool b, std::wstring &dest)
{
	std::wostringstream stream;
	stream << (b ? L"true" : L"false");
	dest += stream.str();
	return dest;
}

static std::wstring AddStringDouble(std::wstring &str, double f)
{
	std::wostringstream stream;
	stream << f;
	str += stream.str();
	return str;
}

static std::wstring AddDoubleString(double f, std::wstring &str)
{
	std::wostringstream stream;
	stream << f;
	return stream.str() + str;
}

static std::wstring AddStringBool(std::wstring &str, bool b)
{
	std::wostringstream stream;
	stream << (b ? L"true" : L"false");
	str += stream.str();
	return str;
}

static std::wstring AddBoolString(bool b, std::wstring &str)
{
	std::wostringstream stream;
	stream << (b ? L"true" : L"false");
	return stream.str() + str;
}

static wchar_t *StringCharAt(unsigned int i, std::wstring &str)
{
	if( i >= str.size() )
	{
		// Set a script exception
		asIScriptContext *ctx = asGetActiveContext();
		ctx->SetException("Out of range");

		// Return a null pointer
		return 0;
	}

	return &str[i];
}

static int StringCmp(const std::wstring &a, const std::wstring &b)
{
	int cmp = 0;
	if( a < b ) cmp = -1;
	else if( a > b ) cmp = 1;
	return cmp;
}

void RegisterStdString_Native(asIScriptEngine *engine)
{
	int r;

	// Register the std::wstring type
	r = engine->RegisterObjectType("string", sizeof(std::wstring), asOBJ_VALUE | asOBJ_APP_CLASS_CDA); assert( r >= 0 );

	// Register the std::wstring factory
	r = engine->RegisterStringFactory("string", asFUNCTION(StringFactory), asCALL_CDECL); assert( r >= 0 );

	// Register the object operator overloads
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f()",                    asFUNCTION(ConstructString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_DESTRUCT,   "void f()",                    asFUNCTION(DestructString),  asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAssign(const string &in)", asMETHODPR(std::wstring, operator =, (const std::wstring&), std::wstring&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(const string &in)", asMETHODPR(std::wstring, operator+=, (const std::wstring&), std::wstring&), asCALL_THISCALL); assert( r >= 0 );

	{
		using namespace std;
		r = engine->RegisterObjectMethod("string", "bool opEquals(const string &in) const", asFUNCTIONPR(operator ==, (const wstring &, const wstring &), bool), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
		r = engine->RegisterObjectMethod("string", "string opAdd(const string &in) const", asFUNCTIONPR(operator +, (const wstring &, const wstring &), wstring), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	}
	r = engine->RegisterObjectMethod("string", "int opCmp(const string &in) const", asFUNCTION(StringCmp), asCALL_CDECL_OBJFIRST); assert( r >= 0 );

	// Register the object methods
#pragma warning( push )
#pragma warning( disable : 4127 )

	if( sizeof(size_t) == 4 )
	{
		r = engine->RegisterObjectMethod("string", "uint length() const", asMETHOD(std::wstring,size), asCALL_THISCALL); assert( r >= 0 );
		r = engine->RegisterObjectMethod("string", "void resize(uint)", asMETHODPR(std::wstring,resize,(size_t),void), asCALL_THISCALL); assert( r >= 0 );
	}
	else
	{
		r = engine->RegisterObjectMethod("string", "uint64 length() const", asMETHOD(std::wstring,size), asCALL_THISCALL); assert( r >= 0 );
		r = engine->RegisterObjectMethod("string", "void resize(uint64)", asMETHODPR(std::wstring,resize,(size_t),void), asCALL_THISCALL); assert( r >= 0 );
	}

#pragma warning( pop )

	// Register the index operator, both as a mutator and as an inspector
	// Note that we don't register the operator[] directory, as it doesn't do bounds checking
	r = engine->RegisterObjectMethod("string", "uint8 &opIndex(uint)", asFUNCTION(StringCharAt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "const uint8 &opIndex(uint) const", asFUNCTION(StringCharAt), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// Automatic conversion from values
	r = engine->RegisterObjectMethod("string", "string &opAssign(double)", asFUNCTION(AssignDoubleToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(double)", asFUNCTION(AddAssignDoubleToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd(double) const", asFUNCTION(AddStringDouble), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd_r(double) const", asFUNCTION(AddDoubleString), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(int)", asFUNCTION(AssignIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(int)", asFUNCTION(AddAssignIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd(int) const", asFUNCTION(AddStringInt), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd_r(int) const", asFUNCTION(AddIntString), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(uint)", asFUNCTION(AssignUIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(uint)", asFUNCTION(AddAssignUIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd(uint) const", asFUNCTION(AddStringUInt), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd_r(uint) const", asFUNCTION(AddUIntString), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(bool)", asFUNCTION(AssignBoolToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(bool)", asFUNCTION(AddAssignBoolToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd(bool) const", asFUNCTION(AddStringBool), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd_r(bool) const", asFUNCTION(AddBoolString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
}

void RegisterStdString(asIScriptEngine * engine)
{
	if (strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY"))
		RegisterStdString_Generic(engine);
	else
		RegisterStdString_Native(engine);
}

END_AS_NAMESPACE




