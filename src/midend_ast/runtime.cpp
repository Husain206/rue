#include "runtime.h"
#include "sym_table.h"
#include "dy_types.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

Value Interpreter::brint(vector<Value> args) {
  for(auto arg : args)
    cout << arg.toString() << " ";
  cout << endl;
  return Value::Nil();
}

Value Interpreter::inbut(vector<Value> args){
  string prompt;
  if(args.size() == 1) prompt = args[0].toString();
  else if(args.size() > 1) throw runtime_error("input() takes at most one argument");
  cout << prompt;

  string line;
  if(!getline(cin, line)) throw runtime_error("failed to read input");

  return Value::String(line);
}

Value Interpreter::bow(vector<Value> args){
  if (args.size() != 2) throw runtime_error("pow expects 2 args");
    long long base = Interpreter::coerceInt(args[0]).i;
    long long exp  = Interpreter::coerceInt(args[1]).i;

    long long result = static_cast<long long>(std::pow(base, exp));
    return Value::Int(result);
 }

 Value Interpreter::len(vector<Value> args){
   if(args.size() != 1) throw runtime_error("len() takes at most one argument");

   string len = args[0].toString();
   return Value::Int(len.size());
 }

void Interpreter::run(const Node* root){
  if(!root || root->type != n_block) throw runtime_error("program must be a block");
  
    // define built-ins
    env.define("true",  Value::Bool(true));
    env.define("false", Value::Bool(false));
    env.define("brint", Value::NativeFunction(brint));
    env.define("bow", Value::NativeFunction(bow));
    env.define("inbut", Value::NativeFunction(inbut));
    env.define("len", Value::NativeFunction(len));
  exec_block(root);
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
    

      default: throw runtime_error("exec: unsupported node " + n->lexeme);
  }
}

void Interpreter::exec_block(const Node* block){
  env.push();
  for(auto& ch : block->children) exec(ch.get());
  env.pop();
}

void Interpreter::exec_set(const Node* n){
  // children: [Ident(name), expr]
  const Node* id = n->children[0].get();
    if(id->type != n_id) throw runtime_error("lhs must be an id");
    
  Value v = Value::Nil();
  if(n->children.size() > 1 && n->children[1]){
    v = eval(n->children[1].get());
  }
  env.define(id->lexeme, v);
}

void Interpreter::exec_print(const Node* n){
  Value v = eval(n->children[0].get());
  cout << v.toString() << endl;  
}

void Interpreter::exec_input(const Node* n){
  // children: [ident]
  const Node* id = n->children[0].get();
  if(id->type != n_id) throw runtime_error("input target must be an identifier");
  string line;
  if(!getline(cin, line)){
    throw runtime_error("failed to read input");
  }
  Value v = Value::String(line);
  env.define(id->lexeme, v);
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
  vector<string> ps;
  for(size_t i{0}; i < n->children.size()-1; i++){
    ps.push_back(n->children[i]->lexeme);
  }
  const Node* body = n->children.back().get();
  Value fn = Value::Function(ps, body);
  env.define(n->lexeme, fn); 
}

void Interpreter::exec_return(const Node* n){
  Value v = Value::Nil();
  if(!n->children.empty())
    v = eval(n->children[0].get());
  throw ReturnException(v);
}


Value Interpreter::eval(const Node* n){
  switch (n->type) {
    case n_num:    return Value::Int(stoll(n->lexeme));
    case n_str:    return Value::String(n->lexeme);
    case n_id:     return eval_id(n);
    case n_binary: return eval_binary(n);
    case n_unary:  return eval_unary(n);
    case n_assign: return eval_assign(n);
    case n_fn_call: return eval_call(n);
    case n_ternary: return eval_ternary(n);

      default: throw std::runtime_error("eval: unsupported node kind");
  }
}

Value Interpreter::eval_id(const Node* n){
  auto v = env.get(n->lexeme);
  if(!v) throw runtime_error("undefined variable: " + n->lexeme);
  return *v;
}

Value Interpreter::eval_assign(const Node* n){
  // children: [lhs, rhs]; lhs must be Ident
  const Node* lhs = n->children[0].get();
  const Node* rhs = n->children[1].get();
  Value v = eval(rhs);

  if(lhs->type == n_id) {
  env.assign(lhs->lexeme, v);
  return v;
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

        auto v = env.get(a->lexeme);
        if (!v) throw std::runtime_error("undefined variable: " + a->lexeme);

        long long delta = (n->op == INC ? 1 : -1);
        Value newVal = Value::Int(coerceInt(*v).i + delta);

        env.assign(a->lexeme, newVal); // update in environment
        return newVal;                 // return the updated value
       }
       case BITWISE_NOT: return Value::Int(~coerceInt(v).i);

        default:
          throw std::runtime_error("unknown unary op");
    }
}

