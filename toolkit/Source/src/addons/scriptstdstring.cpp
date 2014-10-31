#include <assert.h>
#include <sstream>
#include "scriptstdstring.h"
#include <string.h> // strstr

using namespace std;

BEGIN_AS_NAMESPACE

static void StringFactoryGeneric(asIScriptGeneric *gen) {
  asUINT length = gen->GetArgDWord(0);
  const char *s = (const char*)gen->GetArgAddress(1);
  string str(s, length);
  gen->SetReturnObject(&str);
}

static void ConstructStringGeneric(asIScriptGeneric * gen) {
  new (gen->GetObject()) string();
}

static void CopyConstructStringGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgObject(0));
  new (gen->GetObject()) string(*a);
}

static void DestructStringGeneric(asIScriptGeneric * gen) {
  string * ptr = static_cast<string *>(gen->GetObject());
  ptr->~string();
}

static void AssignStringGeneric(asIScriptGeneric *gen) {
  string * a = static_cast<string *>(gen->GetArgObject(0));
  string * self = static_cast<string *>(gen->GetObject());
  *self = *a;
  gen->SetReturnAddress(self);
}

static void AddAssignStringGeneric(asIScriptGeneric *gen) {
  string * a = static_cast<string *>(gen->GetArgObject(0));
  string * self = static_cast<string *>(gen->GetObject());
  *self += *a;
  gen->SetReturnAddress(self);
}

/*static void StringEqualGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgAddress(0));
  string * b = static_cast<string *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a == *b);
}*/

static void StringEqualsGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  string * b = static_cast<string *>(gen->GetArgAddress(0));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a == *b);
}

static void StringCmpGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  string * b = static_cast<string *>(gen->GetArgAddress(0));

  int cmp = 0;
  if( *a < *b ) cmp = -1;
  else if( *a > *b ) cmp = 1;

  *(int*)gen->GetAddressOfReturnLocation() = cmp;
}

/*static void StringNotEqualGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgAddress(0));
  string * b = static_cast<string *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a != *b);
}

static void StringLEqualGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgAddress(0));
  string * b = static_cast<string *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a <= *b);
}

static void StringGEqualGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgAddress(0));
  string * b = static_cast<string *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a >= *b);
}

static void StringLessThanGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgAddress(0));
  string * b = static_cast<string *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a < *b);
}

static void StringGreaterThanGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgAddress(0));
  string * b = static_cast<string *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a > *b);
}*/

static void StringAddGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  string * b = static_cast<string *>(gen->GetArgAddress(0));
  string ret_val = *a + *b;
  gen->SetReturnObject(&ret_val);
}

static void StringLengthGeneric(asIScriptGeneric * gen) {
  string * self = static_cast<string *>(gen->GetObject());
  *static_cast<size_t *>(gen->GetAddressOfReturnLocation()) = self->length();
}

static void StringResizeGeneric(asIScriptGeneric * gen) {
  string * self = static_cast<string *>(gen->GetObject());
  self->resize(*static_cast<size_t *>(gen->GetAddressOfArg(0)));
}

static void StringCharAtGeneric(asIScriptGeneric * gen) {
  unsigned int index = gen->GetArgDWord(0);
  string * self = static_cast<string *>(gen->GetObject());

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
	string *self = static_cast<string*>(gen->GetObject());
	std::stringstream sstr;
	sstr << *a;
	*self = sstr.str();
	gen->SetReturnAddress(self);
}

void AssignUInt2StringGeneric(asIScriptGeneric *gen) 
{
	unsigned int *a = static_cast<unsigned int*>(gen->GetAddressOfArg(0));
	string *self = static_cast<string*>(gen->GetObject());
	std::stringstream sstr;
	sstr << *a;
	*self = sstr.str();
	gen->SetReturnAddress(self);
}

void AssignDouble2StringGeneric(asIScriptGeneric *gen) 
{
	double *a = static_cast<double*>(gen->GetAddressOfArg(0));
	string *self = static_cast<string*>(gen->GetObject());
	std::stringstream sstr;
	sstr << *a;
	*self = sstr.str();
	gen->SetReturnAddress(self);
}

void AssignBool2StringGeneric(asIScriptGeneric *gen) 
{
	bool *a = static_cast<bool*>(gen->GetAddressOfArg(0));
	string *self = static_cast<string*>(gen->GetObject());
	std::stringstream sstr;
	sstr << (*a ? "true" : "false");
	*self = sstr.str();
	gen->SetReturnAddress(self);
}

