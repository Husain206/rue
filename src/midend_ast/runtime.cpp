#include "runtime.h"
#include "sym_table.h"
#include "dy_types.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ffi.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "built-ins.h"
#include "ffi.h"
#include "ffi_sig.h"
#include "../frontend/lexer.h"
#include "../frontend/parser.h"
#include <fstream>
#include <sstream>

#define intern(s) id_lookup.intern(s)

static ffi_type* value_to_ffi_type(const Value& v){
  switch(v.type){
    case Type::Int:  return &ffi_type_sint32;   
    case Type::Float: return &ffi_type_double;
    case Type::Str:  return &ffi_type_pointer;  
    case Type::Bool: return &ffi_type_sint32;   
    default:         return &ffi_type_pointer;  
  }
}

Value Interpreter::callFunction(const Value& fnVal, const std::vector<Value>& args) {
    if (fnVal.type != Type::Func)
        throw runtime_error("callFunction: not a function");

    if (fnVal.params.size() != args.size())
        throw runtime_error("argument count mismatch when calling function");

    env.push();
    for (size_t i = 0; i < fnVal.params.size(); ++i)
        env.define(fnVal.params[i], args[i]);

    Value ret = Value::Nil();
    try {
        exec(fnVal.body);
    } catch (ReturnException& re) {
        ret = re.value;
    }

    env.pop();
    return ret;
}

static std::string dirname(const std::string& path) {
  auto pos = path.find_last_of("/\\");
  if (pos == std::string::npos) return ".";
  return path.substr(0, pos);
}

Value* Interpreter::resolve_lvalue(const Node* n) {
  if (!n) return nullptr;
  if (n->type == n_id) {
    return env.get_mut(n->interner);
  }
  return nullptr; // identifier only for now
}


void Interpreter::run(const Node* root, const std::vector<std::string>& args){
  if(!root || root->type != n_block) throw runtime_error("program must be a block");

  // define built-ins
  env.define(intern("true"),  Value::Bool(true));
  env.define(intern("false"), Value::Bool(false));
  env.define(intern("brint"), Value::NativeFunction(built_ins::brint));
  env.define(intern("bow" ),  Value::NativeFunction(built_ins::bow));
  env.define(intern("inbut"), Value::NativeFunction(built_ins::inbut));
  env.define(intern("len"),   Value::NativeFunction(built_ins::len));
  env.define(intern("num"),   Value::NativeFunction(built_ins::num));
  env.define(intern("chr"),   Value::NativeFunction(built_ins::chr));
  env.define(intern("ord"),   Value::NativeFunction(built_ins::ord));
  env.define(intern("exit"),  Value::NativeFunction(built_ins::ext));
  env.define(intern("open"),  Value::NativeFunction(built_ins::fopen_builtin));
  env.define(intern("close"), Value::NativeFunction(built_ins::fclose_builtin));
  env.define(intern("read"),  Value::NativeFunction(built_ins::fread_builtin));
  env.define(intern("write"), Value::NativeFunction(built_ins::fwrite_builtin));
  env.define(intern("bush"),  Value::NativeFunction(built_ins::bush));
  env.define(intern("bob"),   Value::NativeFunction(built_ins::bob));

  env.define(intern("bush&"),  Value::NativeMut0(built_ins::bush_in));
  env.define(intern("bob&"),   Value::NativeMut0(built_ins::bob_in));
  env.define(intern("array"), Value::NativeFunction(built_ins::init_array));

  exec_block(root, true);

  auto entryOpt = env.get(intern("main"));
  if (!entryOpt) {
      // exec_block(root, false);
      // return;
    for (auto& ch : root->children) {
      if (ch->type == n_fn_dec || ch->type == n_set || ch->type == n_struct ||
          ch->type == n_enum  || ch->type == n_extern || ch->type == n_import) {
        continue; // skip: already declared
      }
      exec(ch.get());
    }
    return;

  }
  Value entry = *entryOpt;
  if (entry.type != Type::Func) {
      throw runtime_error("'main' must be a function");
  }

  std::vector<Value> main_args;
  shared_ptr<vector<Value>> arg_values = make_shared<vector<Value>>();
  if(!entry.params.empty()){
  for(const auto& s : args)
    arg_values->push_back(Value::String(s));

  main_args.push_back(Value::Array(arg_values));
  }

  Value result = callFunction(entry, main_args);
  if(result.type == Type::Int) {
      int code = (int) result.i;
      if (code != 0) exit(code);
  }
}

