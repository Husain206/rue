#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include "dy_types.h"

struct Env {
  vector<unordered_map<string, Value>> scopes;

  void push();
  void pop();
  bool define(const string& name, const Value& v);
  bool assign(const string& name, const Value& v);
  optional<Value> get(const string& name) const;
};
