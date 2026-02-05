#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <ffi.h>
using namespace std;

typedef enum {
  Int,
  Float,
  Str,
  Bool,
  Nil,
  Func,
  NativeFunc,
  Array,
  Struct,
  STRUCT_DEF,
  ENUM_DEF,
  ExternalFunc,
  File,
} Type;

struct FFIProto{
  ffi_cif fixed_cif;
  bool is_variadic = false;
  vector<ffi_type*> fixed_types;
  ffi_type* ret_type = &ffi_type_void;
  size_t fixed_arity = 0;
};

using LibHandle = void*;

struct Node;
struct Env;
struct Value;

typedef Value(*CppFunc)(vector<Value>&);
typedef Value(*CppMut0)(Value* target, std::vector<Value>& rest);

struct Value {
   Type type = Type::Nil;
   long long i = 0;
   double f = 0;
   string s;
   bool b = false;
   CppFunc cpp_func = nullptr;
   CppMut0 cpp_mut0 = nullptr;
   bool native_mut0 = false;

   vector<uint32_t> params;
   const Node* body = nullptr;

   shared_ptr<vector<Value>> array;
   shared_ptr<unordered_map<uint32_t, Value>> fields;
   vector<uint32_t> struct_def_fields;

   shared_ptr<unordered_map<uint32_t, Value>> enm;
   vector<uint32_t> enum_def_variants;

   void* lib_handle = nullptr;
   void* c_fn_ptr = nullptr;  // raw C function ptr
   FFIProto* proto = nullptr; // owned heap ptr describing signature

   std::FILE* file = nullptr;

   static Value Int(long long v);
   static Value Float(double v);
   static Value String(string v);
   static Value Bool(bool v);
   static Value Nil();
   static Value Function(vector<uint32_t> ps, const Node* body);
   static Value NativeFunction(CppFunc func);
   static Value NativeMut0(CppMut0 func);
   static Value Array(shared_ptr<vector<Value>> elems);
   static Value Struct(shared_ptr<unordered_map<uint32_t, Value>> fields);
   static Value StructDef(const vector<uint32_t>& field_names);
   static Value EnumDef(const vector<uint32_t>& order, shared_ptr<unordered_map<uint32_t, Value>> variants);
   static Value External(void* lib, void* fn, FFIProto* p);
   static Value FileHandle(std::FILE* f);


   void push(const Value& v);
   Value pop();
   size_t length();

   string toString() const;
   bool truthy() const;
};
