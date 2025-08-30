#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

using namespace std;

typedef enum {
  Int,
  Str,
  Bool,
  Nil,
  Func,
} Type;

struct Node;

struct Value {
   Type type = Type::Nil;
   long long i = 0;
   string s;
   bool b = false;

   vector<string> params;
   const Node* body = nullptr;

   static Value Int(long long v);
   static Value String(string v);
   static Value Bool(bool v);
   static Value Nil();
   static Value Function(vector<string> ps, const Node* body);

   string toString() const;
   bool truthy() const;
};

struct Env {
  vector<unordered_map<string, Value>> scopes;

  void push();
  void pop();
  bool define(const string& name, const Value& v);
  bool assign(const string& name, const Value& v);
  optional<Value> get(const string& name) const;
};
