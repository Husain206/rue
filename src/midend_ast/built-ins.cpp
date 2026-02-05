#include "built-ins.h"
#include <cmath>
#include <iostream>
#include "dy_types.h"
#include "runtime.h"
#include <cstdio>
#include <memory>
#include <vector>

Value built_ins::brint(vector<Value>& args) {
  for(const auto& arg : args)
    cout << arg.toString() << " ";
  cout << endl;
  return Value::Nil();
}

Value built_ins::inbut(vector<Value>& args){
  string prompt;
  if(args.size() == 1) prompt = args[0].toString();
  else if(args.size() > 1) throw runtime_error("inbut() takes at most one argument");
  cout << prompt;

  string line;
  if(!getline(cin, line)) throw runtime_error("failed to read input");

  return Value::String(line);
}

Value built_ins::bow(vector<Value>& args){
  if (args.size() != 2) throw runtime_error("pow expects 2 args");
    long long base = Interpreter::coerceInt(args[0]).i;
    long long exp  = Interpreter::coerceInt(args[1]).i;

    long long result = std::pow(base, exp);
    return Value::Int(result);
 }

 Value built_ins::len(vector<Value>& args){
   if(args.size() != 1) throw runtime_error("len() takes at most one argument");

   Value v = args[0];
   switch (v.type) {
     case Str: return Value::Int(v.s.size());
     case Array: return Value::Int(v.array->size());
      default:  throw runtime_error("len() not supported for this type");
   }
   
 }

 Value built_ins::num(vector<Value>& args){
    if(args.size() != 1) 
        throw runtime_error("num() expects exactly one argument");

    Value v = args[0];
    switch(v.type){
        case Type::Int: 
            return v;
        case Type::Float:
            return v;
        case Type::Str: {
            if(v.s.empty()) return Value::Int(0);
            try {
                long long n = std::stoll(v.s); 
                return Value::Int(n);
            } catch(const std::invalid_argument&) {
                throw runtime_error("cannot convert string to number: " + v.s);
            } catch(const std::out_of_range&) {
                throw runtime_error("number out of range: " + v.s);
            }
        }
        case Type::Nil:
            return Value::Int(0);
        default:
            throw runtime_error("cannot convert value to number");
    }
}

Value built_ins::ord(vector<Value>& args) {
    if(args.size() != 1 || args[0].type != Type::Str || args[0].s.size() != 1)
        throw runtime_error("ord() expects a single-character string");
    return Value::Int(static_cast<int>(args[0].s[0]));
}

Value built_ins::chr(vector<Value>& args){
    if(args.size() != 1) throw runtime_error("chr() expects one integer argument");
    int code = Interpreter::coerceInt(args[0]).i;
    return Value::String(std::string(1, static_cast<unsigned char>(code)));
}

Value built_ins::bush(std::vector<Value>& args) {
    if (args.size() < 2) throw runtime_error("bush() expects at least a list and one value");
    if (args[0].type != Type::Array)
        throw runtime_error("first argument must be an array");
    Value list = args[0];
    list.array->reserve(list.array->size() + (args.size() - 1));
    for (size_t i = 1; i < args.size(); i++)
        list.array->push_back(args[i]);

    return list;  // return modified array  i.e. a copy
}


Value built_ins::bob(std::vector<Value>& args) {
    if (args.size() != 1)
        throw runtime_error("pop() expects a single array argument");
    if (args[0].type != Type::Array)
        throw runtime_error("pop() expects an array");

    Value list = args[0];

    if (list.array->empty())
        throw runtime_error("pop() called on empty array");

    Value val = list.array->back();
    list.array->pop_back();

    return list;  // return modified array // i.e. a copy
}


Value built_ins::bush_in(Value* target, vector<Value>& rest) {
  if (!target) throw runtime_error("bush(): null target");
  if (target->type != Type::Array)
    throw runtime_error("bush(): first argument must be an array");

  if (!target->array) target->array = std::make_shared<std::vector<Value>>();

  target->array->reserve(target->array->size() + rest.size());
  for (auto& v : rest) target->array->push_back(v);

  return Value::Nil(); // or return *target 
}

Value built_ins::bob_in(Value* target, vector<Value>& rest) {
  (void)rest;
  if (!target) throw runtime_error("bob(): null target");
  if (target->type != Type::Array)
    throw runtime_error("bob(): first argument must be an array");
  if (!target->array || target->array->empty())
    throw runtime_error("bob(): empty array");

  Value v = target->array->back();
  target->array->pop_back();
  return v; 
}

Value built_ins::init_array(vector<Value>& args) {
    if (args.size() != 2)
        throw std::runtime_error("array(size, init) expects exactly 2 arguments");

    Value sizeVal = Interpreter::coerceInt(args[0]);
    if (sizeVal.i < 0)
        throw std::runtime_error("array size must be non-negative");
    shared_ptr<vector<Value>> elems = make_shared<vector<Value>>(sizeVal.i, args[1]);
    return Value::Array(elems);
}

Value built_ins::ext(vector<Value>& args){
  if(args.size() != 1) throw runtime_error("exit() accpets a single argument");
  long long n = Interpreter::coerceInt(args[0]).i;
  exit(n);
  return Value::Nil();
}

Value built_ins::fopen_builtin(vector<Value>& args) {
    if (args.size() != 2)
        throw runtime_error("open(path, mode) expects 2 arguments");

    if (args[0].type != Type::Str || args[1].type != Type::Str)
        throw runtime_error("open() expects string path and string mode");

    const char* path = args[0].s.c_str();
    const char* mode = args[1].s.c_str();

    std::FILE* f = std::fopen(path, mode);
    if (!f) {
        throw runtime_error("failed to open file: " + args[0].s);
    }
    return Value::FileHandle(f);
}

Value built_ins::fclose_builtin(vector<Value>& args) {
    if (args.size() != 1 || args[0].type != Type::File)
        throw runtime_error("close(file) expects a file handle");

    std::FILE* f = args[0].file;
    if (!f) return Value::Nil();

    std::fclose(f);
    return Value::Nil();
}

Value built_ins::fread_builtin(vector<Value>& args) {
    if (args.size() != 2)
        throw runtime_error("read(file, n) expects 2 arguments");
    if (args[0].type != Type::File)
        throw runtime_error("first argument to read() must be a file");
    long long n = Interpreter::coerceInt(args[1]).i;
    if (n < 0) throw runtime_error("read() size must be non-negative");

    std::FILE* f = args[0].file;
    if (!f) throw runtime_error("read() on closed file");

    std::string buf;
    buf.resize((size_t)n);
    size_t read = std::fread(&buf[0], 1, (size_t)n, f);
    buf.resize(read);
    return Value::String(buf);
}

Value built_ins::fwrite_builtin(vector<Value>& args) {
    if (args.size() != 2)
        throw runtime_error("write(file, data) expects 2 arguments");
    if (args[0].type != Type::File)
        throw runtime_error("first argument to write() must be a file");

    std::FILE* f = args[0].file;
    if (!f) throw runtime_error("write() on closed file");

    std::string data = args[1].toString();
    size_t written = std::fwrite(data.data(), 1, data.size(), f);
    return Value::Int((long long)written);
}

