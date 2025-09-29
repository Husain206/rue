#include "built-ins.h"
#include <cmath>
#include <iostream>
#include "runtime.h"

Value built_ins::brint(vector<Value> args) {
  for(auto arg : args)
    cout << arg.toString() << " ";
  cout << endl;
  return Value::Nil();
}

Value built_ins::inbut(vector<Value> args){
  string prompt;
  if(args.size() == 1) prompt = args[0].toString();
  else if(args.size() > 1) throw runtime_error("input() takes at most one argument");
  cout << prompt;

  string line;
  if(!getline(cin, line)) throw runtime_error("failed to read input");

  return Value::String(line);
}

Value built_ins::bow(vector<Value> args){
  if (args.size() != 2) throw runtime_error("pow expects 2 args");
    long long base = Interpreter::coerceInt(args[0]).i;
    long long exp  = Interpreter::coerceInt(args[1]).i;

    long long result = std::pow(base, exp);
    return Value::Int(result);
 }

 Value built_ins::len(vector<Value> args){
   if(args.size() != 1) throw runtime_error("len() takes at most one argument");

   Value v = args[0];
   switch (v.type) {
     case Str: return Value::Int(v.s.size());
     case Array: return Value::Int(v.array.size());
      default:  throw runtime_error("len() not supported for this type");
   }
   
 }

 Value built_ins::num(vector<Value> args){
    if(args.size() != 1) 
        throw runtime_error("num() expects exactly one argument");

    Value v = args[0];
    switch(v.type){
        case Type::Int: 
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

Value built_ins::ord(vector<Value> args) {
    if(args.size() != 1 || args[0].type != Type::Str || args[0].s.size() != 1)
        throw runtime_error("ord() expects a single-character string");
    return Value::Int(static_cast<int>(args[0].s[0]));
}

Value built_ins::chr(vector<Value> args){
    if(args.size() != 1) throw runtime_error("chr() expects one integer argument");
    int code = Interpreter::coerceInt(args[0]).i;
    return Value::String(std::string(1, static_cast<unsigned char>(code)));
}

Value built_ins::bush(std::vector<Value> args) {
    if (args.size() < 2) throw runtime_error("push() expects at least a list and one value");
    Value list = args[0];
    if (list.type != Type::Array)
        throw runtime_error("first argument must be an array");

    for (size_t i = 1; i < args.size(); i++)
        list.push(args[i]);

    return list;  // return modified array // i.e. a copy
}


Value built_ins::bob(std::vector<Value> args) {
    if (args.size() != 1)
        throw runtime_error("pop() expects a single array argument");

    Value list = args[0];
    if (list.type != Type::Array)
        throw runtime_error("pop() expects an array");

    if (list.array.empty())
        throw runtime_error("pop() called on empty array");

    Value val = list.array.back();
    list.array.pop_back();

    return list;  // return modified array // i.e. a copy
}

Value built_ins::init_array(std::vector<Value> args) {
    if (args.size() != 2)
        throw std::runtime_error("array(size, init) expects exactly 2 arguments");

    Value sizeVal = Interpreter::coerceInt(args[0]);
    if (sizeVal.i < 0)
        throw std::runtime_error("array size must be non-negative");

    std::vector<Value> elems(sizeVal.i, args[1]);
    return Value::Array(elems);
}

Value built_ins::ext(vector<Value> args){
  if(args.size() != 1) throw runtime_error("exit() accpets a single argument");
  long long n = Interpreter::coerceInt(args[0]).i;
  exit(n);
  return Value::Nil();
}
