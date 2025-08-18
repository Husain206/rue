#include "ast.h"
#include <ostream>
#include <vector>
#include <unordered_map>
#include <optional>


typedef enum {
  Int,
  Str,
  Bool,
  Nil,
} Type;

struct Value {
   Type type = Type::Nil;
   long long i = 0;
   string s;
   bool b = false;

   static Value Int(long long v);
   static Value String(string v);
   static Value Bool(bool v);
   static Value Nil();

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


struct Interpreter {
  void run(const Node* root);

  Env env;
  ostream* out = &cout;
  void exec(const Node* n);
  void exec_block(const Node* block);
  void exec_set(const Node* n);
  void exec_print(const Node* n);

  Value eval(const Node* n);
  Value eval_id(const Node* n);
  Value eval_assign(const Node* n);
  Value eval_binary(const Node* n);
  Value coerceInt(const Value& v);
  Value plus(const Value& a, const Value& b);
  
  
};