void Interpreter::exec(const Node* n){
  if(!n) return;

  switch (n->type) {
    case n_set:         exec_set(n); break;
    case n_print:       exec_print(n); break;
    case n_expr_stmt:   (void)eval(n->children[0].get()); break;
    case n_ala:         exec_ala(n); break;
    case n_if:          exec_if(n);  break;
    case n_else:        exec_block(n); break;
    case n_for_loop:    exec_for_loop(n); break;
    case n_block:       exec_block(n); break;
    case n_fn_dec:      exec_fn(n); break;
    case n_return:      exec_return(n); break;
    case n_input:       exec_input(n); break;
    case n_struct:      exec_struct(n); break;
    case n_enum:        exec_enum(n); break;
    case n_extern:      exec_extern(n); break;
    case n_import:      exec_import(n); break;

    default: throw runtime_error("exec: unsupported node " + n->lexeme);
  }
}

void Interpreter::exec_block(const Node* block, bool declareOnly){
  for (auto& ch : block->children) {
    if (declareOnly) {
      if (ch->type == n_fn_dec || ch->type == n_set || ch->type == n_struct || ch->type == n_enum || ch->type == n_extern || ch->type == n_import) {
        exec(ch.get());
      }
    } else {
      exec(ch.get());
    }
  }
}

void Interpreter::exec_set(const Node* n){
  // children: [Ident(name), expr]
  const Node* id = n->children[0].get();
  if(id->type != n_id) throw runtime_error("lhs must be an id");

  Value v = Value::Nil();
  if(n->children.size() > 1 && n->children[1]){
    v = eval(n->children[1].get());
  }
  env.define(id->interner, v);
}

void Interpreter::exec_print(const Node* n){
  for(size_t i{0}; i < n->children.size(); i++){
    Value v = eval(n->children[i].get());
    std::cout << v.toString();
    if (i + 1 < n->children.size()) std::cout << " ";
  }
}

void Interpreter::exec_input(const Node* n){
  // children: [ident]
  const Node* id = n->children[0].get();
  if(id->type != n_id) throw runtime_error("input target must be an identifier");
  std::string line;
  if(!std::getline(std::cin, line)){
    throw runtime_error("failed to read input");
  }
  Value v = Value::String(line);
  env.define(id->interner, v);
}

void Interpreter::exec_ala(const Node* n){
  // children: [cond, body]
  while(eval(n->children[0].get()).truthy()){
    env.push();
    exec(n->children[1].get());
    env.pop();
  }
}

void Interpreter::exec_if(const Node* n){
  // children: [cond, thenBlock, (optional) elseBlock]
  Value c = eval(n->children[0].get());
  if(c.truthy()) exec(n->children[1].get());
  else if(n->children.size() >= 3) exec(n->children[2].get());
}

void Interpreter::exec_for_loop(const Node* n){
  // children: [init (nullable), cond, inc, body]
  env.push();
  const Node* init = n->children[0].get();
  const Node* cond = n->children[1].get();
  const Node* inc = n->children[2].get();
  const Node* body = n->children[3].get();

  if(init) exec(init);

  while(eval(cond).truthy()){
    exec(body);
    (void)eval(inc);
  }
  env.pop();
}

void Interpreter::exec_fn(const Node* n){
  // children: [params..., body]
  std::vector<uint32_t> ps;
  for(size_t i{0}; i < n->children.size()-1; i++){
    ps.push_back(n->children[i]->interner);
  }
  const Node* body = n->children.back().get();
  Value fn = Value::Function(ps, body);
  env.define(n->interner, fn);
}

void Interpreter::exec_return(const Node* n){
  Value v = Value::Nil();
  if(!n->children.empty())
    v = eval(n->children[0].get());
  throw ReturnException(v);
}

