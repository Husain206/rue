#pragma once
#include "dy_types.h"


struct built_ins {
  // built-ins
  static Value brint(vector<Value>& args);
  static Value inbut(vector<Value>& args);
  static Value bow(vector<Value>& args);
  static Value len(vector<Value>& args);
  static Value num(vector<Value>& args);
  static Value chr(vector<Value>& args);
  static Value ord(vector<Value>& args);
  static Value bush(vector<Value>& args);
  static Value bob(vector<Value>& args);
  static Value bush_in(Value* target, vector<Value>& args);
  static Value bob_in(Value* target, vector<Value>& args);
  static Value init_array(vector<Value>& args);
  static Value ext(vector<Value>& args);
  static Value fopen_builtin(vector<Value>& args);
  static Value fread_builtin(vector<Value>& args);
  static Value fwrite_builtin(vector<Value>& args);
  static Value fclose_builtin(vector<Value>& args);

};
