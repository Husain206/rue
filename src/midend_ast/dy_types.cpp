#include "dy_types.h"
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include "runtime.h"

Value Value::Int(long long v){
  Value x;
  x.type = Type::Int;
  x.i = v;
  return x;
}

Value Value::Float(double v){
  Value x;
  x.type = Type::Float;
  x.f = v;
  x.i = (long long)v;  // optional, but convenient ig
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

Value Value::Function(vector<uint32_t> ps, const Node *body){
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
  x.native_mut0 = false;
  return x;
}

Value Value::NativeMut0(CppMut0 func){
  Value x;
  x.type = Type::NativeFunc;
  x.native_mut0 = true;
  x.cpp_mut0 = func;
  return x;
}

Value Value::Array(shared_ptr<vector<Value>> elems){
    Value x;
    x.type = Type::Array;
    x.array = std::move(elems);
    return x;
}

Value Value::Struct(shared_ptr<unordered_map<uint32_t, Value>> f) {
    Value x;
    x.type = Type::Struct;
    x.fields = std::move(f);
    return x;
}

Value Value::StructDef(const std::vector<uint32_t>& field_names) {
    Value v; v.type = Type::STRUCT_DEF;
    v.struct_def_fields = field_names;
    return v;
}

Value Value::EnumDef(const vector<uint32_t> &order, shared_ptr<unordered_map<uint32_t, Value>> variants){
  Value v;
  v.type = Type::ENUM_DEF;
  v.enum_def_variants = order;
  v.enm = std::move(variants);
  return v;
}

Value Value::External(void* lib, void* fn, FFIProto* p) {
  Value x; x.type = Type::ExternalFunc; x.lib_handle = lib; x.c_fn_ptr = fn; x.proto = p;
  return x;
}

Value Value::FileHandle(std::FILE* f) {
    Value x;
    x.type = Type::File;
    x.file = f;
    return x;
}


string Value::toString() const {
  switch (type) {
    case Type::Int: return std::to_string(i);
    case Type::Float: {
        std::string s = std::to_string(f);
        while (!s.empty() && s.back() == '0') s.pop_back();
        if (!s.empty() && s.back() == '.') s.pop_back();
        return s;
    }
    case Type::Str: return s;
    case Type::Bool: return b ? "true" : "false";
    case Type::Nil: return "nil";
    case Type::Func: return "<fn>";
    case Type::NativeFunc: return "<native fn>";
    case Type::Array: {
        string s{"["};
        bool first{true};
        for(auto& n : *array){
          if(!first) s += ", ";
          s += n.toString();
          first = false;
        }
        s += "]";
        return s;
      }
    case Type::Struct: {
      string s = "{";
      bool first = true;
      for(auto& [k, v] : *fields) {
          if(!first) s += ", ";
          s += std::to_string(k) + ": " + v.toString();
          first = false;
      }
      s += "}";
      return s;
    }
    case Type::STRUCT_DEF: return "<struct_def>";
    case Type::ENUM_DEF: return "<enum>";
    case Type::ExternalFunc: return "<extern>";
    case Type::File: return "<file>";
  }
  return "nil";
}

bool Value::truthy() const {
  switch(type){
    case Type::Int: return i != 0;
    case Type::Float: return f != 0.0;
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
        array->push_back(v);
    }

    Value Value::pop() {
        if (type != Type::Array)
            throw std::runtime_error("pop() can only be called on a list");
        if (array->empty())
            throw std::runtime_error("pop() called on empty list");
        Value val = array->back();
        array->pop_back();
        return val;
    }

    size_t Value::length() {
        if (type != Type::Array)
            throw runtime_error("length() can only be called on a list");
        return array->size();
    }