void AddAssignDouble2StringGeneric(asIScriptGeneric * gen) {
  double * a = static_cast<double *>(gen->GetAddressOfArg(0));
  string * self = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a;
  *self += sstr.str();
  gen->SetReturnAddress(self);
}

void AddAssignInt2StringGeneric(asIScriptGeneric * gen) {
  int * a = static_cast<int *>(gen->GetAddressOfArg(0));
  string * self = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a;
  *self += sstr.str();
  gen->SetReturnAddress(self);
}

void AddAssignUInt2StringGeneric(asIScriptGeneric * gen) {
  unsigned int * a = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
  string * self = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a;
  *self += sstr.str();
  gen->SetReturnAddress(self);
}

void AddAssignBool2StringGeneric(asIScriptGeneric * gen) {
  bool * a = static_cast<bool *>(gen->GetAddressOfArg(0));
  string * self = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << (*a ? "true" : "false");
  *self += sstr.str();
  gen->SetReturnAddress(self);
}

void AddString2DoubleGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  double * b = static_cast<double *>(gen->GetAddressOfArg(0));
  std::stringstream sstr;
  sstr << *a << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddString2IntGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  int * b = static_cast<int *>(gen->GetAddressOfArg(0));
  std::stringstream sstr;
  sstr << *a << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddString2UIntGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  unsigned int * b = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
  std::stringstream sstr;
  sstr << *a << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddString2BoolGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  bool * b = static_cast<bool *>(gen->GetAddressOfArg(0));
  std::stringstream sstr;
  sstr << *a << (*b ? "true" : "false");
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddDouble2StringGeneric(asIScriptGeneric * gen) {
  double* a = static_cast<double *>(gen->GetAddressOfArg(0));
  string * b = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddInt2StringGeneric(asIScriptGeneric * gen) {
  int* a = static_cast<int *>(gen->GetAddressOfArg(0));
  string * b = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddUInt2StringGeneric(asIScriptGeneric * gen) {
  unsigned int* a = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
  string * b = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddBool2StringGeneric(asIScriptGeneric * gen) {
  bool* a = static_cast<bool *>(gen->GetAddressOfArg(0));
  string * b = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << (*a ? "true" : "false") << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

static void StringFindGeneric(asIScriptGeneric * gen) {
	std::string * a = static_cast<std::string *>(gen->GetObject());
	std::string * b = static_cast<std::string *>(gen->GetArgAddress(0));
	gen->SetReturnDWord(a->find(*b, 0));
}

static void StringSubstrGeneric(asIScriptGeneric * gen) {
	std::string * a = static_cast<std::string *>(gen->GetObject());
	std::size_t pos  = *static_cast<std::size_t*>(gen->GetAddressOfArg(0));
	std::size_t size = *static_cast<std::size_t*>(gen->GetAddressOfArg(1));
	std::string ret_val = a->substr(pos, size);
	gen->SetReturnObject(&ret_val);
}

void RegisterStdString_Generic(asIScriptEngine *engine) {
  int r;

  // Register the string type
  r = engine->RegisterObjectType("string", sizeof(string), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert( r >= 0 );

  // Register the string factory
  r = engine->RegisterStringFactory("string", asFUNCTION(StringFactoryGeneric), asCALL_GENERIC); assert( r >= 0 );

  // Register the object operator overloads
  r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f()",                    asFUNCTION(ConstructStringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(const string &in)",    asFUNCTION(CopyConstructStringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectBehaviour("string", asBEHAVE_DESTRUCT,   "void f()",                    asFUNCTION(DestructStringGeneric),  asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAssign(const string &in)", asFUNCTION(AssignStringGeneric),    asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAddAssign(const string &in)", asFUNCTION(AddAssignStringGeneric), asCALL_GENERIC); assert( r >= 0 );

  r = engine->RegisterObjectMethod("string", "bool opEquals(const string &in) const", asFUNCTION(StringEqualsGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "int opCmp(const string &in) const", asFUNCTION(StringCmpGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd(const string &in) const", asFUNCTION(StringAddGeneric), asCALL_GENERIC); assert( r >= 0 );

#pragma warning( push )
#pragma warning( disable : 4127 )
  // Register the object methods
  if( sizeof(size_t) == 4 )
#pragma warning( pop )
  {
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

void RegisterStdString(asIScriptEngine * engine)
{
	RegisterStdString_Generic(engine);
}

END_AS_NAMESPACE