Value Interpreter::eval_binary(const Node* n){
  Value l = eval(n->children[0].get());
  Value r = eval(n->children[1].get());

  switch (n->op) {
    case PLUS: return plus(l, r);
    case MINUS: return Value::Int(coerceInt(l).i - coerceInt(r).i);
    case STAR: return Value::Int(coerceInt(l).i * coerceInt(r).i);
    case SLASH:{
      auto R = coerceInt(r).i;
      if(R == 0) throw runtime_error("divison by zero");
      return Value::Int(coerceInt(l).i / coerceInt(r).i);
    }

    case POW:{
         return Value::Int(pow(coerceInt(l).i, coerceInt(r).i));
         }
     
    case MOD: {
        auto R = coerceInt(r).i; if(R == 0) throw std::runtime_error("mod by zero");
         return Value::Int(coerceInt(l).i % R);
      }
      
    case EQEQ:   return Value::Bool(equals(l, r));
    case NOTEQ:  return Value::Bool(!equals(l, r));
    case LST:    return Value::Bool(coerceInt(l).i < coerceInt(r).i);
    case GRT:    return Value::Bool(coerceInt(l).i > coerceInt(r).i);
    case LQ:     return Value::Bool(coerceInt(l).i <= coerceInt(r).i);
    case GQ:     return Value::Bool(coerceInt(l).i >= coerceInt(r).i);

    case AND:    return Value::Bool(l.truthy() && eval(n->children[1].get()).truthy());
    case OR:     return Value::Bool(l.truthy() || eval(n->children[1].get()).truthy());

    case ADDR:   return Value::Int(coerceInt(l).i & coerceInt(r).i);
    case BITWISE_OR: return Value::Int(coerceInt(l).i | coerceInt(r).i);
    case BITWISE_XOR: return Value::Int(coerceInt(l).i ^ coerceInt(r).i);
    case BITWISE_LEFT_SHIFT: return Value::Int(coerceInt(l).i << coerceInt(r).i);
    case BITWISE_RIGHT_SHIFT: return Value::Int(coerceInt(l).i >> coerceInt(r).i);
      
      default: throw std::runtime_error("unknown binary op: " + n->lexeme);
  }
}

Value Interpreter::eval_call(const Node* n){
    const Node* calleeNode = n->children[0].get();
    Value callee = eval(calleeNode);

    if (callee.type == Type::NativeFunc) {
      vector<Value> args;
      for(size_t i=1; i < n->children.size(); i++)
        args.push_back(eval(n->children[i].get()));
      return callee.cpp_func(args);
    }

    if(callee.type != Type::Func)
        throw runtime_error("not a function: " + calleeNode->lexeme);

    if(n->children.size()-1 != callee.params.size())
        throw runtime_error("argument count mismatch for function " + calleeNode->lexeme);

    env.push();
    for(size_t i=0; i<callee.params.size(); i++){
        Value arg = eval(n->children[i+1].get());
        env.define(callee.params[i], arg);
    }

    Value ret = Value::Nil();
    try {
        exec(callee.body);
    } catch (ReturnException& re) {
        ret = re.value;
    }
    env.pop();
    return ret;
}

Value Interpreter::eval_ternary(const Node* n){
  Value cond = eval(n->children[0].get());
  if(cond.truthy()){
    return eval(n->children[1].get());
  } else return eval(n->children[2].get());
}

Value Interpreter::coerceInt(const Value& v){
  switch (v.type) {
    case Int: return v;
    case Str: return Value::Int(v.s.empty() ? 0 : stoll(v.s));
    case Bool: return Value::Int(v.b ? 1 : 0);
    case Nil: return Value::Int(0);
        default: throw runtime_error("unknown coerce statement");

  }
  return Value::Int(0);
}

Value Interpreter::plus(const Value& a, const Value& b){
  if(a.type == Str || b.type == Str)
     return Value::String(a.toString() + b.toString());
   return Value::Int(coerceInt(a).i + coerceInt(b).i);
}

bool Interpreter::equals(const Value& a, const Value& b){
   if(a.type == Str && b.type == Str)
     return a.toString() == b.toString();
   if(a.type == Int && b.type == Int)
     return coerceInt(a).i == coerceInt(b).i;
   if(a.type == Bool && b.type == Bool)
     return a.truthy() == b.truthy();
  return false;
}