void Interpreter::exec_struct(const Node* n){
  std::vector<uint32_t> fields;
  for (auto& ch : n->children) {
    if (ch->type != n_id) throw runtime_error("struct field must be an identifier");
    fields.push_back(ch->interner);
  }
  env.define(n->interner, Value::StructDef(fields));
}

// void Interpreter::exec_enum(const Node* n) {
//   std::unordered_map<uint32_t, Value> variants;

//   for (size_t i = 0; i < n->children.size(); i++) {
//     const Node* variant = n->children[i].get();
//     if (variant->type != n_id)
//       throw runtime_error("enum variant must be identifier");

//     variants[variant->interner] = Value::Int(i);
//     // also bind variant as global
//     env.define(variant->interner, Value::Int(i));
//   }

//   // store the enum object itself
//   env.define(n->interner, Value::Enum(variants));
// }

void Interpreter::exec_enum(const Node* n) {
  if (n->interner == UINT32_MAX)
    throw runtime_error("enum must have a name");

  auto variants = make_shared<unordered_map<uint32_t, Value>>();
  variants->reserve(n->children.size());

  vector<uint32_t> order;
  order.reserve(n->children.size());

  for (size_t i = 0; i < n->children.size(); ++i) {
    const Node* variant = n->children[i].get();
    if (!variant || variant->type != n_id)
      throw runtime_error("enum variant must be identifier");

    uint32_t vid = variant->interner;

    Value v = Value::Int((long long)i);

    if (variants->find(vid) != variants->end())
      throw runtime_error("duplicate enum variant: " + variant->lexeme);

    (*variants)[vid] = v;
    order.push_back(vid);

    if (env.get(vid)) throw runtime_error("name already defined: " + variant->lexeme);
    env.define(vid, v);
  }

  env.define(n->interner, Value::EnumDef(order, variants));
}


void Interpreter::exec_extern(const Node* n){
  //  children=1: [sig] -> libc + symbol = n->lexeme
  //  children=2: [lib, sig] -> symbol = n->lexeme
  //  children=3: [lib, sym, sig]
  std::string lib, sym, sig;
  if(n->children.size()==1){
    lib = libc_default();
    sym = n->lexeme;
    sig = n->children[0]->lexeme;
  } else if(n->children.size()==2){
    lib = n->children[0]->lexeme;
    sym = n->lexeme;
    sig = n->children[1]->lexeme;
  } else if(n->children.size()==3){
    lib = n->children[0]->lexeme;
    sym = n->children[1]->lexeme;
    sig = n->children[2]->lexeme;
  } else {
    throw std::runtime_error("extern: bad arity");
  }

  void* h = ffi_open(lib);
  void* fn = ffi_symbol(h, sym.c_str());

  FFIParsed ps = parse_sig(sig);
  auto* p = new FFIProto();
  p->ret_type    = ps.ret;
  p->fixed_types = ps.fixed;
  p->fixed_arity = ps.fixed_arity;
  p->is_variadic = ps.variadic;

  // optionally precompute fixed cif for non-variadic calls (not manditory)
  if(!p->is_variadic){
    if(ffi_prep_cif(&p->fixed_cif, FFI_DEFAULT_ABI,
                    (unsigned int)p->fixed_types.size(),
                    p->ret_type,
                    p->fixed_types.data()) != FFI_OK)
      throw std::runtime_error("ffi_prep_cif failed");
  }

  env.define(n->interner, Value::External(h, fn, p));
}


