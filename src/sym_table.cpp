#include "sym_table.h"
#include <algorithm>
#include <optional>

Value Value::Int(long long v){
  Value x;
  x.type = Type::Int;
  x.i = v;
  return x;
}

Value Value::String(string v){
  Value x;
  x.type = Type::Str;
  x.s = v;
  return x;
}

Value Value::Bool(bool v){
  Value x;
  x.type = Type::Bool;
  x.b = v;
  return x;
}

Value Value::Nil(){
  return Value{};
}

Value Value::Function(vector<string> ps, const Node *body){
  Value x;
  x.type = Type::Func;
  x.params = std::move(ps);
  x.body = body;
  return x;
}

string Value::toString() const {
  switch (type) {
    case Type::Int: return std::to_string(i);
    case Type::Str: return s;
    case Type::Bool: return b ? "true" : "false";
    case Type::Nil: return "nil";
    case Type::Func: return "<fn>";
  }
  return "nil";
}

bool Value::truthy() const {
  switch(type){
    case Type::Int: return i != 0;
    case Type::Str: return !s.empty();
    case Type::Bool: return b;
    case Type::Nil: return false;
  }
  return false;
}


void Env::push(){
  scopes.push_back({});
}

void Env::pop(){
  if(!scopes.empty()) scopes.pop_back();
}

bool Env::define(const string& name, const Value& v){
  if(scopes.empty()) scopes.push_back({});
   scopes.back()[name] = v;
   return true;
}

bool Env::assign(const string& name, const Value& v){
  for(auto it = scopes.rbegin(); it != scopes.rend(); it++){
    auto f = it->find(name);
    if(f != it->end()) { f->second = v; return true; }
  }
  return define(name, v);
}

optional<Value> Env::get(const string& name) const {
    for(auto it = scopes.rbegin(); it != scopes.rend(); it++){
      auto f = it->find(name);
      if(f != it->end()) { return f->second; }
    }
    return nullopt;
}
