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

   static Value Int(long long v);
   static Value String(string v);
   static Value Bool(bool v);
   static Value Nil();
   static Value Function(vector<string> ps, const Node* body);
   static Value NativeFunction(CppFunc func);

   string toString() const;
   bool truthy() const;
};
