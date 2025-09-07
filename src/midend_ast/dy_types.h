#pragma once
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

   static Value Int(long long v);
   static Value String(string v);
   static Value Bool(bool v);
   static Value Nil();
   static Value Function(vector<string> ps, const Node* body);
   static Value NativeFunction(CppFunc func);
   static Value Array(vector<Value> elems);

   void push(const Value& v);
   Value pop();
   size_t length();

   string toString() const;
   bool truthy() const;
};