void Interpreter::exec_import(const Node* n){
  std::string rel = n->lexeme;            // "math.rue"
  std::string full_path;

  // if its not absolute, resolve relative to current module_dir
  if (!rel.empty() && rel[0] != '/' && rel[0] != '.') {
    full_path = module_dir + "/" + rel;   // "tests/math.rue"
  } else {
    full_path = rel;
  }

  if (imported[full_path]) {
    return;
  }
  imported[full_path] = true;

  std::ifstream file(full_path);
  if (!file.is_open())
    throw std::runtime_error("could not open import file: " + full_path);

  std::ostringstream buf;
  buf << file.rdbuf();
  std::string src = buf.str();

  Lexer lex;
  auto toks = lex.tokenize(src);
  Parser parser;
  parser.ids = &id_lookup;
  parser.tokens = std::move(toks);
  auto ast = parser.parse();

  modules.push_back(std::move(ast));
  const Node* root = modules.back().get();

  // temporarily switch module_dir for nested imports from this file
  std::string old_dir = module_dir;
  module_dir = dirname(full_path);

  exec_block(root, /*declareOnly=*/true);

  module_dir = old_dir;
}

Value Interpreter::eval(const Node* n){
  switch (n->type) {
    case n_num:    return eval_num(n);
    case n_str:    return Value::String(n->lexeme);
    case n_id:     return eval_id(n);
    case n_binary: return eval_binary(n);
    case n_unary:  return eval_unary(n);
    case n_assign: return eval_assign(n);
    case n_fn_call:return eval_call(n);
    case n_ternary:return eval_ternary(n);
    case n_array:  return eval_array(n);
    case n_index:  return eval_index(n);
    case n_struct_init: return eval_struct_init(n);
    case n_struct_field:return eval_struct_index(n);
    default: throw std::runtime_error("eval: unsupported node kind");
  }
}

Value Interpreter::eval_num(const Node* n){
  const std::string& s = n->lexeme;
    // crude check: if it has '.' or exponent, treat as float
    if (s.find('.') != std::string::npos ||
        s.find('e') != std::string::npos ||
        s.find('E') != std::string::npos) {
        return Value::Float(std::stod(s));
    } else {
        return Value::Int(stoll(s));
    }
}

Value Interpreter::eval_id(const Node* n){
  auto v = env.get(n->interner);
  if(!v) throw runtime_error("undefined variable: " + n->lexeme);
  return *v;
}

Value Interpreter::eval_assign(const Node* n){
  // children: [lhs, rhs]; lhs must be Ident
  const Node* lhs = n->children[0].get();
  const Node* rhs = n->children[1].get();
  Value v = eval(rhs);

  if(lhs->type == n_id) {
    env.assign(lhs->interner, v);
    return v;
  } else if(lhs->type == n_index) {
    Value arr = eval(lhs->children[0].get());
    Value idx = coerceInt(eval(lhs->children[1].get()));
    if(arr.type != Type::Array) throw runtime_error("not an array");
    if(idx.i < 0 || idx.i >= (long long)arr.array->size())
      throw runtime_error("array index out of bounds");
    arr.array->at(idx.i) = v;
    // write back to variable
    const Node* arrNode = lhs->children[0].get();
    if(arrNode->type != n_id) throw runtime_error("array assignment unsupported");
    env.assign(arrNode->interner, arr);
    return v;
  } else if (lhs->type == n_struct_field) {
    const Node* baseNode = lhs->children[0].get();
    const Node* fieldNode = lhs->children[1].get();
    if (fieldNode->type != n_id) throw runtime_error("expected field name");

    uint32_t fieldName = fieldNode->interner;
    Value newVal = eval(rhs);

    if (baseNode->type == n_id) {
      auto baseOpt = env.get(baseNode->interner);
      if (!baseOpt) throw runtime_error("undefined variable: " + baseNode->lexeme);
      Value baseVal = *baseOpt;
      if (baseVal.type != Type::Struct) throw runtime_error("not a struct instance");
      baseVal.fields->at(fieldName) = newVal;
      env.assign(baseNode->interner, baseVal);
      return newVal;
    } else {
      throw runtime_error("struct field assignment currently only supported on identifiers");
    }
  }

  throw runtime_error("invalid assignment target");
}

