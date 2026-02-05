#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include "dy_types.h"

struct Env {
  vector<unordered_map<uint32_t, Value>> scopes;

  void push();
  void pop();
  // bool define(const string& name, const Value& v);
  // bool assign(const string& name, const Value& v);
  // optional<Value> get(const string& name) const;
  bool define(const uint32_t& name, const Value& v);
  bool assign(const uint32_t& name, const Value& v);
  const Value* get(const uint32_t& name) const;
  Value*       get_mut(uint32_t name_id);

};
