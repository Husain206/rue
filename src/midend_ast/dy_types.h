#pragma once
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

typedef enum {
  Int,
  Str,
  Bool,
  Nil,
  Func,
  NativeFunc,
  Array,
  Struct,
  STRUCT_DEF,
} Type;

struct Node;
struct Env;
struct Value;

typedef Value(*CppFunc)(vector<Value>);

struct Value {
   Type type = Type::Nil;
   long long i = 0;
   string s;
   bool b = false;
   CppFunc cpp_func = nullptr;

   vector<string> params;
   const Node* body = nullptr;

   vector<Value> array;
   unordered_map<string, Value> fields;
   vector<string> struct_def_fields; 

   static Value Int(long long v);
   static Value String(string v);
   static Value Bool(bool v);
   static Value Nil();
   static Value Function(vector<string> ps, const Node* body);
   static Value NativeFunction(CppFunc func);
   static Value Array(vector<Value> elems);
   static Value Struct(unordered_map<string, Value> fields);
   static Value StructDef(const vector<string>& field_names);


   void push(const Value& v);
   Value pop();
   size_t length();

   string toString() const;
   bool truthy() const;
};