Value Interpreter::eval_unary(const Node* n){
  const Node* a = n->children[0].get();
  Value v = eval(a);
  switch (n->op) {
    case PLUS:   return coerceInt(v);
    case MINUS:  return Value::Int(-coerceInt(v).i);
    case NOT:    return Value::Bool(!v.truthy());
    case ADDR:
    case DEREF:
      throw std::runtime_error("unary operator not supported yet: " + n->lexeme);
    case INC:
    case DEC: {
      if (a->type != n_id)
        throw std::runtime_error("INC/DEC operand must be an identifier");

      auto v0 = env.get(a->interner);
      if (!v0) throw std::runtime_error("undefined variable: " + a->lexeme);

      long long delta = (n->op == INC ? 1 : -1);
      Value newVal = Value::Int(coerceInt(*v0).i + delta);

      env.assign(a->interner, newVal);
      return newVal;
    }
    case BITWISE_NOT: return Value::Int(~coerceInt(v).i);
    default:
      throw std::runtime_error("unknown unary op");
  }
}

Value Interpreter::eval_binary(const Node* n){

   if (n->op == AND) {
    Value l = eval(n->children[0].get());
    if (!l.truthy()) return Value::Bool(false);
    return Value::Bool(eval(n->children[1].get()).truthy());
  }
  if (n->op == OR) {
    Value l = eval(n->children[0].get());
    if (l.truthy()) return Value::Bool(true);
    return Value::Bool(eval(n->children[1].get()).truthy());
  }
  
  Value l = eval(n->children[0].get());
  Value r = eval(n->children[1].get());
  const Node* lhs = n->children[0].get();

  bool isFloat = (l.type == Type::Float || r.type == Type::Float);
  double ld = 0.0, rd = 0.0;
  long long li = 0, ri = 0;

  if (isFloat) {
    ld = coerceFloat(l).f;
    rd = coerceFloat(r).f;
  } else {
    li = coerceInt(l).i;
    ri = coerceInt(r).i;
  }

  switch (n->op) {
    case PLUS:  return plus(l, r);

    case MINUS:
      if (isFloat) return Value::Float(ld - rd);
      return Value::Int(li - ri);

    case STAR:
      if (isFloat) return Value::Float(ld * rd);
      return Value::Int(li * ri);

    case SLASH: {
      if (isFloat) {
        if (rd == 0.0) throw runtime_error("division by zero");
        return Value::Float(ld / rd);
      } else {
        if (ri == 0) throw runtime_error("divison by zero");
        return Value::Int(li / ri);
      }
    }

    case POW: {
      double res = std::pow(coerceFloat(l).f, coerceFloat(r).f);
      return Value::Float(res);
    }

    case MOD: {
      if (isFloat) {
        if (rd == 0.0) throw std::runtime_error("mod by zero");
        return Value::Float(std::fmod(ld, rd));
      } else {
        if (ri == 0) throw std::runtime_error("mod by zero");
        return Value::Int(li % ri);
      }
    }

    case LST:
      return Value::Bool(isFloat ? (ld < rd) : (li < ri));
    case GRT:
      return Value::Bool(isFloat ? (ld > rd) : (li > ri));
    case LQ:
      return Value::Bool(isFloat ? (ld <= rd) : (li <= ri));
    case GQ:
      return Value::Bool(isFloat ? (ld >= rd) : (li >= ri));

    case EQEQ:   return Value::Bool(equals(l, r));
    case NOTEQ:  return Value::Bool(!equals(l, r));
   
    case PLUSEQ:  return Value::Int(env.assign(lhs->interner, Value::Int(coerceInt(l).i + coerceInt(r).i)));
    case MINUSEQ: return Value::Int(env.assign(lhs->interner, Value::Int(coerceInt(l).i - coerceInt(r).i)));
    case MULEQ:   return Value::Int(env.assign(lhs->interner, Value::Int(coerceInt(l).i * coerceInt(r).i)));
    case DIVEQ:   return Value::Int(env.assign(lhs->interner, Value::Int(coerceInt(l).i / coerceInt(r).i)));
    case MODEQ:   return Value::Int(env.assign(lhs->interner, Value::Int(coerceInt(l).i % coerceInt(r).i)));

    case ADDR:           return Value::Int(coerceInt(l).i & coerceInt(r).i);
    case BITWISE_OR:     return Value::Int(coerceInt(l).i | coerceInt(r).i);
    case BITWISE_XOR:    return Value::Int(coerceInt(l).i ^ coerceInt(r).i);
    case BITWISE_LEFT_SHIFT:  return Value::Int(coerceInt(l).i << coerceInt(r).i);
    case BITWISE_RIGHT_SHIFT: return Value::Int(coerceInt(l).i >> coerceInt(r).i);

    default: throw std::runtime_error("unknown binary op: " + n->lexeme);
  }
}

