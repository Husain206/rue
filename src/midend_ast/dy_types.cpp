#include "dy_types.h"
#include <stdexcept>

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

Value Value::NativeFunction(CppFunc func) {
  Value x;
  x.type = Type::NativeFunc;
  x.cpp_func = func;
  return x;
}

Value Value::Array(vector<Value> elems){
    Value x;
    x.type = Type::Array;
    x.array = std::move(elems);
    return x;
}

string Value::toString() const {
  switch (type) {
    case Type::Int: return std::to_string(i);
    case Type::Str: return s;
    case Type::Bool: return b ? "true" : "false";
    case Type::Nil: return "nil";
    case Type::Func: return "<fn>";
    case Type::NativeFunc: return "<native fn>";
    case Type::Array: return "<array>";
  }
  return "nil";
}

bool Value::truthy() const {
  switch(type){
    case Type::Int: return i != 0;
    case Type::Str: return !s.empty();
    case Type::Bool: return b;
    case Type::Nil: return false;
      default: {};
  }
  return false;
}


 void Value::push(const Value& v) {
        if (type != Type::Array)
            throw std::runtime_error("push() can only be called on a list");
        array.push_back(v);
    }

    Value Value::pop() {
        if (type != Type::Array)
            throw std::runtime_error("pop() can only be called on a list");
        if (array.empty())
            throw std::runtime_error("pop() called on empty list");
        Value val = array.back();
        array.pop_back();
        return val;
    }

    size_t Value::length() {
        if (type != Type::Array)
            throw runtime_error("length() can only be called on a list");
        return array.size();
    }
