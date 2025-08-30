#include "sym_table.h"
#include <optional>

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
