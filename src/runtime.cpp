#include "runtime.h"
#include "ast.h"
#include "lexer.h"

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

string Value::toString() const {
  switch (type) {
    case Type::Int: return std::to_string(i);
    case Type::Str: return s;
    case Type::Bool: return b ? "true" : "false";
    case Type::Nil: return "nil";
  }
  return "nil";
}

bool Value::truthy() const {
  switch(type){
    case Type::Int: return i != 0;
    case Type::Str: return !s.empty();
    case Type::Bool: return b;
    case Type::Nil: return false;
  }
  return false;
}

void Env::push(){
  scopes.emplace_back();
}

void Env::pop(){
  if(!scopes.empty()) scopes.pop_back();
}

bool Env::define(const string& name, const Value& v){
  if(scopes.empty()) scopes.emplace_back();
   scopes.back()[name] = v;
   return true;
}

bool Env::assign(const string& name, const Value& v){
  for(auto it = scopes.rbegin(); it != scopes.rend(); it++){
    auto f = it->find(name);
    if(f != it->end()) { f->second = v; return true; }
  }
  return define(name, v);
}

optional<Value> Env::get(const string& name) const {
    for(auto it = scopes.rbegin(); it != scopes.rend(); it++){
      auto f = it->find(name);
      if(f != it->end()) { return f->second; }
    }
    return nullopt;
}

void Interpreter::run(const Node* root){
  if(!root || root->type != n_block) throw runtime_error("program must be a block");
  env.push();

    // define built-ins
    env.define("true",  Value::Int(1));
    env.define("false", Value::Int(0));
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
  const Node* expr = n->children[1].get();
  if(id->type != n_id) throw runtime_error("lhs must be an id");
  Value v = eval(expr);
  env.define(id->lexeme, v);
}

void Interpreter::exec_print(const Node* n){
  Value v = eval(n->children[0].get());
  *out << v.toString() << endl;  
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


Value Interpreter::eval(const Node* n){
  switch (n->type) {
    case n_num:    return Value::Int(stoll(n->lexeme));
    case n_str:    return Value::String(n->lexeme);
    case n_id:     return eval_id(n);
    case n_binary: return eval_binary(n);
    case n_unary:  return eval_unary(n);
    case n_assign: return eval_assign(n);

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
  if(lhs->type != n_id) throw std::runtime_error("assignment lhs must be identifier");
  Value v = eval(rhs);
  env.assign(lhs->lexeme, v);
  return v;
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

Value Interpreter::coerceInt(const Value& v){
  switch (v.type) {
    case Int: return v;
    case Str: return Value::Int(v.s.empty() ? 0 : stoll(v.s));
    case Bool: return Value::Int(v.b ? 1 : 0);
    case Nil: return Value::Int(0);
  }
  return Value::Int(0);
}

Value Interpreter::plus(const Value& a, const Value& b){
  if(a.type == Type::Str || b.type == Type::Str)
     return Value::String(a.toString() + b.toString());
   return Value::Int(coerceInt(a).i + coerceInt(b).i);
}

bool Interpreter::equals(const Value& a, const Value& b){
   if(a.type == Type::Str && b.type == Type::Str)
     return a.toString() == b.toString();
   if(a.type == Type::Int && b.type == Type::Int)
     return coerceInt(a).i == coerceInt(b).i;
   if(a.type == Type::Bool && b.type == Type::Bool)
     return a.truthy() == b.truthy();
  return true;
}
