#pragma once
#include "dy_types.h"


struct built_ins {
  // built-ins
  static Value brint(vector<Value> args);
  static Value inbut(vector<Value> args);
  static Value bow(vector<Value> args);
  static Value len(vector<Value> args);
  static Value num(vector<Value> args);
  static Value chr(vector<Value> args);
  static Value ord(vector<Value> args);
  static Value bush(vector<Value> args);
  static Value bob(vector<Value> args);
  static Value init_array(vector<Value> args);
  static Value ext(vector<Value> args);
};
