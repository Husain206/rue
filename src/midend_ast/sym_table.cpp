#include "sym_table.h"
#include "dy_types.h"

void Env::push(){
  scopes.push_back({});
  scopes.back().reserve(16);
}

void Env::pop(){
  if(!scopes.empty()) scopes.pop_back();
}

// bool Env::define(const string& name, const Value& v){
//   if(scopes.empty()) scopes.push_back({});
//    scopes.back()[name] = v;
//    return true;
// }

// bool Env::assign(const string& name, const Value& v){
//   for(auto it = scopes.rbegin(); it != scopes.rend(); it++){
//     auto f = it->find(name);
//     if(f != it->end()) { f->second = v; return true; }
//   }
//   return define(name, v);
// }

// optional<Value> Env::get(const string& name) const {
//     for(auto it = scopes.rbegin(); it != scopes.rend(); it++){
//       auto f = it->find(name);
//       if(f != it->end()) { return f->second; }
//     }
//     return nullopt;
// }

bool Env::define(const uint32_t& name, const Value& v){
  if(scopes.empty()) scopes.push_back({});
   // scopes.back()[name] = v;
   scopes.back().insert_or_assign(name, v);
   return true;
}

bool Env::assign(const uint32_t& name, const Value& v){
  for(auto it = scopes.rbegin(); it != scopes.rend(); it++){
    auto f = it->find(name);
    if(f != it->end()) { f->second = v; return true; }
  }
  return define(name, v);
}

const Value* Env::get(const uint32_t& name) const {
    for(auto it = scopes.rbegin(); it != scopes.rend(); it++){
      auto f = it->find(name);
      if(f != it->end()) { return &f->second; }
    }
    return nullptr;
}

Value* Env::get_mut(uint32_t name) {
    for(auto it = scopes.rbegin(); it != scopes.rend(); it++){
      auto f = it->find(name);
      if(f != it->end()) { return &f->second; }
    }
    return nullptr;
}