Value Interpreter::eval_call(const Node* n) {
  Value callee = eval(n->children[0].get());

  if (callee.type == Type::NativeFunc) {
    // MUTATES ARG0 (in-place) :"D
    if (callee.native_mut0) {
      if (n->children.size() < 2)
        throw std::runtime_error("builtin expects at least 1 argument");
  
      Value* target = resolve_lvalue(n->children[1].get());
      if (!target)
        throw std::runtime_error("first argument must be an identifier");
  
      std::vector<Value> rest;
      for (size_t i = 2; i < n->children.size(); ++i)
        rest.push_back(eval(n->children[i].get()));
  
      return callee.cpp_mut0(target, rest);
    }
  
    // NORMAL :"D
    std::vector<Value> args;
    for (size_t i = 1; i < n->children.size(); ++i)
      args.push_back(eval(n->children[i].get()));
    return callee.cpp_func(args);
  }

  if (callee.type == Type::Func) {
    std::vector<Value> args;
    for(size_t i=1;i<n->children.size();++i) args.push_back(eval(n->children[i].get()));
    return callFunction(callee, args);
  }

  if (callee.type == Type::ExternalFunc) {
    if(!callee.c_fn_ptr || !callee.proto) throw std::runtime_error("extern fn not initialized");
    auto* P = callee.proto;

    std::vector<Value> argv;
    argv.reserve(n->children.size()-1);
    for(size_t i=1;i<n->children.size();++i)
      argv.push_back(eval(n->children[i].get()));

    std::vector<ffi_type*> arg_types;
    std::vector<void*>     arg_ptrs;
    arg_types.reserve(argv.size());
    arg_ptrs.reserve(argv.size());

    std::vector<long long>   i64s;    i64s.reserve(argv.size());
    std::vector<int32_t>     i32s;    i32s.reserve(argv.size());
    std::vector<double>      doubles; doubles.reserve(argv.size());
    std::vector<void*>       ptrvals; ptrvals.reserve(argv.size());

    auto push_val_fixed = [&](ffi_type* t, const Value& v){
      if (t == &ffi_type_sint64) {
        i64s.push_back(coerceInt(v).i);
        arg_types.push_back(t);
        arg_ptrs.push_back(&i64s.back());
        return;
      }
      if (t == &ffi_type_sint32) {
        i32s.push_back(static_cast<int32_t>(coerceInt(v).i));
        arg_types.push_back(t);
        arg_ptrs.push_back(&i32s.back());
        return;
      }

      if (t == &ffi_type_pointer) {
        void* p = nullptr;
        if (v.type == Type::Str) {
          p = (void*)v.s.c_str();
        }
        ptrvals.push_back(p);
        arg_types.push_back(t);
        arg_ptrs.push_back(&ptrvals.back());
        return;
      }

      if (t == &ffi_type_double) {
        double dv = 0.0;
        if (v.type == Type::Float)      dv = v.f;                      
        else if (v.type == Type::Int)   dv = static_cast<double>(v.i);
        else if (v.type == Type::Str)   dv = std::stod(v.s);
        else throw std::runtime_error("double arg expected");
        doubles.push_back(dv);
        arg_types.push_back(t);
        arg_ptrs.push_back(&doubles.back());
        return;
      }

      
      throw std::runtime_error("unsupported ffi arg type");
    };

    if(argv.size() < P->fixed_arity)
      throw std::runtime_error("not enough arguments for extern call");

    for(size_t i=0;i<P->fixed_arity;i++){
      push_val_fixed(P->fixed_types[i], argv[i]);
    }

    if(P->is_variadic){
      for(size_t i=P->fixed_arity;i<argv.size();++i){
        const Value& v = argv[i];
        ffi_type* t = value_to_ffi_type(v);
        push_val_fixed(t, v);
      }
    } else if(argv.size() > P->fixed_arity){
      throw std::runtime_error("too many arguments");
    }

    ffi_cif cif;
    if(P->is_variadic){
      if(ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI,
                          (unsigned int)P->fixed_arity,
                          (unsigned int)arg_types.size(),
                          P->ret_type, arg_types.data()) != FFI_OK)
        throw std::runtime_error("ffi_prep_cif_var failed");
    } else {
      cif = P->fixed_cif; 
    }

    long long i64ret = 0;
    int32_t   i32ret = 0;
    double    dret   = 0.0;
    void*     pret   = nullptr;
    void*     rptr   = nullptr;

    if      (P->ret_type == &ffi_type_sint64)  rptr = &i64ret;
    else if (P->ret_type == &ffi_type_sint32)  rptr = &i32ret;
    else if (P->ret_type == &ffi_type_double)  rptr = &dret;
    else if (P->ret_type == &ffi_type_pointer) rptr = &pret;
    else if (P->ret_type == &ffi_type_void)    rptr = nullptr;
    else   throw std::runtime_error("unsupported ffi return type");

    ffi_call(&cif, FFI_FN(callee.c_fn_ptr), rptr, arg_ptrs.data());

    if      (P->ret_type == &ffi_type_sint64)  return Value::Int(i64ret);
    else if (P->ret_type == &ffi_type_sint32)  return Value::Int(i32ret);
    else if (P->ret_type == &ffi_type_double)  return Value::Float((long long)std::llround(dret)); // int-ify doubles
    else if (P->ret_type == &ffi_type_pointer) return pret ? Value::String((const char*)pret)
                                                           : Value::Nil();
    return Value::Nil();
  }

  throw std::runtime_error("not a function");
}

Value Interpreter::eval_ternary(const Node* n){
  Value cond = eval(n->children[0].get());
  if(cond.truthy()){
    return eval(n->children[1].get());
  } else return eval(n->children[2].get());
}

Value Interpreter::eval_array(const Node* n){
  shared_ptr<vector<Value>> elems = make_shared<vector<Value>>();
  elems->reserve(n->children.size());
  for(auto& ch : n->children) elems->push_back(eval(ch.get()));
  return Value::Array(elems);
}

Value Interpreter::eval_index(const Node* n){
  Value arr = eval(n->children[0].get());
  Value idx = coerceInt(eval(n->children[1].get()));
  if(arr.type == Type::Array) {
    // if(idx.i < 0 || idx.i >= (long long)arr.array.size())
    //   throw runtime_error("array index out of bounds");
    return arr.array->at(idx.i);
  }
  if (arr.type == Type::Str) {
    // if (idx.i < 0 || idx.i >= (long long)arr.s.size())
    //   throw runtime_error("string index out of bounds");
    return Value::String(std::string(1, arr.s[idx.i]));
  }

  throw runtime_error("not an array or string");
}

// Value Interpreter::eval_struct_init(const Node* n){
//   auto defOpt = env.get(n->interner);
//   if(!defOpt || defOpt->type != Type::STRUCT_DEF)
//     throw runtime_error("undefined struct type: " + n->lexeme);

//   const auto& field_names = defOpt->struct_def_fields;

//   shared_ptr<unordered_map<uint32_t, Value>> fields = make_shared<unordered_map<uint32_t, Value>>();
//   for (auto &fn : field_names) fields->at(fn) = Value::Nil();

//   if (n->children.size() > field_names.size())
//     throw runtime_error("too many initializers for struct " + n->lexeme);

//   for (size_t i = 0; i < n->children.size(); ++i) {
//     Value val = eval(n->children[i].get());
//     fields->at(field_names[i]) = val;
//   }

//   return Value::Struct(fields);
// }

Value Interpreter::eval_struct_init(const Node* n) {
  auto defOpt = env.get(n->interner);
  if (!defOpt)
    throw runtime_error("undefined type: " + n->lexeme);

  if (defOpt->type == Type::ENUM_DEF) {
    if (!n->children.empty())
      throw runtime_error("enum init only supports empty braces: " + n->lexeme + "{}");

    if (defOpt->enum_def_variants.empty())
      throw runtime_error("enum has no variants: " + n->lexeme);

    return *defOpt;
  }

  if (defOpt->type != Type::STRUCT_DEF)
    throw runtime_error("type is not a struct or enum: " + n->lexeme);

  const auto& field_names = defOpt->struct_def_fields;

  auto fields = make_shared<unordered_map<uint32_t, Value>>();
  fields->reserve(field_names.size());

  // IMPORTANT: don't use .at() on empty map
  for (auto fn : field_names) {
    (*fields)[fn] = Value::Nil();
  }

  if (n->children.size() > field_names.size())
    throw runtime_error("too many initializers for struct " + n->lexeme);

  for (size_t i = 0; i < n->children.size(); ++i) {
    Value val = eval(n->children[i].get());
    (*fields)[field_names[i]] = val; // also no .at()
  }

  return Value::Struct(fields);
}

Value Interpreter::eval_struct_index(const Node* n){
  Value base = eval(n->children[0].get());
  const uint32_t field_name = n->children[1]->interner;

  // Struct field access: person.name
  if (base.type == Type::Struct) {
    auto it = base.fields->find(field_name);
    if (it == base.fields->end())
      throw runtime_error("struct field not found: " + n->children[1]->lexeme);
    return it->second;
  }

  if (base.type == Type::ENUM_DEF) {
    if (!base.enm) throw runtime_error("enum has no variant map");
    auto it = base.enm->find(field_name);
    if (it == base.enm->end())
      throw runtime_error("enum variant field not found: " + n->children[1]->lexeme);
    return it->second;
  }


  throw runtime_error("'.' only works on struct or enum");
}


Value Interpreter::coerceInt(const Value& v){
  switch (v.type) {
    case Type::Int:  return v;
    case Type::Float: return Value::Int((long long)v.f);
    case Type::Str: {
       return Value::Int(v.s.empty() ? 0 : stoll(v.s));
      // if (v.s.empty()) return Value::Int(0);
      // try {
      //   return Value::Int(std::stoll(v.s));
      // } catch (const std::exception& e) {
      //   std::cerr << "coerceInt error: tried to parse string '"
      //             << v.s << "' as integer\n";
      //   throw std::runtime_error("cannot convert string to int: '" + v.s + "'");
      // }
    }
    case Type::Bool: return Value::Int(v.b ? 1 : 0);
    case Type::Nil:  return Value::Int(0);
    default: throw runtime_error("unknown coerce statement");
  }
}


Value Interpreter::coerceFloat(const Value& v){
  switch (v.type) {
    case Type::Float: return v;
    case Type::Int:   return Value::Float((double)v.i);
    case Type::Str:   return Value::Float(v.s.empty() ? 0.0 : std::stod(v.s));
    case Type::Bool:  return Value::Float(v.b ? 1.0 : 0.0);
    case Type::Nil:   return Value::Float(0.0);
    default: throw runtime_error("cannot convert to float");
  }
}


Value Interpreter::plus(const Value& a, const Value& b){
  if(a.type == Type::Str || b.type == Type::Str)
    return Value::String(a.toString() + b.toString());

  bool isFloat = (a.type == Type::Float || b.type == Type::Float);

  if (isFloat) {
    double ad = coerceFloat(a).f;
    double bd = coerceFloat(b).f;
    return Value::Float(ad + bd);
  }
  
  return Value::Int(coerceInt(a).i + coerceInt(b).i);
}

bool Interpreter::equals(const Value& a, const Value& b){
  if(a.type == Type::Str && b.type == Type::Str)
    return a.toString() == b.toString();
  if((a.type == Type::Float || a.type == Type::Int) &&
     (b.type == Type::Float || b.type == Type::Int))
    return coerceInt(a).i == coerceInt(b).i;
  if(a.type == Type::Bool && b.type == Type::Bool)
    return a.truthy() == b.truthy();
  return false;
}
